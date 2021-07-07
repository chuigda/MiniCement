#ifndef WIN32COMPAT_WTYPES_H
#define WIN32COMPAT_WTYPES_H

#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>

typedef char *LPSTR;
typedef const char *LPCSTR;

typedef void VOID;
typedef VOID *LPVOID;
typedef const VOID *LPCVOID;
typedef char CHAR;
typedef unsigned char UCHAR;
typedef CHAR *PCHAR;
typedef int INT;
typedef unsigned int UINT;
typedef short SHORT;
typedef unsigned short USHORT;
typedef unsigned long ULONG;

typedef int32_t INT32;

typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint64_t QWORD;

typedef DWORD *LPDWORD;

typedef int BOOL;
#define TRUE 1
#define FALSE 0

typedef void *HANDLE;
typedef size_t SIZE_T;
#define INVALID_HANDLE_VALUE ((HANDLE)0xFFFFFFFF)

#ifndef __cdecl
#define __cdecl
#endif

#endif
