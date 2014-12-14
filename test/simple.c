
/**
 * `test/simple.c' - qute
 * copyright (c) 2014 - joseph.werle@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>

#include <fs/fs.h>

#include <qute.h>

int
main (void) {
  const char *name = "simple";
  const char *src = fs_read("./test/simple.txt");
  q_parser_t parser;
  q_program_t program;

  // init
  if (q_parser_init(&parser, name, src) > 0) {
    return 1;
  }

  // parse
  if (q_parse(&parser, &program) > 0) {
    return 1;
  }

  QAST_BLOCK_EACH(&program, q_node_t *node, {
    printf("token: %s\n", node->token.as.string);
  });

  return 0;
}
