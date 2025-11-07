#ifndef STRVIEW_H_INCLUDED_
#define STRVIEW_H_INCLUDED_

#include <stddef.h> /* size_t */
#include <stdbool.h> /* bool */

#define STRVIEW_FMT "%.*s"

#define STRVIEW_FMT_PARAMS(name) (int)name.len, name.ptr

typedef struct {
    const char* ptr;
    size_t len;
} StrView;

/* Creates a new string view from a ptr + len, this allows to get slices
 * into a standard C string. */
StrView strview_make(const char* sptr, size_t len);

/* Takes a null-terminated string and creates a string view that spans over
 * the entire length of the string. */
StrView strview_from_cstr(const char* sptr);

/* Heap allocates a new null-terminated string that copies the slice that is
 * inside the stringview */
char* strview_to_cstr_alloc(StrView sv);

/* Compares two string views in a lexicographic way. (like memcmp) */
int strview_compare(StrView sv1, StrView sv2);

/* Checks if two stirng views are equal, meaning same length and contents */
bool strview_equals(StrView sv1, StrView sv2);

/* Checks if a string view start with another string view */
bool strview_starts_with(StrView sv1, StrView sv2);

/* Checks if a string view start with a defined substring */
bool strview_starts_with_cstr(StrView sv, const char *sptr);

#endif /* STRVIEW_H_INCLUDED_ */