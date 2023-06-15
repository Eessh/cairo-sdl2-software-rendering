#pragma once

#include <stdbool.h>

struct result {
  bool ok;
  union {
    const char *error;
  };
};

typedef struct result result;

#define DEFINE_RESULT_WITH_TYPE_AND_NAME(result_type, result_name)             \
  typedef struct result_name {                                                 \
    bool ok;                                                                   \
    union {                                                                    \
      result_type value;                                                       \
      const char *error;                                                       \
    };                                                                         \
  } result_name;
