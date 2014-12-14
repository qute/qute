
/**
 * `error.h' - qute
 * copyright (c) 2014 - joseph.werle@gmail.com
 */

#ifndef QUTE_ERROR_H
#define QUTE_ERROR_H

enum {

  QE_OVERFLOW = -1,
  QE_LOK = 0,

  /**
   * lex errors.
   */

  QE_LEXMEM,
  QE_LEXNULL,
  QE_LEXTOKEN,

  /**
   * parser errors.
   */

  QE_PARSERMEM,
  QE_PARSERNULL,

  /**
   * ast errors.
   */

  QE_ASTNODEMEM,
  QE_NODE_NULL,

};

#endif
