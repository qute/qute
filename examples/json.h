
#ifndef Q_JSON_H
#define Q_JSON_H

#include <qute.h>

struct json_value;

enum {
  JSON_NULL,
  JSON_ARRAY,
  JSON_STRING,
  JSON_NUMBER,
  JSON_OBJECT,
  JSON_BOOLEAN,
};

enum {
  EJSON_OK = 0,
  EJSON_MEM,
  EJSON_PARSE,
  EJSON_TOKEN,
  EJSON_PARSERMEM,
};

#define JSON_MAX_VALUES 0xffff

#define JSON_VALUE_FIELDS                        \
  int type;                                      \
  int truthy;                                    \
  int errno;                                     \
  int arraylike;                                 \
  size_t size;                                   \
  const char *id;                                \
  q_node_t *current;                             \
  struct json_value *next;                       \
  struct json_value *prev;                       \
  struct json_value *parent;                     \
  struct json_value *values[JSON_MAX_VALUES];    \
  struct {                                       \
    const char *string;                          \
    float number;                                \
  } as;                                          \


typedef struct json_value {
  JSON_VALUE_FIELDS;
} json_value_t;

json_value_t *
json_parse (const char *, const char *);

char *
json_stringify (json_value_t *);

void
json_perror (json_value_t *);

#endif
