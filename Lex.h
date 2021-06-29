#pragma once

#include <wtypes.h>
#include "Error.h"

enum e_token_kind {
    TK_LPAREN = 0,
    TK_RPAREN = 1,
    TK_STRING = 2,
    TK_INT    = 3,
    TK_NUMBER = 4,
    TK_QUOTE  = 5,
    TK_SYMBOL = 6,
    TK_ID     = 7,
    TK_DOT    = 8,

    TK_DEFINE = 9,
    TK_LAMBDA = 10,
    TK_COND   = 11,
    TK_ELSE   = 12,
    TK_IF     = 13,
    TK_AND    = 14,
    TK_OR     = 15,
    TK_CASE   = 16
};

enum e_paren_kind {
    TK_P_PAREN   = 0,
    TK_P_BRACKET = 1,
    TK_P_BRACE   = 2
};

typedef union u_token_value {
    INT32 iValue;
    float fValue;
    LPSTR lpszStrValue;
} TokenValue;

typedef struct st_token {
    TokenValue value;
    USHORT nLine;
    USHORT nCol;
    UCHAR uchTokenKind;
} Token;

typedef Token* LPTOKEN;
typedef Token const* LPCTOKEN;

enum e_lex_err_codes {
    /* Errors */
    LEX_ERR_UNTERMINATED_STRING = 1201,

    /* Warnings */
    LEX_WARN_UNEXPECTED_AFTER_NUM = 4201
};

typedef struct st_lexer {
    LPCSTR lpszSrcCode;
    UINT uPos;
    USHORT nLine;
    USHORT nCol;
    Token *pCachedToken;
} Lexer;

typedef Lexer* LPLEXER;
typedef Lexer const* LPCLEXER;

extern void __cdecl InitLexer(LPLEXER lpLexer, LPCSTR lpszSrcCode);
extern LPTOKEN __cdecl NextToken(LPLEXER lpLexer, LPERROR lpErr);
extern LPTOKEN __cdecl PeekToken(LPLEXER lpLexer, LPERROR lpErr);
extern void __cdecl DropLexer(LPLEXER lpLexer);
extern void __cdecl DropToken(LPTOKEN lpToken);
