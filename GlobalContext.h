#pragma once

#include <wtypes.h>

typedef struct st_global_context {
    HANDLE hHeap;
    HINSTANCE hInstance;
} GlobalContext;

typedef GlobalContext* LPGCONTEXT;
typedef GlobalContext const* LPCGCONTEXT;
