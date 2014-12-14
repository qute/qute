
/**
 * `lex.c' - qute
 * copyright (c) 2014 - joseph.werle@gmail.com
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex.h"
#include "ast.h"
#include "error.h"

#define peek(self) self->src[self->offset]
#define EQ(a, b) (0 == strcmp(a, b))

static unsigned char
next (q_lex_t *self) {
  unsigned char ch = 0;

  // current character
  ch = self->src[self->offset];

  // eof
  if (self->offset == self->length) {
    return '\0';
  }

  self->ch = ch;
  self->offset++;

  return ch;
}

static unsigned char
prev (q_lex_t *self) {
  unsigned char ch = 0;
  int idx = self->offset - 1;
  if (idx < 0) {
    self->offset = 0;
    self->colno = 1;
    self->ch = self->src[0];
    return self->src[0];
  } else {
    self->offset--;
    if (self->colno < 3) {
      self->colno = 1;
    } else {
      self->colno--;
    }

    self->ch = self->src[idx];
    return self->src[idx];
  }
}

static void
token (q_lex_t *self, q_lex_tok_t type, const char *string) {
  self->token.type = type;
  self->token.colno = self->colno;
  self->token.lineno = self->lineno;
  self->token.as.string = strdup(string);

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
  if ('"' != quote && '\'' != quote) {
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
  unsigned char buf[BUFSIZ];
  size_t size = 0;
  int num = 0;
  int set = 0;
  int i = 0;

  do {
#ifdef QCHAR_COMMENT
    if (QCHAR_COMMENT == ch) {
      while (ch != '\n' && ch != '\r') { ch = next(self); }
    }
#endif
    buf[size++] = ch;
    ch = next(self);
  } while (isalpha(ch) || isdigit(ch) || '_' == ch || '.' == ch);

  prev(self);

  if (0 == size) {
    return QE_LEXTOKEN;
  }

  buf[size] = '\0';
  num = 1;

  for (; i < size; ++i) {
    if (!isdigit(buf[i]) && '.' != buf[i]) {
      num = 0;
    }
  }

#define SET_TOKEN_IF(str, tok)                             \
  if (0 == set && EQ((char * ) str, (char *) buf)) {       \
    set = 1; token(self, tok, (char *) buf);               \
  }

  if (num) {
    token(self, QTOK_NUMBER, (char *) buf);
  } else {
    SET_TOKEN_IF(buf, QTOK_IDENTIFIER);
  }

#undef SET_TOKEN_IF
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
      return -1;

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
      token(self, QTOK_COMMA, ",");
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

#undef peek
#undef EQ
