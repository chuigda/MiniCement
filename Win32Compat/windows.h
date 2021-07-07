#ifndef WIN32COMPAT_WINDOWS_H
#define WIN32COMPAT_WINDOWS_H

#include <wtypes.h>

enum {
    HEAP_ZERO_MEMORY = 1
};

extern LPVOID HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwSize);
extern BOOL HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpvMem);
extern HANDLE GetProcessHeap(VOID);

enum {
    GENERIC_READ  = 0x01,
    GENERIC_WRITE = 0x02
};

enum {
    FILE_SHARE_READ = 0x01,
    FILE_SHARE_WRITE = 0x02,
    FILE_SHARE_DELETE = 0x04
};

enum {
    CREATE_NEW = 1,
    CREATE_ALWAYS = 2,
    OPEN_EXISTING = 3,
    OPEN_ALWAYS = 4,
    TRUNCATE_EXISTING = 5
};

extern HANDLE CreateFile(LPCSTR lpszFileName,
                          DWORD dwDesiredAccess,
                          DWORD dwShareMode,
                          LPVOID lpvSecurityAttrbutes,
                          DWORD dwCreationDisposition,
                          DWORD dwFlagsAndAttributes,
                          HANDLE hTemplateFile);
extern BOOL ReadFile(HANDLE hFile,
                      LPVOID lpvBuffer,
                      DWORD dwBytesToRead,
                      LPDWORD lpBytesRead,
                      LPVOID lpvOverlapped);

extern BOOL WriteFile(HANDLE hFile,
                       LPCVOID lpvBuffer,
                       DWORD dwBytesToWrite,
                       LPDWORD lpBytesWritten,
                       LPVOID lpvOverlapped);

extern DWORD GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);

extern BOOL CloseHandle(HANDLE hObject);

extern VOID CopyMemory(LPVOID lpvDest, LPCVOID lpvSrc, SIZE_T length);

#endif
