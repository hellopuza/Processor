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

#define TYPE int

const size_t POINTER_SIZE = sizeof(size_t);
const size_t NUMBER_SIZE  = sizeof(TYPE);

const double NIL  = 1e-7;

const int NUM_FLAG = 0x80;
const int REG_FLAG = 0x40;

const int PROCESS_END = -666;

/*------------------------------------------------------------------------------
                   Commands codes                                              *
*///----------------------------------------------------------------------------

enum Commands
{
    CMD_END   = 0x00,
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

};

struct command
{
    char code;
    const char* word;
};

static command cmd_names[] =
{
    { CMD_END  ,  "end"  },//0
    { CMD_PUSH ,  "push" },//1
    { CMD_POP  ,  "pop"  },//2
    { CMD_ADD  ,  "add"  },//3
    { CMD_SUB  ,  "sub"  },//4
    { CMD_MUL  ,  "mul"  },//5
    { CMD_DIV  ,  "div"  },//6
    { CMD_NEG  ,  "neg"  },//7
    { CMD_SIN  ,  "sin"  },//8
    { CMD_COS  ,  "cos"  },//9
    { CMD_SQRT ,  "sqrt" },//10
    { CMD_IN   ,  "in"   },//11
    { CMD_OUT  ,  "out"  },//12
    { CMD_JMP  ,  "jmp"  },//13
    { CMD_JE   ,  "je"   },//14
    { CMD_JNE  ,  "jne"  },//15
    { CMD_JA   ,  "ja"   },//16
    { CMD_JAE  ,  "jae"  },//17
    { CMD_JB   ,  "jb"   },//18
    { CMD_JBE  ,  "jbe"  },//19
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
    return ( (code == CMD_JMP) ||
             (code == CMD_JE ) ||
             (code == CMD_JNE) ||
             (code == CMD_JA ) ||
             (code == CMD_JAE) ||
             (code == CMD_JB ) ||
             (code == CMD_JBE)   );
}

//------------------------------------------------------------------------------

#endif // COMMAND_H_INCLUDED