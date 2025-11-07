#ifndef TOKEN_H_INCLUDED_
#define TOKEN_H_INCLUDED_

#include <stddef.h> /* size_t */
#include "../common/strview.h" /* StrView */

/* this enum is generated using X-macros, please refer to the included
 * file for the list of different token kinds and their descriptions. */
typedef enum {
#define XX(a) a,
#define XXKW(a, ...) a,
#include "defs/tokenkind_xmacros.defs"
#undef XX
#undef XXKW
} TokenKind;

typedef struct {
    size_t line;       /* line in the source code          */
    size_t col;        /* column in the source code        */
    StrView sv;        /* string view into the source code */
    TokenKind kind;    /* token kind (defined above)       */
} Token;

const char *token_kind_to_string(TokenKind tk);

Token token_make(TokenKind tk, size_t line, size_t col, const char *ptr, size_t len);

void token_debug(Token tok);

#endif /* TOKEN_H_INCLUDED */