#pragma once

#include <wtypes.h>
#include "Syntax.h"

enum e_parse_error_codes {
    /* Errors */
    PARSE_ERR_GENERIC  = 1401,

    /* Warnings */
    PARSE_WARN_GENERIC = 4401,

    /* Info */
    PARSE_INFO         = 6000
};

extern LPCMTOPLEVEL __cdecl ParseProgram(LPCSTR lpszSrcCode, 
                                         UCHAR uchParseBufSize,
                                         UCHAR uchErrBufSize,
                                         UCHAR *uchErrBufUsage,
                                         LPERROR lpErrBuf[]);
