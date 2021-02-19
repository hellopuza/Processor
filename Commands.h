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

#define _CRT_SECURE_NO_WARNINGS

#include "Template.h"
#include "Errors.h"
#include <math.h>

#define TYPE double
#include "StackLib/Stack.h"

const size_t NUMBER_SIZE = sizeof(TYPE);

const double NIL  = 1e-7;

const int REG_BIT = 0x40;
const int NUM_BIT = 0x80;

const int PROCESS_END = -666;

/*------------------------------------------------------------------------------
                   Commands codes                                              *
*///----------------------------------------------------------------------------

enum Commands
{
    CMD_END   = 0x00             ,
    CMD_PUSH  = 0x01  | NUM_BIT  ,
    CMD_POP   = 0x02  | REG_BIT  ,
    CMD_ADD   = 0x03             ,
    CMD_SUB   = 0x04             ,
    CMD_MUL   = 0x05             ,
    CMD_DIV   = 0x06             ,
    CMD_NEG   = 0x07             ,
    CMD_SIN   = 0x08             ,
    CMD_COS   = 0x09             ,
    CMD_SQRT  = 0x0a             ,
    CMD_IN    = 0x0b             ,
    CMD_OUT   = 0x0c             ,
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

/*------------------------------------------------------------------------------
                   Execution of commands                                       *
*///----------------------------------------------------------------------------

static int EXECUTE_CMD(TEMPLATE(stack, TYPE)* p_stk, char cmd_code)
{
    TYPE num  = TEMPLATE(TYPE, POISON);
    TYPE num1 = TEMPLATE(TYPE, POISON);
    TYPE num2 = TEMPLATE(TYPE, POISON);

    switch (cmd_code)
    {
        case CMD_END:
            return PROCESS_END;
            break;

        case CMD_ADD:
            num1 = TEMPLATE(StackPop, TYPE) (p_stk);
            num2 = TEMPLATE(StackPop, TYPE) (p_stk);
            TEMPLATE(StackPush, TYPE) (p_stk, num1 + num2);
            break;

        case CMD_SUB:
            num1 = TEMPLATE(StackPop, TYPE) (p_stk);
            num2 = TEMPLATE(StackPop, TYPE) (p_stk);
            TEMPLATE(StackPush, TYPE) (p_stk, num2 - num1);
            break;

        case CMD_MUL:
            num1 = TEMPLATE(StackPop, TYPE) (p_stk);
            num2 = TEMPLATE(StackPop, TYPE) (p_stk);
            TEMPLATE(StackPush, TYPE) (p_stk, num2 * num1);
            break;

        case CMD_DIV:
            num1 = TEMPLATE(StackPop, TYPE) (p_stk);
            num2 = TEMPLATE(StackPop, TYPE) (p_stk);
            if (fabs(num1) < NIL)
                return DIVISION_BY_ZERO;
            TEMPLATE(StackPush, TYPE) (p_stk, num2 / num1);
            break;

        case CMD_NEG:
            num = TEMPLATE(StackPop, TYPE) (p_stk);
            TEMPLATE(StackPush, TYPE) (p_stk, -num);
            break;

        case CMD_SIN:
            num = TEMPLATE(StackPop, TYPE) (p_stk);
            TEMPLATE(StackPush, TYPE) (p_stk, sin(num));
            break;

        case CMD_COS:
            num = TEMPLATE(StackPop, TYPE) (p_stk);
            TEMPLATE(StackPush, TYPE) (p_stk, cos(num));
            break;

        case CMD_SQRT:
            num = TEMPLATE(StackPop, TYPE) (p_stk);
            if (num < 0)
                return ROOT_OF_A_NEG_NUMBER;
            TEMPLATE(StackPush, TYPE) (p_stk, sqrt(num));
            break;

        case CMD_OUT:
            num = TEMPLATE(StackPop, TYPE) (p_stk);
            TEMPLATE(StackPush, TYPE) (p_stk, num);
            printf(TEMPLATE(TYPE, PRINT_FORMAT) "\n", num);
            break;

        case CMD_IN:
            if (scanf(TEMPLATE(TYPE, PRINT_FORMAT), &num) != 1)
                return INCORRECT_INPUT;
            TEMPLATE(StackPush, TYPE) (p_stk, num);
            break;
        
            /*
        case CMD_DUMP:
            CPUDump(cpu);
            break;
            */

        default:
            return NOT_OK;
    }

    return OK;
}


#endif // COMMAND_H_INCLUDED