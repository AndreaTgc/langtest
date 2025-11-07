#include "lexer.h"
#include <stdbool.h> /* bool */
#include <ctype.h> /* isspace, isdigit, isalpha */
#include "../common/macros.h" /* ARRLEN */
#include <string.h> /* memcmp */

/* this array contains the language keywords, since the set of keywords
 * is relatively small there is no reason to use perfect hashing just yet
 * we can simply do a linear seach over the entries */
static const struct {
    const char *kw ;
    TokenKind tk;
} keywords[] = {
#define XX(...)
#define XXKW(a, b) { #b, a },
#include "defs/tokenkind_xmacros.defs"
#undef XX
#undef XXKW
};

/* Advances the lexer by one char */
static void advance(Lexer *lex);
/* Looks at the current char in the lexer */
static char peek(Lexer *lex);
/* Returns the current char and advances by one */
static char consume(Lexer *lex);
/* Looks at the char at current pos + offset */
static char peek_offset(Lexer *lex, int offset);
/* advances the lexer only if peek matches a char */
static bool match(Lexer *lex, char c);
/* Skips the chars until the next line in the source */
static void skip_line(Lexer *lex);
/* Skips all the consecutive spaces starting from the current position */
static void skip_whitespace(Lexer *lex);
/* Skips all the non meaningful characters at the start */
static void skip_whitespace_and_comments(Lexer *lex);
/* Lexes a number that can either be a float or integer literal */
static Token lex_number(Lexer *lex);
/* Lexes a string literal */
static Token lex_string(Lexer *lex);
/* Lexes an identifier or keyword */
static Token lex_ident_or_keyword(Lexer *lex);

Token lexer_next_token(Lexer *lex)
{
    /* skip all whitespaces and comments to get to the first meaningful
     * next character in the source code. */
    skip_whitespace_and_comments(lex);
    /* save the starting point of the new token, this is done to display
     * the source position and string of the token during debugging and
     * error reporting. */
    size_t line = lex->line;
    size_t col = lex->col;
    const char *start = &lex->source.ptr[lex->pos];
    /* check if we reached the end of the source code */
    if (peek(lex) == '\0') {
        return token_make(TOK_EOF, line, col, start, 0);
    }
    if (isalpha(peek(lex))) {
        return lex_ident_or_keyword(lex);
    }
    /* check if the current position points to a number */
    if (isdigit(peek(lex))) {
        return lex_number(lex);
    }
    /* check if the current position points to a string */
    if (peek(lex) == '"') {
        return lex_string(lex);
    }
    /* handle single / multiple chars symbols (operators) */
    switch (consume(lex)) {
    /* Start checking for multi-char tokens, each starting character can
     * lead to one or more different tokens depending on the following
     * characters. */
    case '!':
        if (match(lex, '=')) {
            return token_make(TOK_NOTEQUAL, line, col, start, 2);
        }
        return token_make(TOK_BANG, line, col, start, 1);

    case '=':
        if (match(lex, '=')) {
            return token_make(TOK_DOUBLEEQUAL, line, col, start, 2);
        }
        return token_make(TOK_EQUAL, line, col, start, 1);

    case '>':
        if (match(lex, '=')) {
            return token_make(TOK_GTE, line, col, start, 2);
        }
        return token_make(TOK_GT, line, col, start, 1);

    case '<':
        if (match(lex, '=')) {
            return token_make(TOK_LTE, line, col, start, 2);
        }
        return token_make(TOK_LT, line, col, start, 1);

    case ':':
        if (match(lex, ':')) {
            return token_make(TOK_DOUBLECOLON, line, col, start, 2);
        }
        return token_make(TOK_COLON, line, col, start, 1);

    /* check for single character tokens, this section containst mostly
     * punctuation and math related tokens. */
    /* *INDENT-OFF* */
    case '+': return token_make(TOK_PLUS, line, col, start, 1);
    case '-': return token_make(TOK_MINUS, line, col, start, 1);
    case '*': return token_make(TOK_STAR, line, col, start, 1);
    case '/': return token_make(TOK_SLASH, line, col, start, 1);
    case '%': return token_make(TOK_PERCENT, line, col, start, 1);
    case '(': return token_make(TOK_LPAREN, line, col, start, 1);
    case ')': return token_make(TOK_RPAREN, line, col, start, 1);
    case '[': return token_make(TOK_LBRACKET, line, col, start, 1);
    case ']': return token_make(TOK_RBRACKET, line, col, start, 1);
    case '{': return token_make(TOK_LBRACE, line, col, start, 1);
    case '}': return token_make(TOK_RBRACE, line, col, start, 1);
    case ';': return token_make(TOK_SEMICOLON, line, col, start, 1);
    case ',': return token_make(TOK_COMMA, line, col, start, 1);
    case '.': return token_make(TOK_DOT, line, col, start, 1);
    /* we didn't find any valid char, return back an invalid token
     * to signal the syntax error. */
    default: return token_make(TOK_INVALID, line, col, start, 1);
    /* *INDENT-ON* */
    }
}

static void advance(Lexer *lex)
{
    char c = peek(lex);
    if (c == '\0')
        return;
    if (c == '\n') {
        lex->col = 1;
        lex->line++;
    } else {
        lex->col++;
    }
    lex->pos++;
}

static char peek(Lexer *lex)
{
    return lex->source.ptr[lex->pos];
}

static char consume(Lexer *lex)
{
    /* return the peeked char and advance by one */
    char c = peek(lex);
    advance(lex);
    return c;
}

static char peek_offset(Lexer *lex, int offset)
{
    return lex->source.ptr[lex->pos + offset];
}

static bool match(Lexer *lex, char c)
{
    if (peek(lex) != c) {
        return false;
    }
    advance(lex);
    return true;
}

static void skip_line(Lexer *lex)
{
    while (peek(lex) != '\n' && peek(lex) != '\0') {
        advance(lex);
    }
    if (peek(lex) == '\n') {
        advance(lex);
    }
}

static void skip_whitespace(Lexer *lex)
{
    while (isspace(peek(lex))) {
        advance(lex);
    }
}

static void skip_whitespace_and_comments(Lexer *lex)
{
begin:
    skip_whitespace(lex);
    if (peek(lex) == '#') {
        skip_line(lex);
        goto begin;
    }
}

static Token lex_number(Lexer *lex)
{
    size_t col = lex->col;
    size_t line = lex->line;
    const char *start = &lex->source.ptr[lex->pos]; /* initial ptr */
    TokenKind tk = TOK_INT_LIT; /* defaults to int at start */
    while (isdigit(peek(lex))) {
        advance(lex);
    }
    if (peek(lex) == '.' && isdigit(peek_offset(lex, 1))) {
        /* lexing a float literal */
        tk = TOK_FLOAT_LIT;
        advance(lex);
        while (isdigit(peek(lex))) {
            advance(lex);
        }
    }

    return token_make(tk, line, col, start, &lex->source.ptr[lex->pos] - start);
}

static Token lex_string(Lexer *lex)
{
    advance(lex); /* skip initial quotes */
    size_t col = lex->col;
    size_t line = lex->line;
    const char *start = &lex->source.ptr[lex->pos];
    while (peek(lex) != '"' && peek(lex) != '\0') {
        advance(lex);
    }
    const char *end = &lex->source.ptr[lex->pos];
    advance(lex); /* skip ending quotes */
    return token_make(TOK_STRING_LIT, line, col, start, end - start);
}

static Token lex_ident_or_keyword(Lexer *lex)
{
    size_t col = lex->col;
    size_t line = lex->line;
    const char *start = &lex->source.ptr[lex->pos];
    TokenKind kind = TOK_IDENTIFIER; /* defaults as identifier */
    while (isalnum(peek(lex)) || peek(lex) == '_') {
        advance(lex);
    }
    size_t len = &lex->source.ptr[lex->pos] - start;
    /* check if the identifier is a keyword or not */
    for (size_t i = 0; i < ARRLEN(keywords); i++) {
        if (!memcmp(start, keywords[i].kw, len)) {
            kind = keywords[i].tk;
        }
    }
    return token_make(kind, line, col, start, len);
}