#include "strview.h" /* StrView */
#include <stdlib.h> /* malloc */
#include <string.h> /* memcmp, strlen */

StrView strview_make(const char *sptr, size_t len)
{
    StrView ret;
    ret.len = len;
    ret.ptr = sptr;
    return ret;
}

StrView strview_from_cstr(const char *sptr)
{
    StrView ret;
    ret.len = strlen(sptr);
    ret.ptr = sptr;
    return ret;
}

char* strview_to_cstr_alloc(StrView sv)
{
    char *ret = malloc(sv.len);
    if (!ret) {
        return NULL;
    }
    memcpy(ret, sv.ptr, sv.len);
    ret[sv.len] = '\0';
    return ret;
}

int strview_compare(StrView sv1, StrView sv2)
{
    size_t cmplen = sv1.len >= sv2.len ? sv2.len : sv1.len;
    return memcmp(sv1.ptr, sv2.ptr, cmplen);
}

bool strview_equals(StrView sv1, StrView sv2)
{
    return strview_compare(sv1, sv2) == 0;
}

bool strview_starts_with(StrView sv1, StrView sv2)
{
    if (sv2.len > sv1.len) {
        return false;
    }
    return memcmp(sv1.ptr, sv2.ptr, sv2.len) == 0;
}

bool strview_starts_with_cstr(StrView sv, const char *sptr)
{
    size_t len = strlen(sptr);
    if (len > sv.len) {
        return false;
    }
    return memcmp(sv.ptr, sptr, len) == 0;
}