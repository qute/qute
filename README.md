qute
====

Simple lexer and parser API

## install

With clib:

```sh
$ clib install jwerle/qute
```

From source:

```sh
$ make install
```

## example

```c

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
  const char *src = "123 \"456\" 789\n" // number string number
                    "(a ,b, c, d)\n" // token id id id id token
                    "[0, 1]"; // bracket number number bracket

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
```

When compiled will yield:

```
(node [QNODE_NUMBER] '123')
(node [QNODE_STRING] '456')
(node [QNODE_NUMBER] '789')
(node [QNODE_TOKEN] '(')
(node [QNODE_IDENTIFIER] 'a')
(node [QNODE_TOKEN] ',')
(node [QNODE_IDENTIFIER] 'b')
(node [QNODE_TOKEN] ',')
(node [QNODE_IDENTIFIER] 'c')
(node [QNODE_TOKEN] ',')
(node [QNODE_IDENTIFIER] 'd')
(node [QNODE_TOKEN] ')')
(node [QNODE_TOKEN] '[')
(node [QNODE_NUMBER] '0')
(node [QNODE_TOKEN] ',')
(node [QNODE_NUMBER] '1')
(node [QNODE_TOKEN] ']')
```

## api

Coming soon...

See header files

## license

MIT
