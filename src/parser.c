
/**
 * `parser.c' - qute
 * copyright (c) 2014 - joseph.werle@gmail.com
 */

#include <stdlib.h>
#include <stdio.h>

#include "qute.h"

int
q_parser_init (q_parser_t *self, const char *name, const char *src) {
  if (NULL == self) {
    return QE_PARSERNULL;
  }

  self->name = name;
  self->src = src;

  return 0;
}

int
q_parse (q_parser_t *self, q_node_block_t *root) {
  q_lex_t lexer;
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

  rc = q_lex_init(&lexer, self->name, self->src);
  if (rc > 0) {
    return rc;
  }

  root->id = self->name;
  root->length = 0;

  while (0 == (rc = q_lex_scan(&lexer))) {
    q_node_identifier_t *identifier = NULL;
    q_node_operator_t *operator = NULL;
    q_node_string_t *string = NULL;
    q_node_number_t *number = NULL;
    q_node_token_t *token = NULL;
    q_node_hex_t *hex = NULL;
    q_node_t *node = NULL;

    switch (lexer.token.type) {
      case QTOK_NONE:
        break;

      case QTOK_STRING:
        if (NULL == q_node_alloc(string, string)) { return QE_PARSERMEM; }
        rc = q_node_string_init(string, lexer.token.as.string);
        if (rc > 0) { return rc; }
        node = (q_node_t *) string;
        break;

      case QTOK_NUMBER:
        if (NULL == q_node_alloc(number, number)) { return QE_PARSERMEM; }
        rc = q_node_number_init(number, lexer.token.as.number);
        if (rc > 0) { return rc; }
        node = (q_node_t *) number;
        break;

      case QTOK_HEX:
        if (NULL == q_node_alloc(hex, hex)) { return QE_PARSERMEM; }
        rc = q_node_hex_init(hex, lexer.token.as.string);
        if (rc > 0) { return rc; }
        node = (q_node_t *) hex;
        break;

      case QTOK_COMMA:
      case QTOK_LPAREN:
      case QTOK_RPAREN:
      case QTOK_LBRACE:
      case QTOK_RBRACE:
      case QTOK_LBRACKET:
      case QTOK_RBRACKET:
        if (NULL == q_node_alloc(token, token)) { return QE_PARSERMEM; }
        rc = q_node_token_init(token);
        if (rc > 0) { return rc; }
        node = (q_node_t *) token;
        break;

      case QTOK_OPERATOR:
        if (NULL == q_node_alloc(operator, operator)) { return QE_PARSERMEM; }
        rc = q_node_operator_init(operator);
        if (rc > 0) { return rc; }
        node = (q_node_t *) operator;
        break;

      case QTOK_IDENTIFIER:
        if (NULL == q_node_alloc(identifier, identifier)) { return QE_PARSERMEM; }
        rc = q_node_identifier_init(identifier);
        if (rc > 0) { return rc; }
        node = (q_node_t *) identifier;
        break;
    }

    if (NULL == node) {
      continue;
    }

    // set token
    node->token = lexer.token;
    node->as.string = node->token.as.string;
    node->as.number = node->token.as.number;

    // push to block
    q_node_block_push(root, node);
  }

  return 0;
}
