#pragma once

#include <wtypes.h>

#include "Lex.h"

enum e_cement_list_value_kind {
    CL_ATOM       = 1,
    CL_SUB_LIST   = 2,
    CL_QUOTE_LIST = 3
};

struct st_cement_list;

typedef struct st_cement_list* LPCMLIST;

typedef struct st_cement_list_value {
    union {
        LPTOKEN lpTokenValue;
        LPCMLIST lpListValue;
    } value;
    UCHAR uchValueKind;
} CementListValue;

typedef struct st_cement_list {
    USHORT nStartLine;
    USHORT nStartCol;
    USHORT nEndLine;
    USHORT nEndCol;
    USHORT nLen;
    CementListValue arrValues[0];
} CementList;

struct st_cement_top_level;

typedef struct st_cement_top_level* LPCMTOPLEVEL;

typedef struct st_cement_top_level {
    LPCMLIST lpCmListData;
    LPCMTOPLEVEL lpNext;
} CementTopLevel;

extern LPCMLIST __cdecl CreateCementList(USHORT nStartLine,
                                         USHORT nStartCol,
                                         USHORT nEndLine,
                                         USHORT nEndCol,
                                         USHORT nLen);
extern void __cdecl DropCementList(LPCMLIST lpCmList);
extern CementListValue __cdecl CementAtomValue(LPTOKEN lpToken);
extern CementListValue __cdecl CementSubListValue(LPCMLIST lpCmList);
extern CementListValue __cdecl CementQuoteListValue(LPCMLIST lpCmList);
