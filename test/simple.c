
/**
 * `test/simple.c' - qute
 * copyright (c) 2014 - joseph.werle@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fs/fs.h>
#include <ok/ok.h>

#include <qute.h>

int
main (void) {
  const char *name = "simple";
  const char *src = fs_read("./test/simple.txt");
  int index = 0;

  q_program_t program;
  q_parser_t parser;

  memset(&program, 0, sizeof(program));

  // init
  if (q_parser_init(&parser, name, src) > 0) {
    return 1;
  }

  // parse
  if (q_parse(&parser, &program) > 0) {
    return 1;
  }

  QAST_BLOCK_EACH(&program, q_node_t *node, {
      char buf[BUFSIZ];
      switch (index++) {
        case 0: // 123
          assert(QNODE_NUMBER == node->type);
          break;

        case 1: // 456
          assert(QNODE_NUMBER == node->type);
          break;

        case 2: // "bradley the kinkajou"
          assert(QNODE_STRING == node->type);
          break;

        case 3: // a
          assert(QNODE_IDENTIFIER == node->type);
          break;

        case 4: // b
          assert(QNODE_IDENTIFIER == node->type);
          break;

        case 5: // (
          assert(QNODE_TOKEN == node->type);
          break;

        case 6: // 123
          assert(QNODE_NUMBER == node->type);
          break;

        case 7: // ,
          assert(QNODE_TOKEN == node->type);
          break;

        case 8: // "456"
          assert(QNODE_STRING == node->type);
          break;

        case 9: // ,
          assert(QNODE_TOKEN == node->type);
          break;

        case 10: // 789
          assert(QNODE_NUMBER == node->type);
          break;

        case 11: // )
          assert(QNODE_TOKEN == node->type);
          break;

        case 12: // 543.210
          assert(QNODE_NUMBER == node->type);
          break;
      }

      sprintf(buf, "node[%s]: `%s'",
        qnode_str[node->type],
        node->as.string);
      ok(buf);
  });

  ok_done();
  return 0 == ok_count();
}
