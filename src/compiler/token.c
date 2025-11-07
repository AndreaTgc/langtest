#include "token.h"
#include <stdio.h>

const char *token_kind_as_string(TokenKind tk)
{
    switch (tk) {
#define XX(a) case a: return #a;
#define XXKW(a, ...) case a: return #a;
#include "defs/tokenkind_xmacros.defs"
    default:
        return "<unknown token>";
#undef XX
#undef XXKW
    }
}

Token token_make(TokenKind tk, size_t line, size_t col, const char *ptr, size_t len)
{
    Token ret;
    ret.col = col;
    ret.line = line;
    ret.kind = tk;
    ret.sv = strview_make(ptr, len);
    return ret;
}

void token_debug(Token tok)
{
    const char *tk_str = token_kind_as_string(tok.kind);
    printf("[ %s | " STRVIEW_FMT " | %zu:%zu]\n",
           tk_str, STRVIEW_FMT_PARAMS(tok.sv),tok.line, tok.col);
}