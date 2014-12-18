
/**
 * `lex.h' - qute
 * copyright (c) 2014 - joseph.werle@gmail.com
 */

#ifndef QUTE_LEX_H
#define QUTE_LEX_H

#include <stdlib.h>

struct q_lex;
struct q_lex_token;

/**
 * token types def.
 */

#define QLEX_TOKEN_TYPES \
  X(QTOK_NONE),          \
  X(QTOK_COMMA),         \
  X(QTOK_STRING),        \
  X(QTOK_NUMBER),        \
  X(QTOK_LPAREN),        \
  X(QTOK_RPAREN),        \
  X(QTOK_LBRACE),        \
  X(QTOK_RBRACE),        \
  X(QTOK_LBRACKET),      \
  X(QTOK_RBRACKET),      \
  X(QTOK_OPERATOR),      \
  X(QTOK_IDENTIFIER),    \

/**
 * token types.
 */

typedef enum {
#define X(x) x
  QLEX_TOKEN_TYPES
#undef X
} q_lex_tok_t;

/**
 * lexer token structure.
 */

typedef struct q_lex_token {
  q_lex_tok_t type;
  int lineno;
  int colno;
  struct {
    const char *string;
    float number;
  } as;
} q_lex_token_t;

/**
 * lexer structure.
 */

typedef struct q_lex {
  int colno;
  int lineno;
  int offset;
  size_t length;
  const char *src;
  const char *name;
  unsigned char ch;
  q_lex_token_t token;
} q_lex_t;

/**
 * initializes lexer token with type.
 */

void
q_lex_token_init (q_lex_t *, q_lex_token_t *, q_lex_tok_t);

/**
 * initializes lexer.
 */

int
q_lex_init (q_lex_t *, const char *, const char *);

/**
 * scans next lexer token.
 */

int
q_lex_scan (q_lex_t *);

#endif
