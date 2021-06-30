#pragma once

#include <wtypes.h>

extern LPSTR __cdecl CreateStrFromSlice(CHAR const* pcStart, CHAR const* pcEnd);
extern BOOL __cdecl ReadFileToString(LPCSTR lpszFileName, LPSTR* lpDestBuf);
extern BOOL __cdecl WriteStringToFile(LPCSTR lpszFileName, LPCSTR lpszContent);
extern LPSTR __cdecl StringCopy(LPCSTR lpszSource);