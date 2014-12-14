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
q_program_t program;
q_parser_t parser;

const char *name = "test";
const char *src = "123 \"456\" 789\n" // string number string
                  "(a,b,c,d)" // token id id id id token
                  "[0,1"; // bracket number number bracket

// init
q_parser_init(&parser, name, src);

// parse
q_parse(&parser, &program);

// traverse
QAST_BLOCK_EACH(&program, q_node_t *node, {
  printf("(node [%s] '%s')\n",
    qnode_str(node->type),
    node->as.string);
});
```

## api

Coming soon...

See header files

## license

MIT
