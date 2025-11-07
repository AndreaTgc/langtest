#include "lexer.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char **argv)
{
    const char *path = argv[1];
    FILE* f = fopen(path, "r");
    if (!f) return 1;
    fseek(f, 0, SEEK_END);
    size_t bytes = ftell(f);
    rewind(f);
    char *source = calloc(bytes, sizeof(char));
    fread(source, bytes, 1, f);
    Lexer lex;
    lex.col = 1;
    lex.pos = 0;
    lex.line = 1;
    lex.source = strview_make(source, bytes);
    Token tok;
    do {
        tok = lexer_next_token(&lex);
        token_debug(tok);
    } while (tok.kind != TOK_EOF);

    return 0;
}