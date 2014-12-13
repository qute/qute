
/**
 * `parser.c' - qute
 * copyright (c) 2014 - joseph.werle@gmail.com
 */

#include <stdlib.h>
#include <stdio.h>

#include "ast.h"
#include "lex.h"
#include "error.h"
#include "parser.h"

int
q_parser_init (q_parser_t *self, const char *name, const char *src) {
  q_lex_t *lex = NULL;
  int rc = 0;

  if (NULL == self) {
    return QE_PARSERNULL;
  }

  lex = (q_lex_t *) malloc(sizeof(q_lex_t));
  if (NULL == lex) {
    return QE_LEXMEM;
  }

  rc = q_lex_init(lex, name, src);
  if (rc > 0) {
    return rc;
  }

  self->lex = lex;
  self->name = name;
  self->src = src;

  return 0;
}

int
q_parse (q_parser_t *self, q_node_block_t *root) {
  int rc = 0;

  // ensure parser is initialized
  if (NULL == self) {
    return QE_PARSERMEM;
  }

  // ensure root block is initialized
  if (NULL == root) {
    rc = q_node_block_init(root);
    if (rc > 0) { return rc; }
  }

  while (0 == q_lex_scan(self->lex)) {
    q_lex_token_t token = self->lex->token;
    q_node_t *node = NULL;
    q_node_string_t *string = NULL;

    switch (token.type) {
      case QTOK_NONE:
        break;

      case QTOK_COMMA:
        break;

      case QTOK_STRING:
        rc = q_node_string_init(string, token.as.string);
        if (rc > 0) { return rc; }
        node = (q_node_t *) string;
        break;

      case QTOK_NUMBER:
        break;

      case QTOK_LPAREN:
        break;

      case QTOK_RPAREN:
        break;

      case QTOK_LBRACE:
        break;

      case QTOK_RBRACE:
        break;

      case QTOK_LBRACKET:
        break;

      case QTOK_RBRACKET:
        break;

      case QTOK_OPERATOR:
        break;

      case QTOK_IDENTIFIER:
        break;
    }

    // push to block
    rc = q_node_block_push(root, node);
    if (QE_OVERFLOW == rc) { return 1; }
  }

  return 0;
}
