
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
q_node_init (q_node_t **self) {
  if (NULL == self) {
    *self = (q_node_t *) malloc(sizeof(q_node_t));
  }

  if (NULL == self) {
    return QE_ASTNODEMEM;
  }

  return 0;
}

int
q_node_block_init (q_node_block_t *self) {
  int rc = q_node_init((q_node_t **) &self);

  if (rc > 0) {
    return rc;
  }

  self->type = QNODE_BLOCK;

  return 0;
}

int
q_node_string_init (q_node_string_t *self, const char *string) {
  int rc = q_node_init((q_node_t **) &self);

  if (rc > 0) {
    return rc;
  }

  self->type = QNODE_STRING;
  if (NULL != string) {
    self->length = strlen(string);
    self->as.string = string;
  }

  return 0;
}

int
q_node_block_push (q_node_block_t *self, q_node_t *node) {
  if (self->length < QMAX_BLOCK_NODES) {
    self->nodes[self->length++] = node;
    return self->length;
  }

  return QE_OVERFLOW;
}
