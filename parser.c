
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
  while (0 == (rc = q_lex_scan(self->lex))) {
    q_node_identifier_t *identifier = NULL;
    q_node_operator_t *operator = NULL;
    q_node_string_t *string = NULL;
    q_node_number_t *number = NULL;
    q_node_token_t *token = NULL;
    q_node_block_t *block = NULL;
    q_node_t *node = NULL;

    switch (self->lex->token.type) {
      case QTOK_NONE:
        break;

      case QTOK_STRING:
        string = (q_node_string_t *) malloc(sizeof(q_node_string_t));
        rc = q_node_string_init(string, self->lex->token.as.string);
        if (rc > 0) { return rc; }
        node = (q_node_t *) string;
        break;

      case QTOK_NUMBER:
        number = (q_node_number_t *) malloc(sizeof(q_node_number_t));
        rc = q_node_number_init(number, self->lex->token.as.number);
        if (rc > 0) { return rc; }
        node = (q_node_t *) number;
        break;

      case QTOK_COMMA:
      case QTOK_LPAREN:
      case QTOK_RPAREN:
      case QTOK_LBRACE:
      case QTOK_RBRACE:
      case QTOK_LBRACKET:
      case QTOK_RBRACKET:
        token = (q_node_token_t *) malloc(sizeof(q_node_token_t));
        rc = q_node_token_init(token);
        if (rc > 0) { return rc; }
        node = (q_node_t *) token;
        break;

      case QTOK_OPERATOR:
        operator = (q_node_operator_t *) malloc(sizeof(q_node_operator_t));
        rc = q_node_operator_init(operator);
        if (rc > 0) { return rc; }
        node = (q_node_t *) operator;
        break;

      case QTOK_IDENTIFIER:
        identifier = (q_node_identifier_t *) malloc(sizeof(q_node_identifier_t));
        rc = q_node_identifier_init(identifier);
        if (rc > 0) { return rc; }
        node = (q_node_t *) identifier;
        break;
    }

    if (NULL == node) {
      continue;
    }

    // set token
    node->token = self->lex->token;
    node->as.string = node->token.as.string;
    node->as.number = node->token.as.number;

    // push to block
    q_node_block_push(root, node);
  }

  return 0;
}
