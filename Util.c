#include <string.h>
#include <windows.h>
#include "Util.h"

LPSTR __cdecl CreateStrFromSlice(CHAR const* pcStart, CHAR const* pcEnd) {
    SIZE_T size = pcEnd - pcStart;
    LPSTR buf = (LPSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size + 1);
    strncpy(buf, pcStart, size);
    return buf;
}