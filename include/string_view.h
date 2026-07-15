#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include <stddef.h>

typedef struct {
  const char *data;
  size_t len;

} String_View;

String_View sv_from_cstr(const char *cstr);

#endif
