
/**
 * `parser.h' - qute
 * copyright (c) 2014 - joseph.werle@gmail.com
 */

#ifndef QUTE_PARSER_H
#define QUTE_PARSER_H

#include "lex.h"
#include "ast.h"

struct q_parser;

/**
 * token parser.
 */

typedef struct q_parser {
  const char *src;
  const char *name;
  q_lex_opts_t lexopts;
  q_lex_t *lex;
} q_parser_t;

/**
 * initializes a parser with filename and soruce
 */

int
q_parser_init (q_parser_t *, const char *, const char *);

/**
 * parse current parser state into block node
 */

int
q_parse (q_parser_t *, q_node_block_t *);

#endif
