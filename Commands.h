/*------------------------------------------------------------------------------
    * File:        Commands.h                                                  *
    * Description: ***********************************                         *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
*///----------------------------------------------------------------------------

#ifndef COMMANDS_H_INCLUDED
#define COMMANDS_H_INCLUDED


#ifndef PTR_T
#define PTR_T
typedef size_t ptr_t;

#include <limits.h>
#define PTR_MAX UINT_MAX
#endif // PTR_T


#define NUM_TYPE int
#define PTR_TYPE ptr_t

const size_t POINTER_SIZE = sizeof(PTR_TYPE);
const size_t NUMBER_SIZE  = sizeof(NUM_TYPE);

const int NUM_FLAG = 0x80;
const int REG_FLAG = 0x40;

const int PROCESS_HALT = -666;

/*------------------------------------------------------------------------------
                   Commands codes                                              *
*///----------------------------------------------------------------------------

enum Commands
{
    CMD_HLT   = 0x00,
    CMD_PUSH  = 0x01,
    CMD_POP   = 0x02,
    CMD_ADD   = 0x03,
    CMD_SUB   = 0x04,
    CMD_MUL   = 0x05,
    CMD_DIV   = 0x06,
    CMD_NEG   = 0x07,
    CMD_SIN   = 0x08,
    CMD_COS   = 0x09,
    CMD_SQRT  = 0x0A,
    CMD_IN    = 0x0B,
    CMD_OUT   = 0x0C,
    CMD_JMP   = 0x0D,
    CMD_JE    = 0x0E,
    CMD_JNE   = 0x0F,
    CMD_JA    = 0x10,
    CMD_JAE   = 0x11,
    CMD_JB    = 0x12,
    CMD_JBE   = 0x13,
    CMD_CALL  = 0x14,
    CMD_RET   = 0x15,
};

struct command
{
    char code;
    const char* word;
};

static command cmd_names[] =
{
    { CMD_ADD  ,  "add"  },
    { CMD_CALL ,  "call" },
    { CMD_COS  ,  "cos"  },
    { CMD_DIV  ,  "div"  },
    { CMD_HLT  ,  "hlt"  },
    { CMD_IN   ,  "in"   },
    { CMD_JA   ,  "ja"   },
    { CMD_JAE  ,  "jae"  },
    { CMD_JB   ,  "jb"   },
    { CMD_JBE  ,  "jbe"  },
    { CMD_JE   ,  "je"   },
    { CMD_JMP  ,  "jmp"  },
    { CMD_JNE  ,  "jne"  },
    { CMD_MUL  ,  "mul"  },
    { CMD_NEG  ,  "neg"  },
    { CMD_OUT  ,  "out"  },
    { CMD_POP  ,  "pop"  },
    { CMD_PUSH ,  "push" },
    { CMD_RET  ,  "ret"  },
    { CMD_SIN  ,  "sin"  },
    { CMD_SQRT ,  "sqrt" },
    { CMD_SUB  ,  "sub"  },
};

const int CMD_NUM = sizeof(cmd_names)/sizeof(cmd_names[0]);

/*------------------------------------------------------------------------------
                   Register codes                                              *
*///----------------------------------------------------------------------------

enum Registers
{
    REG_EAX = 0x01,
    REG_EBX = 0x02,
    REG_ECX = 0x03,
    REG_EDX = 0x04,

    REG_RAX = 0x05,
    REG_RBX = 0x06,
    REG_RCX = 0x07,
    REG_RDX = 0x08,
};

struct reg
{
    char code;
    const char* word;
};

static command reg_names[] =
{
    { REG_EAX  ,  "eax" },//1
    { REG_EBX  ,  "ebx" },//2
    { REG_ECX  ,  "ecx" },//3
    { REG_EDX  ,  "edx" },//4

    { REG_RAX  ,  "rax" },//5
    { REG_RBX  ,  "rbx" },//6
    { REG_RCX  ,  "rcx" },//7
    { REG_RDX  ,  "rdx" },//8
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