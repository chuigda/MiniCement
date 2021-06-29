#include "Parse.h"
#include "Lex.h"

#include <windows.h>

typedef struct st_parse_context {
    LPLEXER lpLexer;
    LPCMTOPLEVEL lpCmTopLevel;
    LPCMLIST lpLastCmList;
} ParseContext;

typedef ParseContext *LPCTX;

static void InitParseContext(LPCSTR lpszSrcCode);

LPCMTOPLEVEL __cdecl ParseProgram(LPCSTR lpszSrcCode, 
                                  UCHAR uchParseBufSize,
                                  UCHAR uchErrBufSize,
                                  UCHAR *uchErrBufUsage,
                                  LPERROR lpErrBuf[]) {
    return NULL;
}
