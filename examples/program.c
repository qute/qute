
#include <qute.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fs/fs.h>

#define usage "usage: program <file>"

#define EQ(a, b) (0 == strcmp(a, b))

#define PSTATEMENT_FIELDS \
  int type;               \
  const char *id;         \
  q_node_t *node;         \
  struct {                \
    float number;         \
    const char *string;   \
  } value;

#define PSTACK_FIELDS                                                \
  struct p_assignment_stmt_s *assignments[QMAX_BLOCK_NODES];         \
  struct p_function_stmt_s  *functions[QMAX_BLOCK_NODES];            \
  size_t assignment_length;                                          \
  size_t function_length;                                            \

#define peek(p) p->nodes[p->cursor]
#define next(p) p->nodes[p->cursor++]

#define stack_push_assignment(stack, a) \
  stack->assignments[stack->assignment_length++] = a

#define stack_push_function(stack, f) \
  stack->functions[stack->function_length++] = f

#define ThrowSyntaxError(p, n) ({                                    \
  char tmp[BUFSIZ];                                                  \
  sprintf(tmp, "Unexpected token `%s'", n->as.string);               \
  qthrow(p, n, "SyntaxError", tmp);                                  \
})

#define ThrowReferenceError(p, n) ({                                 \
  char tmp[BUFSIZ];                                                  \
  sprintf(tmp, "`%s' is not defined", n->as.string);                 \
  qthrow(p, n, "ReferenceError", tmp);                               \
})


enum {
  TYPE_STRING,
  TYPE_NUMBER,
  TYPE_FUNCTION,
  TYPE_FUNCTION_NATIVE,
};

struct p_stack_s;
struct p_function_stmt_s;
struct p_assignment_stmt_s;
struct p_expression_stmt_s;
struct p_function_arguments_s;

typedef void * (p_function_t) (struct p_function_arguments_s *);

typedef struct p_stack_s {
  PSTACK_FIELDS;
} p_stack_t;

typedef struct p_function_stmt_s {
  PSTATEMENT_FIELDS;
  q_node_t *nodes[QMAX_BLOCK_NODES];
  q_node_block_t scope;
  p_function_t *native;
  struct p_stack_s stack;
  size_t length;
  int cursor;
} p_function_stmt_t;

typedef struct p_assignment_stmt_s {
  PSTATEMENT_FIELDS;
} p_assignment_stmt_t;

typedef struct p_expression_stmt_s {
  PSTATEMENT_FIELDS;
  struct p_expression_stmt_s *expressions[QMAX_BLOCK_NODES];
  size_t expressions_length;
} p_expression_stmt_t;

typedef struct p_function_arguments_s {
  q_node_block_t *scope;
  p_stack_t *stack;
  q_node_t *nodes[QMAX_BLOCK_NODES];
  char *values[QMAX_BLOCK_NODES];
  size_t length;
  int cursor;
} p_function_arguments_t;

q_program_t *program = NULL;
p_stack_t *heap = NULL;

int
p_call_function (q_node_block_t *, p_stack_t *, p_function_stmt_t *, q_node_t *);

int
p_assignment_stmt_new (q_node_block_t *, p_stack_t *, const char *, q_node_t *);

static p_assignment_stmt_t *
stack_find_assignment (p_stack_t *stack, const char *id) {
  int i = 0;

#define find                                                         \
  for (i = 0; i < STACK->assignment_length; ++i) {                   \
    p_assignment_stmt_t *assignment = STACK->assignments[i];         \
    if (assignment && EQ(id, assignment->id)) { return assignment; } \
  }

#define STACK stack
  find
#undef STACK

#define STACK heap
  find
#undef STACK

#undef find

  return NULL;
}

static p_function_stmt_t *
stack_find_function (p_stack_t *stack, const char *id) {

#define find                                               \
  for (int i = 0; i < STACK->function_length; ++i) {       \
    p_function_stmt_t *function = STACK->functions[i];     \
    if (EQ(id, function->id)) { return function; }         \
  }

#define STACK stack
  find
#undef STACK

#define STACK heap
  find
#undef STACK

#undef find

  return NULL;
}

static int
is_function (p_stack_t *stack, q_node_t *node) {
  return NULL != stack_find_function(stack, node->as.string);
}

static int
is_variable (p_stack_t *stack, q_node_t *node) {
  return NULL != stack_find_assignment(stack, node->as.string);
}

int
p_function_stmt_new (q_node_block_t *block, p_stack_t *stack, q_node_t *node) {
  int ends = 1;
  int parens = 1;
  q_node_t *next = NULL;
  p_function_stmt_t *function =
    (p_function_stmt_t *) malloc(sizeof(p_function_stmt_t));

  if (NULL == function) {
    return 1;
  }

  if (!EQ("def", node->as.string)) {
    return 1;
  }

  if (QNODE_IDENTIFIER != peek(block)->type) {
    ThrowSyntaxError(block, peek(block));
  }

  function->id = next(block)->as.string;
  function->type = TYPE_FUNCTION;
  function->cursor = 0;
  function->length = 0;
  function->node = node;

  if ('(' == next(block)->as.string[0]) {
    while (1) {
      node = next(block);
      if ('(' == node->as.string[0]) {
        parens++;
      } else if (')' == node->as.string[0]) {
        if (1 == parens) { break; }
        parens--;
      } else if (QNODE_IDENTIFIER == node->type) {
        p_assignment_stmt_new(&(function->scope),
                              &(function->stack),
                              node->as.string, node);
      }
    }
  }

  while (1) {
    node = next(block);

    if (EQ("end", node->as.string)) {
      if (1 == ends) {
        break;
      } else { ends--; }
    } else if (EQ("def", node->as.string)) {
      ends++;
    }

    if (function->length < QMAX_BLOCK_NODES) {
      function->nodes[function->length++] = node;
    } else {
      qthrow(block, node, "NodeOverflow", "Maximum node stack exceeded");
    }
  }

  stack_push_function(stack, function);
  return 0;
}

int
p_function_native_new (q_node_block_t *block,
                       p_stack_t *stack,
                       const char *id,
                       p_function_t *fn) {

  p_function_stmt_t *function =
    (p_function_stmt_t *) malloc(sizeof(p_function_stmt_t));

  function->id = id;
  function->type = TYPE_FUNCTION_NATIVE;
  function->native = fn;

  stack_push_function(stack, function);
  return 0;
}

int
p_assignment_stmt_new (q_node_block_t *block, p_stack_t *stack, const char *id, q_node_t *node) {
  p_assignment_stmt_t *existing = NULL;
  p_assignment_stmt_t *tmp = NULL;
  p_assignment_stmt_t *assignment =
    (p_assignment_stmt_t *) malloc(sizeof(p_assignment_stmt_t));

  if (NULL == assignment) {
    return 1;
  }

  existing = stack_find_assignment(stack, id);
  if (NULL != existing) {
    free(assignment);
    assignment = existing;
  }

  assignment->value.string = strdup(node->as.string);
  assignment->value.number = node->as.number;

  if (QNODE_STRING == node->type) {
    assignment->type = TYPE_STRING;
  } else if (QNODE_NUMBER == node->type) {
    assignment->type = TYPE_NUMBER;
  } else if (QNODE_IDENTIFIER == node->type) {
    tmp = stack_find_assignment(stack, node->as.string);
    if (NULL != tmp) {
      assignment->value.string = tmp->value.string;
      assignment->value.number = tmp->value.number;
      assignment->type = tmp->type;
    } else {
      ThrowReferenceError(block, node);
    }
  } else {
    ThrowSyntaxError(block, node);
  }

  assignment->id = id;
  stack_push_assignment(stack, assignment);
  return 0;
}

int
p_expression_stmt_new (q_node_block_t *block, p_stack_t *stack, q_node_t **nodes) {
  return 0;
}

int
p_parse (q_node_block_t *block, p_stack_t *mem) {
  q_node_t *node = NULL;
  p_stack_t *scope;

  scope = mem;

  do {
    node = next(block);
    if (NULL == node) { break; }
    //printf("[%s] %s\n", qnode_str[node->type], node->as.string);
    switch (node->type) {
      case QNODE_NULL:
      case QNODE_BLOCK:
      case QNODE_TOKEN:
      case QNODE_STRING:
      case QNODE_NUMBER:
      case QNODE_OPERATOR:
        break;

      case QNODE_IDENTIFIER:
        if (peek(block) && QNODE_OPERATOR == peek(block)->type &&
            '=' == peek(block)->as.string[0]) {
          if (0 != p_assignment_stmt_new(block,
                                         scope,
                                         node->as.string,
                                         peek(block)->next)) {
            qthrow(block, node, "Error", "Failed to create assignment");
          }
        } else if (EQ("def", node->as.string)) {
          if (0 != p_function_stmt_new(block, scope, node)) {
            qthrow(block, node, "Error", "Failed to create function");
          }
        } else if (is_function(scope, node)) {
          p_function_stmt_t *fn = stack_find_function(scope, node->as.string);
          if (0 != p_call_function(block, scope, fn, node)) {
            return 1;
          }
        } else if (is_variable(scope, node)) {
        } else {
          if ('(' == node->next->as.string[0]) {
            ThrowReferenceError(block, node);
          }
        }
        break;
    }
  } while (node);
  return 0;
}

int
p_call_function (q_node_block_t *block,
                 p_stack_t *stack,
                 p_function_stmt_t *fn,
                 q_node_t *node) {

  int parens = 1;
  q_node_block_t fnblock;
  p_function_arguments_t args;

  memset(&fnblock, 0, sizeof(fnblock));
  memset(&fn->stack, 0, sizeof(fn->stack));
  memset(&args, 0, sizeof(args));

  args.stack = stack;
  if (EQ("def", node->as.string)) {
    node = node->next->next;
  } else {
    node = node->next;
  }

  if ('(' != node->as.string[0]) {
    ThrowSyntaxError(block, node);
  }

  while (1) {
    node = node->next;
    if (NULL == node) { break; }
    if (')' == node->as.string[0]) {
      if (1 == parens) {
        break;
      } else {
        parens--;
      }
    } else if ('(' == node->as.string[0]) {
      parens++;
    } else if (',' != node->as.string[0]) {
      args.values[args.length] = (char *) node->as.string;
      if (QNODE_IDENTIFIER == node->type) {
        p_assignment_stmt_t *a = NULL;
        a = stack_find_assignment(stack, node->as.string);
        if (NULL == a) {
          a = stack_find_assignment(heap, node->as.string);
        }

        if (a) {
          if (a->value.string) {
            args.values[args.length] = (char *) a->value.string;
          }
        } else {
          ThrowReferenceError(program, node);
        }
      }
      args.nodes[args.length++] = node;
    }
  }

  if (TYPE_FUNCTION_NATIVE == fn->type) {
    fn->native(&args);
  } else {
    memcpy(fnblock.nodes, fn->nodes, sizeof(fn->nodes));
    fnblock.length = fn->length;
    return p_parse(&fnblock, &(fn->stack));
  }

  return 0;
}

void *
p_print (p_function_arguments_t *arguments) {
  size_t length = arguments->length;
  int i = 0;
  for (; i < length; ++i) {
    q_node_t *node = arguments->nodes[i];
    char *value = arguments->values[i];
    if (QNODE_STRING == node->type ||
        QNODE_NUMBER == node->type ||
        QNODE_IDENTIFIER == node->type) {

      printf("%s", (char *) value);

      if (i < length -1) {
        printf(" ");
      }
    }
  }

  printf("\n");
  return NULL;
}

void *
p_sleep (p_function_arguments_t *arguments) {
  unsigned int seconds = 0;
  if (1 != arguments->length) {
    qthrow(arguments->scope,
           (q_node_t *) arguments->scope,
           "InvalidArguments",
           "`sleep()': Expecting 1 argument");
  }

  seconds = (int) arguments->nodes[0]->as.number;
  sleep(seconds);
  return NULL;
}

int
p_init_native (q_node_block_t *block, p_stack_t *stack) {
  p_function_native_new(block, stack, "print", p_print);
  p_function_native_new(block, stack, "sleep", p_sleep);
  return 0;
}

int
main (int argc, char **argv) {
  q_parser_t parser;
  char *name = NULL;
  char *src = NULL;
  int rc = 0;
  int node_offset = 0;

  if (argc < 2) {
    printf("%s\n", usage);
    return 1;
  }

  name = argv[1];

  if (0 != fs_exists(name)) {
    printf("E: NotFound: `%s'\n", name);
    return 1;
  }

  q_node_alloc(block, program);

  heap = (p_stack_t *) malloc(sizeof(p_stack_t));

  src = fs_read(name);

  // init
  if ((rc = q_parser_init(&parser, name, src)) > 0) {
    printf("error: init %s\n", qerror_str[rc]);
    return 1;
  }

  // parse
  if ((rc = q_parse(&parser, program)) > 0) {
    printf("error: parse %s\n", qerror_str[rc]);
    return 1;
  }

  p_init_native(program, heap);

  rc = p_parse(program, heap);

  // run main if defined
  p_function_stmt_t *fn = stack_find_function(heap, "main");
  if (NULL != fn) {
    rc = p_call_function(program, heap, fn, fn->node);
  }

  return rc;
}
