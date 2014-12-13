
/**
 * `lex.h' - qute
 * copyright (c) 2014 - joseph.werle@gmail.com
 */

#ifndef QUTE_LEX_H
#define QUTE_LEX_H

struct q_lex_token;
struct q_lex;

/**
 * token types.
 */

typedef enum {
  QTOK_NONE = 0,
  QTOK_COMMA,
  QTOK_STRING,
  QTOK_NUMBER,
  QTOK_LPAREN,
  QTOK_RPAREN,
  QTOK_LBRACE,
  QTOK_RBRACE,
  QTOK_LBRACKET,
  QTOK_RBRACKET,
  QTOK_OPERATOR,
  QTOK_IDENTIFIER,
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
