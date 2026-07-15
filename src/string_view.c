#include "string_view.h"
#include <string.h>

String_View sv_from_cstr(const char *cstr) {
    return (String_View) {
	.data = cstr,
	.len = strlen(cstr),
    };
}
