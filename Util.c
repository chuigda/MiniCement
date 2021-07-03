#define _CRT_SECURE_NO_WARNINGS

#include <string.h>
#include <windows.h>
#include "Util.h"

LPSTR __cdecl CreateStrFromSlice(CHAR const* pcStart, CHAR const* pcEnd) {
    SIZE_T size = pcEnd - pcStart;
    LPSTR buf = (LPSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size + 1);
    strncpy(buf, pcStart, size);
    return buf;
}

BOOL __cdecl ReadFileToString(LPCSTR lpszFileName, LPSTR* lpDestBuf) {
    HANDLE hFile;
    DWORD dwFileSize;
    DWORD dwDummy;
    LPSTR lpszBuffer;
    BOOL bSuccess;

    hFile = CreateFile(lpszFileName,
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       NULL,
                       OPEN_EXISTING,
                       0,
                       NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    dwFileSize = GetFileSize(hFile, NULL);
    if (dwFileSize == 0xFFFFFFFF) {
        CloseHandle(hFile);
        return FALSE;
    }

    lpszBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFileSize + 1);
    if (lpszBuffer == NULL) {
        CloseHandle(hFile);
        return FALSE;
    }

    bSuccess = ReadFile(hFile, lpszBuffer, dwFileSize, &dwDummy, NULL);
    if (!bSuccess) {
        HeapFree(GetProcessHeap(), 0, lpszBuffer);
        CloseHandle(hFile);
        return FALSE;
    }

    *lpDestBuf = lpszBuffer;
    CloseHandle(hFile);
    return TRUE;
}

BOOL __cdecl WriteStringToFile(LPCSTR lpszFileName, LPCSTR lpszContent) {
    HANDLE hFile;
    DWORD dwFileSize;
    DWORD dwDummy;
    BOOL bSuccess;

    hFile = CreateFile(lpszFileName,
                       GENERIC_WRITE,
                       0,
                       NULL,
                       OPEN_ALWAYS,
                       0,
                       NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    dwFileSize = strlen(lpszContent);
    bSuccess = WriteFile(hFile, lpszContent, dwFileSize, &dwDummy, NULL);
    CloseHandle(hFile);
    return bSuccess;
}

LPSTR __cdecl StringCopy(LPCSTR lpszSource) {
    ULONG ulLen = strlen(lpszSource);
    LPVOID lpvDestBuf = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ulLen + 1);
    CopyMemory(lpvDestBuf, lpszSource, ulLen);
    return (LPSTR)lpvDestBuf;
}
