#define _CRT_SECURE_NO_WARNINGS

#include "Error.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

SourceInfo __cdecl CreateSourceInfo(LPCSTR lpszSrcFile, USHORT nLine, USHORT nCol) {
    SourceInfo ret;
    ret.lpszSrcFile = lpszSrcFile;
    ret.nLine = nLine;
    ret.nCol = nCol;
    return ret;
}

LPERROR __cdecl CreateErrorBuffer(USHORT nBufferSize) {
    LPERROR lpErrRet = (LPERROR)HeapAlloc(GetProcessHeap(),
                                          HEAP_ZERO_MEMORY,
                                          sizeof(Error) + nBufferSize);
    if (!lpErrRet) {
        return NULL;
    }
    lpErrRet->nBufferSize = nBufferSize;
    return lpErrRet;
}

void __cdecl DropError(LPERROR lpErr) {
    HeapFree(GetProcessHeap(), 0, (LPVOID)lpErr);
}

BOOL __cdecl IsError(LPCERROR lpErr) {
    return !lpErr->nErrCode;
}

void __cdecl FormatError(LPERROR lpErrDest,
                         SourceInfo srcInfo,
                         USHORT nErrCode,
                         LPCSTR fmt,
                         ...) {
    va_list va;

    lpErrDest->srcInfo = srcInfo;
    lpErrDest->nErrCode = nErrCode;

    va_start(va, fmt);
    vsprintf((LPSTR const)lpErrDest->szBuffer, fmt, va);
    va_end(va);
}
