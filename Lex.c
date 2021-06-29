#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "Lex.h"
#include "Util.h"

static INT GetParenType(CHAR ch);
static LPTOKEN BaseToken(UCHAR uchTokenKind, USHORT nLine, USHORT nCol);
static LPTOKEN IntToken(INT iValue, USHORT nLine, USHORT nCol);
static LPTOKEN FloatToken(float fValue, USHORT nLine, USHORT nCol);
static LPTOKEN StrToken(UCHAR uchTokenKind, LPSTR lpszStr, USHORT nLine, USHORT nCol);
static CHAR CurChar(LPLEXER lpLexer);
static CHAR const* CurCharPtr(LPLEXER lpLexer);
static CHAR PeekChar(LPLEXER lpLexer);
static BOOL IsIdChar(CHAR ch);
static void NextChar(LPLEXER lpLexer);
static void SkipWhiteSpace(LPLEXER lpLexer);
static LPTOKEN LexString(LPLEXER lpLexer, LPERROR lpErr);
static LPTOKEN LexNumber(LPLEXER lpLexer, LPERROR lpErr);
static LPTOKEN LexIdentifier(LPLEXER lpLexer, LPERROR lpErr);
static LPTOKEN LexSymbolOrQuote(LPLEXER lpLexer, LPERROR lpErr);

void __cdecl InitLexer(LPLEXER lpLexer, LPCSTR lpszSrcCode) {
    lpLexer->lpszSrcCode = lpszSrcCode;
    lpLexer->uPos = 0;
    lpLexer->nLine = 1;
    lpLexer->nCol = 1;
    lpLexer->pCachedToken = NULL;
}

LPTOKEN __cdecl NextToken(LPLEXER lpLexer, LPERROR lpErr) {
    CHAR ch;

    if (lpLexer->pCachedToken) {
        Token *ret = lpLexer->pCachedToken;
        lpLexer->pCachedToken = NULL;
        return ret;
    }

    SkipWhiteSpace(lpLexer);
    switch (ch = CurChar(lpLexer)) {
    case '\0': {
        return NULL;
    }
    case '(': case '[': case '{': {
        Token *token = BaseToken(TK_LPAREN, lpLexer->nLine, lpLexer->nCol);
        token->value.iValue = GetParenType(ch);
        NextChar(lpLexer);
        return token;
    }
    case ')': case ']': case '}': {
        Token *token = BaseToken(TK_RPAREN, lpLexer->nLine, lpLexer->nCol);
        token->value.iValue = GetParenType(ch);
        NextChar(lpLexer);
        return token;
    }
    case '.': {
        Token *token = BaseToken(TK_DOT, lpLexer->nLine, lpLexer->nCol);
        NextChar(lpLexer);
        return token;
    }
    case '\'': {
        return LexSymbolOrQuote(lpLexer, lpErr);
    }
    case '"': {
        return LexString(lpLexer, lpErr);
    }
    case '1': case '2': case '3': case '4': case '5': case '6':
    case '7': case '8': case '9': case '0': case '+': case '-': {
        return LexNumber(lpLexer, lpErr);
    }
    default: {
        return LexIdentifier(lpLexer, lpErr);
    }
    }
}

LPTOKEN __cdecl PeekToken(LPLEXER lpLexer, LPERROR lpErr) {
    if (!lpLexer->pCachedToken) {
        lpLexer->pCachedToken = NextToken(lpLexer, lpErr);
    }
    return lpLexer->pCachedToken;
}

void __cdecl DropToken(Token *token) {
    switch (token->uchTokenKind) {
    case TK_STRING:
    case TK_QUOTE:
    case TK_SYMBOL:
    case TK_ID:
        HeapFree(GetProcessHeap(), 0, (LPVOID)token->value.lpszStrValue);
        break;
    }
    HeapFree(GetProcessHeap(), 0, (LPVOID)token);
}

static INT GetParenType(CHAR ch) {
    switch (ch) {
    case '(': case ')': return TK_P_PAREN;
    case '[': case ']': return TK_P_BRACKET;
    case '{': case '}': return TK_P_BRACE;
    default: return -1;
    }
}

static LPTOKEN BaseToken(UCHAR uchTokenKind, USHORT nLine, USHORT nCol) {
    Token *token = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Token));
    token->uchTokenKind = uchTokenKind;
    token->nLine;
    token->nCol;
    return token;
}

static LPTOKEN IntToken(INT iValue, USHORT nLine, USHORT nCol) {
    Token *token = BaseToken(TK_INT, nLine, nCol);
    token->value.iValue = iValue;
    return token;
}

static LPTOKEN FloatToken(float fValue, USHORT nLine, USHORT nCol) {
    Token *token = BaseToken(TK_NUMBER, nLine, nCol);
    token->value.fValue = fValue;
    return token;
}

static LPTOKEN StrToken(UCHAR uchTokenKind, LPSTR lpszStr, USHORT nLine, USHORT nCol) {
    Token *token = BaseToken(uchTokenKind, nLine, nCol);
    token->value.lpszStrValue = lpszStr;
    return token;
}

static CHAR CurChar(LPLEXER lpLexer) {
    return lpLexer->lpszSrcCode[lpLexer->uPos];
}

static CHAR const* CurCharPtr(LPLEXER lpLexer) {
    return lpLexer->lpszSrcCode + lpLexer->uPos;
}

static CHAR PeekChar(LPLEXER lpLexer) {
    return lpLexer->lpszSrcCode[lpLexer->uPos + 1];
}

static void NextChar(LPLEXER lpLexer) {
    lpLexer->uPos += 1;
    if (CurChar(lpLexer) == '\n') {
        lpLexer->nLine += 1;
        lpLexer->nCol = 1;
    } else {
        lpLexer->nCol += 1;
    }
}

static BOOL IsIdChar(CHAR ch) {
    if (isalpha((int)ch) || isdigit((int)ch)) {
        return TRUE;
    }
    switch (ch) {
    case '-': case '+': case '.': case '/': case '?': case '!':
    case '@': case '$': case '%': case '^': case '*': case '&':
    case '<': case '>': case ':': case '|': case '\\': case '=':
    case '_': case '~':
        return TRUE;
    default:
        return FALSE;
    }
}

static void SkipWhiteSpace(LPLEXER lpLexer) {
    while (isspace((int)CurChar(lpLexer))) {
        NextChar(lpLexer);
    }
}

static LPTOKEN LexString(LPLEXER lpLexer, LPERROR lpErr) {
    USHORT nLine = lpLexer->nLine;
    USHORT nCol = lpLexer->nCol;
    CHAR const *pcStart, *pcEnd;
    CHAR ch;

    NextChar(lpLexer);
    pcStart = CurCharPtr(lpLexer);
    ch = CurChar(lpLexer);
    while (ch != '"' && ch != '\0') {
        NextChar(lpLexer);
        ch = CurChar(lpLexer);
    }
    if (ch == '\0') {
        FormatError(lpErr, CreateSourceInfo(NULL, nLine, nCol),
                    LEX_ERR_UNTERMINATED_STRING,
                    "unterminated string: unexpected end of file");
        return NULL;
    }
    pcEnd = CurCharPtr(lpLexer);
    NextChar(lpLexer);
    return StrToken(TK_STRING, CreateStrFromSlice(pcStart, pcEnd), nLine, nCol);
}

static LPTOKEN LexNumber(LPLEXER lpLexer, LPERROR lpErr) {
    USHORT nLine = lpLexer->nLine;
    USHORT nCol = lpLexer->nCol;
    CHAR const *pcStart, *pcEnd;
    CHAR ch = CurChar(lpLexer);
    Token *token;
    LPSTR lpszNum;

    pcStart = CurCharPtr(lpLexer);
    while (isdigit((int)ch)) {
        NextChar(lpLexer);
        ch = CurChar(lpLexer);
    }

    if (ch == '.') {
        float fValue;
        NextChar(lpLexer);
        while (isdigit((int)ch)) {
            NextChar(lpLexer);
            ch = CurChar(lpLexer);
        }
        pcEnd = CurCharPtr(lpLexer);
        lpszNum = CreateStrFromSlice(pcStart, pcEnd);
        fValue = (float)atof(lpszNum);
        HeapFree(GetProcessHeap(), 0, lpszNum);
        token = FloatToken(fValue, nLine, nCol);
    } else {
        int iValue;
        pcEnd = CurCharPtr(lpLexer);
        lpszNum = CreateStrFromSlice(pcStart, pcEnd);
        iValue = atoi(lpszNum);
        HeapFree(GetProcessHeap(), 0, lpszNum);
        token = IntToken(iValue, nLine, nCol);
    }

    if (!isspace((int)ch) && ch != '(' && ch != ')') {
        FormatError(lpErr, CreateSourceInfo(NULL, lpLexer->nLine, lpLexer->nCol),
                    LEX_WARN_UNEXPECTED_AFTER_NUM,
                    "unexpected character '%c' after number", ch);
    }
    return token;
}

#define KEYWORD_CNT 8

static const LPCSTR KeywordStrings[KEYWORD_CNT] = {
    "define",
    "lambda",
    "cond",
    "else",
    "if",
    "and",
    "or",
    "case"
};

static const UCHAR KeywordValues[KEYWORD_CNT] = {
    TK_DEFINE,
    TK_LAMBDA,
    TK_COND,
    TK_ELSE,
    TK_IF,
    TK_AND,
    TK_OR,
    TK_CASE
};

static LPTOKEN LexIdentifier(LPLEXER lpLexer, LPERROR lpErr) {
    USHORT nLine = lpLexer->nLine;
    USHORT nCol = lpLexer->nCol;
    UCHAR i = 0;
    CHAR const *pcStart, *pcEnd;
    CHAR ch = CurChar(lpLexer);
    pcStart = CurCharPtr(lpLexer);
    while (IsIdChar(ch)) {
        NextChar(lpLexer);
        ch = CurChar(lpLexer);
    }
    pcEnd = CurCharPtr(lpLexer);
    for (; i < KEYWORD_CNT; i++) {
        if (!strncmp(pcStart, KeywordStrings[i], pcEnd - pcStart)) {
            return BaseToken(KeywordValues[i], nLine, nCol);
        }
    }

    return StrToken(TK_ID, CreateStrFromSlice(pcStart, pcEnd), nLine, nCol);
}

static LPTOKEN LexSymbolOrQuote(LPLEXER lpLexer, LPERROR lpErr) {
    USHORT nLine = lpLexer->nLine;
    USHORT nCol = lpLexer->nCol;
    if (IsIdChar(PeekChar(lpLexer))) {
        CHAR const *pcStart, *pcEnd;
        CHAR ch = CurChar(lpLexer);
        pcStart = CurCharPtr(lpLexer);
        while (IsIdChar(ch)) {
            NextChar(lpLexer);
            ch = CurChar(lpLexer);
        }
        pcEnd = CurCharPtr(lpLexer);
        return StrToken(TK_SYMBOL, CreateStrFromSlice(pcStart, pcEnd), nLine, nCol);
    } else {
        return BaseToken(TK_QUOTE, nLine, nCol);
    }
}
