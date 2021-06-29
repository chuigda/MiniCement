#pragma once

#include <wtypes.h>

typedef union u_raw_value {
    INT32 iValue;
    float fValue;
    DWORD32 dwRef;
} RawValue;

typedef enum e_value_kind {
    VK_INT,
    VK_FLOAT,
    VK_REF
} ValueKind;

typedef struct st_value {
    RawValue raw;
    UCHAR ucValueKind;
} Value;
