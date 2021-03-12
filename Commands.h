/*------------------------------------------------------------------------------
    * File:        Commands.h                                                  *
    * Description: Names of commands, registers and their codes                *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#ifndef COMMANDS_H_INCLUDED
#define COMMANDS_H_INCLUDED


#ifndef PTR_T
#define PTR_T
typedef size_t ptr_t;

#include <limits.h>
#define PTR_MAX UINT_MAX
#endif // PTR_T


#define NUM_INT_TYPE int
#define NUM_FLT_TYPE double

#define PTR_TYPE ptr_t

const size_t POINTER_SIZE     = sizeof(PTR_TYPE);
const size_t NUMBER_INT_SIZE  = sizeof(NUM_INT_TYPE);
const size_t NUMBER_FLT_SIZE  = sizeof(NUM_FLT_TYPE);

const int NUM_FLAG = 0x80;
const int REG_FLAG = 0x40;
const int PTR_FLAG = 0x20;

const int PROCESS_HALT = -666;

/*------------------------------------------------------------------------------
                   Commands codes                                              *
*///----------------------------------------------------------------------------

enum Commands
{
    CMD_HLT      = 0x00,
    CMD_PUSH     = 0x01,
    CMD_POP      = 0x02,
    CMD_IN       = 0x03,
    CMD_OUT      = 0x04,
    CMD_ADD      = 0x05,
    CMD_SUB      = 0x06,
    CMD_MUL      = 0x07,
    CMD_DIV      = 0x08,
    CMD_NEG      = 0x09,
    CMD_AND      = 0x0A,
    CMD_OR       = 0x0B,
    CMD_XOR      = 0x0C,
    CMD_PUSHQ    = 0x0D,
    CMD_POPQ     = 0x0E,
    CMD_INQ      = 0x0F,
    CMD_OUTQ     = 0x10,
    CMD_ADDQ     = 0x11,
    CMD_SUBQ     = 0x12,
    CMD_MULQ     = 0x13,
    CMD_DIVQ     = 0x14,
    CMD_NEGQ     = 0x15,
    CMD_SIN      = 0x16,
    CMD_COS      = 0x17,
    CMD_SQRT     = 0x18,
    CMD_JMP      = 0x19,
    CMD_JE       = 0x1A,
    CMD_JNE      = 0x1B,
    CMD_JA       = 0x1C,
    CMD_JAE      = 0x1D,
    CMD_JB       = 0x1E,
    CMD_JBE      = 0x1F,
    CMD_CALL     = 0x20,
    CMD_RET      = 0x21,
    CMD_FLT2INT  = 0x22,
    CMD_INT2FLT  = 0x23,
    CMD_SCREEN   = 0x24,
};

struct command
{
    char code;
    const char* word;
};

static command cmd_names[] =
{
    { CMD_ADD      ,  "add"     },
    { CMD_ADDQ     ,  "addq"    },
    { CMD_AND      ,  "and"     },
    { CMD_CALL     ,  "call"    },
    { CMD_COS      ,  "cos"     },
    { CMD_DIV      ,  "div"     },
    { CMD_DIVQ     ,  "divq"    },
    { CMD_FLT2INT  ,  "flt2int" },
    { CMD_HLT      ,  "hlt"     },
    { CMD_IN       ,  "in"      },
    { CMD_INQ      ,  "inq"     },
    { CMD_INT2FLT  ,  "int2flt" },
    { CMD_JA       ,  "ja"      },
    { CMD_JAE      ,  "jae"     },
    { CMD_JB       ,  "jb"      },
    { CMD_JBE      ,  "jbe"     },
    { CMD_JE       ,  "je"      },
    { CMD_JMP      ,  "jmp"     },
    { CMD_JNE      ,  "jne"     },
    { CMD_MUL      ,  "mul"     },
    { CMD_MULQ     ,  "mulq"    },
    { CMD_NEG      ,  "neg"     },
    { CMD_NEGQ     ,  "negq"    },
    { CMD_OR       ,  "or"      },
    { CMD_OUT      ,  "out"     },
    { CMD_OUTQ     ,  "outq"    },
    { CMD_POP      ,  "pop"     },
    { CMD_POPQ     ,  "popq"    },
    { CMD_PUSH     ,  "push"    },
    { CMD_PUSHQ    ,  "pushq"   },
    { CMD_RET      ,  "ret"     },
    { CMD_SCREEN   ,  "screen"  },
    { CMD_SIN      ,  "sin"     },
    { CMD_SQRT     ,  "sqrt"    },
    { CMD_SUB      ,  "sub"     },
    { CMD_SUBQ     ,  "subq"    },
    { CMD_XOR      ,  "xor"     },
};

const int CMD_NUM = sizeof(cmd_names)/sizeof(cmd_names[0]);

/*------------------------------------------------------------------------------
                   Register codes                                              *
*///----------------------------------------------------------------------------

enum Registers
{
    REG_EAX  = 0x01,
    REG_EBX  = 0x02,
    REG_ECX  = 0x03,
    REG_EDX  = 0x04,

    REG_RAX  = 0x05,
    REG_RBX  = 0x06,
    REG_RCX  = 0x07,
    REG_RDX  = 0x08,

    REG_RBP  = 0x09,
    REG_RSP  = 0x0A,

    REG_SCRX = 0x0B,
    REG_SCRY = 0x0C,
};

struct reg
{
    char code;
    const char* word;
};

static command reg_names[] =
{
    { REG_EAX   ,  "eax"  },//1
    { REG_EBX   ,  "ebx"  },//2
    { REG_ECX   ,  "ecx"  },//3
    { REG_EDX   ,  "edx"  },//4

    { REG_RAX   ,  "rax"  },//5
    { REG_RBX   ,  "rbx"  },//6
    { REG_RCX   ,  "rcx"  },//7
    { REG_RDX   ,  "rdx"  },//8

    { REG_RBP   ,  "rbp"  },//9
    { REG_RSP   ,  "rsp"  },//10

    { REG_SCRX  ,  "scrx" },//11
    { REG_SCRY  ,  "scry" },//12
};

const int REG_NUM = sizeof(reg_names) / sizeof(reg_names[0]);

//------------------------------------------------------------------------------

static int isJUMP(char code)
{
    return ( (code == CMD_JMP ) ||
             (code == CMD_JE  ) ||
             (code == CMD_JNE ) ||
             (code == CMD_JA  ) ||
             (code == CMD_JAE ) ||
             (code == CMD_JB  ) ||
             (code == CMD_JBE ) ||
             (code == CMD_CALL)   );
}

//------------------------------------------------------------------------------

#endif // COMMAND_H_INCLUDED