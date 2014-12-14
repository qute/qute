
/**
 * `ast.h' - qute
 * copyright (c) 2014 - joseph.werle@gmail.com
 */

#ifndef QUTE_AST_H
#define QUTE_AST_H

#include <stdlib.h>
#include "lex.h"

/**
 * node structures.
 */

struct q_node;
struct q_node_as_s;
struct q_node_block;
struct q_node_token;
struct q_node_string;
struct q_node_number;
struct q_node_operator;
struct q_node_identifier;
struct q_node_expression;

#define q_node_ q_node

/**
 * program node aliased to `q_node_block'.
 */

#define q_program q_node_block
#define q_program_t q_node_block_t

/**
 * max children parser nodes per block.
 */

#define QMAX_BLOCK_NODES 0xffff

/**
 * ast block node ierator.
 */

#define QAST_BLOCK_EACH(block, node, body) ({    \
  for (int i = 0; i < (block)->length; ++i)      \
  { node = (block)->nodes[i]; (body); }          \
})

/**
 * base parser node fields.
 */

#define Q_NODE_FIELDS        \
  const char *id;            \
  q_node_type_t type;        \
  q_lex_token_t token;       \
  struct q_node *prev;       \
  struct q_node *next;       \
  struct q_node_as_s as;

/**
 * node type def.
 */

#define QNODE_TYPES          \
  X(QNODE_NULL),             \
  X(QNODE_BLOCK),            \
  X(QNODE_TOKEN),            \
  X(QNODE_STRING),           \
  X(QNODE_NUMBER),           \
  X(QNODE_OPERATOR),         \
  X(QNODE_IDENTIFIER),       \

/**
 * node enum types.
 */

typedef enum {
#define X(x) x
  QNODE_TYPES
#undef X
} q_node_type_t;

static char *qnode_str[] = {
#define X(x) # x
  QNODE_TYPES
#undef X
};

struct q_node_as_s {
  const char *string;
  float number;
};

/**
 * parser base node.
 */

typedef struct q_node {
  Q_NODE_FIELDS;
} q_node_t;

/**
 * parser token node.
 */

typedef struct q_node_token {
  Q_NODE_FIELDS;
} q_node_token_t;

/**
 * parser block node.
 */

typedef struct q_node_block {
  Q_NODE_FIELDS;
  q_node_t *nodes[QMAX_BLOCK_NODES];
  size_t length;
} q_node_block_t;

/**
 * parser string node.
 */

typedef struct q_node_string {
  Q_NODE_FIELDS;
  size_t length;
} q_node_string_t;

/**
 * parser number node.
 */

typedef struct q_node_number {
  Q_NODE_FIELDS;
} q_node_number_t;

/**
 * parser operator node.
 */

typedef struct q_node_operator {
  Q_NODE_FIELDS;
} q_node_operator_t;

/**
 * parser identifier node.
 */

typedef struct q_node_identifier {
  Q_NODE_FIELDS;
} q_node_identifier_t;

/**
 * allocates parser node.
 */

#define q_node_alloc(T, n) \
  (n = (NULL != n ? n :    \
   (struct q_node_##T *) malloc(sizeof(struct q_node_ ##T))))

/**
 * initializes parser node.
 */

int
q_node_init (q_node_t *);

/**
 * initializes parser block node.
 */

int
q_node_block_init (q_node_block_t *);

/**
 * initializes parser string node.
 */

int
q_node_string_init (q_node_string_t *, const char *);

/**
 * initializes parser number node.
 */

int
q_node_number_init (q_node_number_t *, float);

/**
 * initializes parser operator node.
 */

int
q_node_operator_init (q_node_operator_t *);

/**
 * initializes parser identifier node.
 */

int
q_node_identifier_init (q_node_identifier_t *);

/**
 * initializes parser token node.
 */

int
q_node_token_init (q_node_token_t *);

/**
 * pushes a node onto a block.
 */

int
q_node_block_push (q_node_block_t *, q_node_t *);

#endif
