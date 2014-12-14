
/**
 * `test/ast.c' - qute
 * copyright (c) 2014 - joseph.werle@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <ok/ok.h>

#include <qute.h>

#define E(m, ...) ({                   \
  fprintf(stderr, "E: ");              \
  fprintf(stderr, m, ##__VA_ARGS__);  \
  fprintf(stderr, "\n");               \
})

int
main (void) {
  q_node_t *node = NULL;
  int rc = 0;

  if (NULL == q_node_alloc(, node)) {
    E("alloc: %s", qerror_str[rc]);
    return 1;
  }

  if ((rc = q_node_init(node)) > 0) {
    E("init: %s", qerror_str[rc]);
    return 1;
  }

  //printf("%s\n", node.id);
  //assert("" == node.id);

  return 0;
}
