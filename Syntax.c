#include "Syntax.h"

LPCMLIST __cdecl CreateCementList(USHORT nStartLine,
                                  USHORT nStartCol,
                                  USHORT nEndLine,
                                  USHORT nEndCol,
                                  USHORT nLen) {
    LPCMLIST lpCmList = 
        (LPCMLIST)HeapAlloc(GetProcessHeap(),
                            HEAP_ZERO_MEMORY,
                            sizeof(CementList) + nLen * sizeof(CementListValue));
    lpCmList->nStartLine = nStartLine;
    lpCmList->nStartCol = nStartCol;
    lpCmList->nEndLine = nEndLine;
    lpCmList->nEndCol = nEndCol;
    lpCmList->nLen = nLen;
    return lpCmList;
}

void __cdecl DropCementList(LPCMLIST lpCmList) {
    USHORT i = 0;
    for (; i < lpCmList->nLen; i++) {
        CementListValue value = lpCmList->arrValues[i];
        if (value.uchValueKind == CL_SUB_LIST
            || value.uchValueKind == CL_QUOTE_LIST) {
            DropCementList(value.value.lpListValue);
        } else {
            DropToken(value.value.lpTokenValue);
        }
    }
    HeapFree(GetProcessHeap(), 0, lpCmList);
}

CementListValue __cdecl CementAtomValue(LPTOKEN lpToken) {
    CementListValue ret;
    ret.uchValueKind = CL_ATOM;
    ret.value.lpTokenValue = lpToken;
    return ret;
}

CementListValue __cdecl CementSubListValue(LPCMLIST lpCmList) {
    CementListValue ret;
    ret.uchValueKind = CL_SUB_LIST;
    ret.value.lpListValue = lpCmList;
    return ret;
}

CementListValue __cdecl CementQuoteListValue(LPCMLIST lpCmList) {
    CementListValue ret;
    ret.uchValueKind = CL_QUOTE_LIST;
    ret.value.lpListValue = lpCmList;
    return ret;
}

