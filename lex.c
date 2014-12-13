
/**
 * `lex.c' - qute
 * copyright (c) 2014 - joseph.werle@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex.h"
#include "ast.h"
#include "error.h"

#define peek(self) self->src[self->offset]

static unsigned char
next (q_lex_t *self) {
  unsigned char ch = 0;

  // current character
  ch = self->src[self->offset];

  // eof
  if (self->offset == self->length - 1) {
    return '\0';
  }

  self->ch = ch;
  self->offset++;

  return ch;
}

static void
token (q_lex_t *self, q_lex_tok_t type, const char *string) {
  self->token.type = type;
  self->token.colno = self->colno;
  self->token.lineno = self->lineno;
  self->token.as.string = string;

  if (QTOK_NUMBER == type) {
    self->token.as.number = atof(string);
  }
}

static int
scan_string (q_lex_t *self, unsigned char ch) {
  unsigned char quote = ch;
  unsigned char buf[BUFSIZ];
  size_t size = 0;
  int ignore = 0;

  // ensure ch is actually a quote
  if ('"' != quote || '\'' != quote) {
    return QE_LEXTOKEN;
  }

  for (;;) {
    ch = next(self);
    ignore = 0;

    if ('\\' == ch && ('"' == peek(self) || '\'' == peek(self))) {
      buf[size++] = ch;
      ch = next(self);
      ignore = 1;
    }

    if (quote == ch && 0 == ignore) {
      break;
    }

    if ('\n' == ch || '\r' == ch) {
      return QE_LEXTOKEN;
    }

    buf[size++] = ch;
  }

  buf[size] = '\0';
  self->colno -= size;
  token(self, QTOK_STRING, (char *) buf);

  return 0;
}

static int
scan_identifier (q_lex_t *self, unsigned char ch) {
  return 0;
}

void
q_lex_token_init (q_lex_t *self, q_lex_token_t *token, q_lex_tok_t type) {
  token->type = type;
  token->lineno = self->lineno;
}

int
q_lex_init (q_lex_t *self, const char *name, const char *src) {
  if (NULL == self) {
    return QE_LEXNULL;
  }

  self->src = src;
  self->name = name;
  self->colno = 1;
  self->lineno = 1;
  self->offset = 0;
  self->length = strlen(src);

  return 0;
}

int
q_lex_scan (q_lex_t *self) {
  unsigned char ch = 0;
scan:
  ch = next(self);
  switch (ch) {
    // EOF
    case '\0':
      return 1;

    // white space and tabs
    case ' ': case '\t':
      goto scan;

    // handle user defined comment character
#ifdef QCHAR_COMMENT
    case QCHAR_COMMENT:
      while (ch != '\n' && ch != '\r') { ch = next(self); }
      goto scan;
#endif

    // handle newlines
    case '\n': case '\r':
      self->lineno++;
      self->colno = 1;
      goto scan;

    // scan quoted string
    case '"': case '\'':
      return scan_string(self, ch);

    case ',':
      token(self, QTOK_COMMA, "ch");
      break;

    case '(':
      token(self, QTOK_LPAREN, "(");
      break;

    case ')':
      token(self, QTOK_RPAREN, ")");
      break;

    case '{':
      token(self, QTOK_LBRACE, "{");
      break;

    case '}':
      token(self, QTOK_RBRACE, "}");
      break;

    case '[':
      token(self, QTOK_LBRACKET, "[");
      break;

    case ']':
      token(self, QTOK_RBRACKET, "]");
      break;

    case '+': case '-': case '*': case '/':
    case '=': case '<': case '>': case '&':
    case '%': case '~': case '?':
      token(self, QTOK_OPERATOR, (const char *) &ch);
      break;

    default:
      return scan_identifier(self, ch);
  }

  return 0;
}
