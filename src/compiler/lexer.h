#ifndef LEXER_H_INCLUDED_
#define LEXER_H_INCLUDED_

#include <stddef.h> /* size_t */
#include "token.h" /* Token */
#include "../common/strview.h" /* StrView */

typedef struct {
    StrView source;  /* string view into the source code */
    size_t pos;      /* current position in the view     */
    size_t line;     /* current source line              */
    size_t col;      /* current source column            */
} Lexer;

/* Gets the next token from the source file, each token represent a syntax
 * construct like numeric literals, strings, identifiers etc.
 * when the end of the source file is reached, the lexer returns a token of
 * kind TOK_EOF. */
Token lexer_next_token(Lexer *lex);

#endif /* LEXER_H_INCLUDED_ */