
#include <stdio.h>
#include <string.h>
#include <qute.h>

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
                    "5 - 5\n"; // 0

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

  // traverse
  QAST_BLOCK_EACH(&program, q_node_t *node, {
    if (QNODE_OPERATOR == node->type) {
      printf("%s\n", node->as.string);
    }
  });

  return 0;
}
