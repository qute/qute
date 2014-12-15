qute
====

Simple lexer and parser API

## install

You can install **qute** serveral ways.

As part of a [clib](https://github.com/clibs/clib) package:

```sh
$ clib install qute/qute
```

As a system static library using
[ghi](https://github.com/stephenmathieson/ghi):

```sh
$ ghi qute/qute
```

Or from source:

```sh
$ make
$ make install
```

## linking

You can link the libary if installed from source or with
[ghi](https://github.com/stephenmathieson/ghi):

```sh
$ cc -lqute program.c -o program
```

```c
#include <qute.h>
```

If you've installed **qute** with [clib](https://github.com/clibs/clib):
then just include the source in compilation and include as such:

```sh
$ cc -Ideps $(ls deps/qute/*.c) program.c -o program
```

```c
#include <qute/qute.h>
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

## example program

Using the [program example](/examples/program.c)
with the following syntax we can parse and run a simple program that has
functions, variables, scope, and a few built ins like the `print()` and
`sleep` functions.

Variables are defiend with the `=` operator. Functions are defined with
the `def` operator and use the `end` operator to signal the end of the
function body. Functions are called with the `()`. Currently function
arguments are not supported unless passed to a built in like `print` or
`sleep`.

```ruby
name = "werle"

def hello ()
  print("hello", name)
end

def goodbye ()
  print("goodbye", name)
end

def main ()
  hello()
  sleep(2)
  goodbye()
end
```

The above code can be parsed and executed by the `program` binary compiled with
the `make examples/program` command. Much like C, a `main` is
automatically executed. However, it is optional here. The program should
output `hello werle`, sleep for **2** seconds, and then output
`goodbye werle` before exiting.

```sh
$ make
$ make examples/program
$ ./examples/program ./examples/program.txt
hello werle
goodbye werle
```

## api

Coming soon...

See header files

## license

MIT
