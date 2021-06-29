#pragma once

#include <wtypes.h>

typedef struct st_source_info {
    LPCSTR lpszSrcFile;
    USHORT nLine;
    USHORT nCol;
} SourceInfo;

extern SourceInfo __cdecl CreateSourceInfo(LPCSTR lpszSrcFile, USHORT nLine, USHORT nCol);

typedef struct st_error {
    SourceInfo srcInfo;
    USHORT nErrCode;
    USHORT nBufferSize;
    CHAR szBuffer[0];
} Error;

typedef Error* LPERROR;
typedef Error const* LPCERROR;

extern LPERROR __cdecl CreateErrorBuffer(USHORT nBufferSize);
extern void __cdecl DropError(LPERROR lpErr);
extern BOOL __cdecl IsError(LPCERROR lpErr);
extern void __cdecl FormatError(LPERROR lpErrDest,
                                SourceInfo srcInfo,
                                USHORT nErrCode,
                                LPCSTR fmt,
                                ...);
