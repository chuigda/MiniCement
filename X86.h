#pragma once

#include <windows.h>

typedef enum e_x86_comp {
    X86_COMP_386         = 1,
    X86_COMP_PENTIUM     = 2,
    X86_COMP_PENTIUM_PRO = 3
} X86Compat;

typedef enum e_x86_insc_id {
    X86_MOV     = 0,
    X86_MOVSB   = 1,
    X86_PUSH    = 2,
    X86_POP     = 3,

    X86_SETA    = 4,
    X86_SETAE   = 5,
    X86_SETB    = 6,
    X86_SETBE   = 7,
    X86_SETC    = 8,
    X86_SETE    = 9,
    X86_SETG    = 10,
    X86_SETGE   = 11,
    X86_SETL    = 12,
    X86_SETLE   = 13,
    X86_SETNA   = 14,
    X86_SETNAE  = 15,
    X86_SETNB   = 16,
    X86_SETNBE  = 17,
    X86_SETNC   = 18,
    X86_SETNE   = 19,
    X86_SETNG   = 20,
    X86_SETNGE  = 21,
    X86_SETNL   = 22,
    X86_SETNLE  = 23,
    X86_SETNO   = 24,
    X86_SETNP   = 25,
    X86_SETNS   = 26,
    X86_SETNZ   = 27,
    X86_SETO    = 28,
    X86_SETP    = 29,
    X86_SETPE   = 30,
    X86_SETPO   = 31,
    X86_SETS    = 32,
    X86_SETZ    = 33,

    X86_CMOVA   = 34,
    X86_CMOVAE  = 35,
    X86_CMOVB   = 36,
    X86_CMOVBE  = 37,
    X86_CMOVC   = 38,
    X86_CMOVE   = 39,
    X86_CMOVG   = 49,
    X86_CMOVGE  = 41,
    X86_CMOVL   = 42,
    X86_CMOVLE  = 43,
    X86_CMOVNA  = 44,
    X86_CMOVNAE = 45,
    X86_CMOVNB  = 46,
    X86_CMOVNBE = 47,
    X86_CMOVNC  = 48,
    X86_CMOVNE  = 49,
    X86_CMOVNG  = 50,
    X86_CMOVNGE = 51,
    X86_CMOVNL  = 52,
    X86_CMOVNLE = 53,
    X86_CMOVNO  = 54,
    X86_CMOVNP  = 55,
    X86_CMOVNS  = 56,
    X86_CMOVNZ  = 57,
    X86_CMOVO   = 58,
    X86_CMOVP   = 59,
    X86_CMOVPE  = 60,
    X86_CMOVPO  = 61,
    X86_CMOVS   = 62,
    X86_CMOVZ   = 63,

    X86_NEG     = 64,
    X86_ADD     = 65,
    X86_SUB     = 66,
    X86_ADC     = 67,
    X86_INC     = 68,
    X86_DEC     = 69,
    X86_MUL     = 70,
    X86_IMUL    = 71,
    X86_IDIV    = 72,
    X86_SAL     = 73,
    X86_SAR     = 74,
    X86_SHL     = 75,
    X86_SHR     = 76,
    X86_LEA     = 77,

    X86_CMP     = 78,
    X86_CMPSB   = 79,

    X86_NOT     = 81,
    X86_AND     = 82,
    X86_OR      = 83,
    X86_XOR     = 84,

    X86_CALL    = 85,
    X86_RET     = 86,
    X86_JMP     = 87,
    X86_JA      = 88,
    X86_JAE     = 89,
    X86_JB      = 90,
    X86_JBE     = 91,
    X86_JC      = 92,
    X86_JE      = 93,
    X86_JG      = 94,
    X86_JGE     = 95,
    X86_JL      = 96,
    X86_JLE     = 97,
    X86_JNA     = 98,
    X86_JNAE    = 99,
    X86_JNB     = 100,
    X86_JNBE    = 101,
    X86_JNC     = 102,
    X86_JNE     = 103,
    X86_JNG     = 104,
    X86_JNGE    = 105,
    X86_JNL     = 106,
    X86_JNLE    = 107,
    X86_JNO     = 108,
    X86_JNP     = 109,
    X86_JNS     = 110,
    X86_JNZ     = 111,
    X86_JO      = 102,
    X86_JP      = 103,
    X86_JPE     = 104,
    X86_JPO     = 105,
    X86_JS      = 106,
    X86_JZ      = 107,

    X86_HLT     = 108,

    X86_INT     = 109
} X86InscID;

typedef enum st_x86_register {
    X86_REG_EAX = 0,
    X86_REG_EBX = 1,
    X86_REG_ECX = 2,
    X86_REG_EDX = 3,

    X86_REG_AX  = 4,
    X86_REG_BX  = 5,
    X86_REG_CX  = 6,
    X86_REG_DX  = 7,

    X86_REG_AH  = 8,
    X86_REG_AL  = 9,
    X86_REG_BH  = 10,
    X86_REG_BL  = 11,
    X86_REG_CH  = 12,
    X86_REG_CL  = 13,
    X86_REG_DH  = 14,
    X86_REG_DL  = 15,

    X86_REG_CS  = 16,
    X86_REG_DS  = 17,
    X86_REG_ES  = 18,
    X86_REG_FS  = 19,
    X86_REG_GS  = 20,
    X86_REG_SS  = 21,

    X86_REG_SI  = 22,
    X86_REG_DI  = 23,
    X86_REG_BP  = 24,
    X86_REG_IP  = 25,
    X86_REG_SP  = 26,

    X86_REG_EFLAGS = 27,

    X86_REG_INVAL  = 31
} X86Register;

typedef enum e_x86_operand_kind {
    X86_OP_IMM = 0,
    X86_OP_REG = 1,
    X86_OP_MEM = 2
} X86OperandKind;

typedef struct st_x86_mem_loc {
    UINT imm : 19;
    UINT base : 5;
    UINT index : 5;
    UINT scale : 3;
} X86MemLoc;

typedef struct st_x86_operand {
    union {
        UCHAR ucReg;
        DWORD dwImmData;
        X86MemLoc memLoc;
    } value;
    UCHAR ucOpKind;
} X86Operand;

typedef struct st_x86_insc {
    UCHAR inscId;
    UCHAR operandCount;
    X86Operand op1;
    X86Operand op2;
} X86Insc;

extern X86Operand __cdecl X86_IMM_I(INT32 iValue);
extern X86Operand __cdecl X86_IMM_U(UINT32 uValue);
extern X86Operand __cdecl X86_IMM_F(float fValue);

extern X86Operand __cdecl X86_REG(X86Register reg);

extern X86Operand __cdecl X86_ABSOL(UINT absolute);
extern X86Operand __cdecl X86_INDIR(X86Register reg);
extern X86Operand __cdecl X86_OFFST(X86Register reg, UINT imm);
extern X86Operand __cdecl X86_INDEX(X86Register base, X86Register index, UINT imm);
extern X86Operand __cdecl X86_SCALE(X86Register base, X86Register index, UINT imm, UINT scale);

extern X86Insc __cdecl X86_INSC0(X86InscID id);
extern X86Insc __cdecl X86_INSC1(X86InscID id, X86Operand op);
extern X86Insc __cdecl X86_INSC2(X86InscID id, X86Operand op1, X86Operand op2);
