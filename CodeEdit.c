#include "CodeEdit.h"
#include <windows.h>

typedef struct st_code_edit_data {
    WNDPROC lpfnDefaultEditProc;
    LPSTR lpszBuffer;
    UINT uBufSize;
} CodeEditData;

typedef CodeEditData *LPCEDATA;

static USHORT GetLineIndent(LPCSTR lpszSource, UINT uIndex);
static LRESULT CALLBACK CodeEditWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND __cdecl CreateCodeEditControl(HWND hWndParent,
                                   INT x,
                                   INT y,
                                   INT nWidth,
                                   INT nHeight) {
    HWND hWnd = CreateWindowEx(0, "EDIT", NULL,
                               WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
                               x, y, nWidth, nHeight,
                               hWndParent,
                               NULL,
                               (HINSTANCE)GetWindowLongPtr(hWndParent, GWLP_HINSTANCE),
                               NULL);
    if (!hWnd) {
        return NULL;
    }

    LPCEDATA lpCodeEditData = (LPCEDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CodeEditData));
    if (!lpCodeEditData) {
        return NULL;
    }

    lpCodeEditData->lpfnDefaultEditProc = GetWindowLong(hWnd, GWL_WNDPROC);
    SetWindowLong(hWnd, GWL_USERDATA, (LONG)lpCodeEditData);
    SetWindowLong(hWnd, GWL_WNDPROC, CodeEditWindowProc);
    return hWnd;
}

static LRESULT CALLBACK CodeEditWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    LPCEDATA lpCodeEditData = (LPCEDATA)GetWindowLong(hWnd, GWL_USERDATA);
    switch (msg) {
    case WM_CHAR:
        switch (wParam) {
        case '\r': {
            DWORD dwCursor;
            INT uTextLength;
            LPSTR lpszBuffer;
            USHORT nLineIndent;

            uTextLength = GetWindowTextLength(hWnd);
            if (uTextLength + 1 > lpCodeEditData->uBufSize) {
                lpszBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, uTextLength + 4096);
                if (!lpszBuffer) {
                    return lpCodeEditData->lpfnDefaultEditProc(hWnd, msg, wParam, lParam);
                }
                HeapFree(GetProcessHeap(), 0, lpCodeEditData->lpszBuffer);
                lpCodeEditData->lpszBuffer = lpszBuffer;
                lpCodeEditData->uBufSize = uTextLength + 4096;
            }

            GetWindowText(hWnd, lpCodeEditData->lpszBuffer, uTextLength + 1);
            SendMessage(hWnd, EM_GETSEL, (WPARAM)&dwCursor, (LPARAM)NULL);

            nLineIndent = GetLineIndent(lpCodeEditData->lpszBuffer, dwCursor);

            USHORT i;
            lpCodeEditData->lpfnDefaultEditProc(hWnd, msg, wParam, lParam);
            for (i = 0; i < nLineIndent; i++) {
                SendMessage(hWnd, WM_CHAR, ' ', 0);
            }
            return TRUE;
        }
        case '\t': {
            INT i;
            for (i = 0; i < 2; i++) {
                SendMessage(hWnd, WM_CHAR, ' ', 0);
            }
            return TRUE;
        }
        }
        /* fallthrough */
    default:
        if (lpCodeEditData) {
            lpCodeEditData->lpfnDefaultEditProc(hWnd, msg, wParam, lParam);
        }
    }
}

static USHORT GetLineIndent(LPCSTR lpszSource, UINT uIndex) {
    USHORT nCount = 0;

    while (uIndex != 0 && lpszSource[uIndex] != '\n') {
        --uIndex;
    }
    if (lpszSource[uIndex] == '\n') {
        ++uIndex;
    }
    while (isspace((int)lpszSource[uIndex])) {
        ++nCount;
        ++uIndex;
    }
    return nCount;
}
