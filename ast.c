
/**
 * `ast.c' - qute
 * copyright (c) 2014 - joseph.werle@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "error.h"

int
q_node_init (q_node_t *self) {
  if (NULL == self) {
    self = (q_node_t *) malloc(sizeof(q_node_t));
  }

  if (NULL == self) {
    return QE_ASTNODEMEM;
  }

  return 0;
}

int
q_node_block_init (q_node_block_t *self) {
  int rc = q_node_init((q_node_t *) &self);

  if (rc > 0) {
    return rc;
  }

  if (NULL == self) {
    return QE_NODE_NULL;
  }

  self->type = QNODE_BLOCK;

  return 0;
}

int
q_node_string_init (q_node_string_t *self, const char *string) {
  int rc = q_node_init((q_node_t *) &self);

  if (rc > 0) {
    return rc;
  }

  if (NULL == self) {
    return QE_NODE_NULL;
  }

  self->type = QNODE_STRING;
  if (NULL != string) {
    self->length = strlen(string);
    self->as.string = string;
  }

  return 0;
}

int
q_node_number_init (q_node_number_t *self, float number) {
  int rc = q_node_init((q_node_t *) &self);
  char str[BUFSIZ];

  if (rc > 0) {
    return rc;
  }

  if (NULL == self) {
    return QE_NODE_NULL;
  }

  sprintf(str, "%f", number);

  self->type = QNODE_NUMBER;
  self->as.number = number;
  self->as.string = strdup(str);

  return 0;
}

/**
 * initializes parser operator node.
 */

int
q_node_operator_init (q_node_operator_t *self) {
  int rc = q_node_init((q_node_t *) &self);
  if (rc > 0) {
    return rc;
  }

  self->type = QNODE_OPERATOR;

  return 0;
}

/**
 * initializes parser identifier node.
 */

int
q_node_identifier_init (q_node_identifier_t *self) {
  return 0;
}

/**
 * initializes parser expression node.
 */

int
q_node_expression_init (q_node_expression_t *self) {
  return 0;
}

int
q_node_token_init (q_node_token_t *self) {
  return 0;
}

int
q_node_block_push (q_node_block_t *self, q_node_t *node) {
  if (self->length < QMAX_BLOCK_NODES) {
    // link
    if (self->length > 1) {
      self->nodes[self->length -1]->next = node;
      node->prev = self->nodes[self->length -1];
    }

    // push
    self->nodes[self->length++] = node;
    return self->length;
  }

  return QE_OVERFLOW;
}
