
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
  const char *src = "123 \"456\" 789\n" // string number string
    "(a,b,c,d)\n" // token id id id id token
    "[0,1]"; // bracket number number bracket

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
      printf("(node [%s] '%s')\n",
        qnode_str[node->type],
        node->as.string);
  });

  return 0;
}
