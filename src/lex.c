
/**
 * `lex.c' - qute
 * copyright (c) 2014 - joseph.werle@gmail.com
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strdup/strdup.h"
#include "qute.h"

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
  self->colno++;

  return ch;
}

static unsigned char
prev (q_lex_t *self) {
  int idx = self->offset - 1;
  if (idx < 0) {
    self->offset = 0;
    self->colno = 1;
    self->ch = self->src[0];
    return self->src[0];
  } else {
    self->offset--;
    if (2 == self->colno) {
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
  self->token.as.number = 0;

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

  if (self->offset < self->length) {
    prev(self);
  }

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
  token->colno = self->colno;
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

  memset(&self->opts, 0, sizeof(self->opts));

  return 0;
}

int
q_lex_scan (q_lex_t *self) {
  unsigned char ch = 0;
  char tmp[BUFSIZ];

  // initialize options
  if (0 == self->opts.ch.space) {
    self->opts.ch.space = ' ';
  }

  if (0 == self->opts.ch.tab) {
    self->opts.ch.tab = '\t';
  }

  if (0 == self->opts.ch.newline) {
    self->opts.ch.newline = '\n';
  }

  if (0 == self->opts.ch.creturn) {
    self->opts.ch.creturn = '\r';
  }

  if (0 == self->opts.ch.dquote) {
    self->opts.ch.dquote = '"';
  }

scan:
  memset(tmp, 0, BUFSIZ);
  ch = next(self);


  // handle constant tokens
  switch (ch) {
  // EOF
    case '\0':
      return -1;

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
    case '%': case '~': case '?': case '.':
    case '^': case '|': case '!': case '`':
    case ';': case ':':
      sprintf(tmp, "%c", ch);
      token(self, QTOK_OPERATOR, (const char *) tmp);
      break;

    default:

      // white space and tabs
      if (ch == self->opts.ch.space || ch == self->opts.ch.tab) {
        goto scan;
      }

      // handle comments
      if (ch == self->opts.ch.comment) {
        while (ch != '\n' && ch != '\r') { ch = next(self); }
        goto scan;
      }

      // handle newlines
      if (ch == self->opts.ch.newline || ch == self->opts.ch.creturn) {
        self->lineno++;
        self->colno = 1;
        goto scan;
      }

      // scan quoted string
      if (ch == self->opts.ch.dquote || ch == self->opts.ch.squote) {
        return scan_string(self, ch);
      }

      return scan_identifier(self, ch);
  }

  return 0;
}

#undef peek
#undef EQ
