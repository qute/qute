
#include <qute.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void
throw (q_program_t *program, q_node_t *node, const char *name, const char *message) {
  char trace[BUFSIZ];

  sprintf(trace, "at %s:%d:%d",
      program->id, node->token.lineno, node->token.colno);

  fprintf(stderr, "%s: %s\n\t%s\n",
      name, message, trace);

  exit(1);
}

int
main (void) {
  int rc = 0;

  q_program_t program;
  q_parser_t parser;

  memset(&program, 0, sizeof(program));

  const char *name = "test";
  const char *src = "10 + 10\n" // 20
                    "8 * 2\n" // 16
                    "10 / 2\n" // 5
                    "5 - 5"; // 0

  // init
  if ((rc = q_parser_init(&parser, name, src)) > 0) {
    printf("error: init %s\n", qerror_str[rc]);
    return 1;
  }

  // parse
  if ((rc = q_parse(&parser, &program)) > 0) {
    printf("error: parse %s\n", qerror_str[rc]);
    return 1;
  }

  program.id = name;

#define ThrowSyntaxError(msg) throw(&program, node, "SyntaxError", msg)
  // traverse
  QAST_BLOCK_EACH(&program, q_node_t *node, {
    q_node_number_t *left = (q_node_number_t *) node->prev;
    q_node_number_t *right = (q_node_number_t *) node->next;
    float value = 0;

    if (QNODE_OPERATOR == node->type) {
      if (QNODE_NUMBER != right->type) {
        ThrowSyntaxError("Unexpected token. Expecting number");
      }

      switch (node->as.string[0]) {
        case '+':
          if (QNODE_NUMBER != left->type) {
            value = right->as.number;
          } else {
            value = left->as.number + right->as.number;
          }
          break;

        case '-':
          if (QNODE_NUMBER != left->type) {
            value = right->as.number;
          } else {
            value = left->as.number - right->as.number;
          }
          break;

        case '*':
          if (QNODE_NUMBER != left->type) {
            ThrowSyntaxError("Unexpected token *");
          } else {
            value = left->as.number * right->as.number;
          }
          break;

        case '/':
          if (QNODE_NUMBER != left->type) {
            ThrowSyntaxError("Unexpected token /");
          } else {
            value = left->as.number / right->as.number;
          }
          break;
      }

      if (value == (int) value) {
        printf("[%d] %d\n", node->token.lineno, (int) value);
      } else {
        printf("[%d] %2.f\n", node->token.lineno, value);
      }
    }

  });
#undef ThrowSyntaxError

  return 0;
}
