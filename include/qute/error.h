
/**
 * `error.h' - qute
 * copyright (c) 2014 - joseph.werle@gmail.com
 */

#ifndef QUTE_ERROR_H
#define QUTE_ERROR_H

#define QERROR_TYPES         \
  X(QE_LOK),                 \
  X(QE_OVERFLOW),            \
  X(QE_LEXMEM),              \
  X(QE_LEXNULL),             \
  X(QE_LEXTOKEN),            \
  X(QE_PARSERMEM),           \
  X(QE_PARSERNULL),          \
  X(QE_ASTNODEMEM),          \
  X(QE_NODE_NULL),           \

enum {
#define X(x) x
  QERROR_TYPES
#undef X
};

void
qthrow (q_program_t *, q_node_t *, const char *, const char *);

#endif
