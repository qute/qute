

#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "error.h"

void
qthrow (q_node_block_t *block, q_node_t *node, const char *name, const char *message) {
  char trace[BUFSIZ];

  sprintf(trace, "at %s:%d:%d",
      block->id, node->token.lineno, node->token.colno);

  fprintf(stderr, "%s: %s\n\t%s\n",
      name, message, trace);

  exit(1);
}
