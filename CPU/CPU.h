/*------------------------------------------------------------------------------
    * File:        CPU.h                                                       *
    * Description: Declaration of functions and data types used for executing  *
                   binary programs                                             *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
//#define NDEBUG


#include <assert.h>

#include "../Template.h"
#include "../Errors.h"
#include "../Commands.h"
#include "../StringLib/StringLib.h"

#define TYPE NUM_TYPE
#include "../StackLib/Stack.h"
#undef TYPE

#define TYPE PTR_TYPE
#include "../StackLib/Stack.h"
#undef TYPE


static const char* cpu_logname = "cpu.log";

#define CPU_ASSERTOK(cond, err, printcode, p_cpu) if (cond)                                                           \
                                                  {                                                                   \
                                                      printError(cpu_logname, __FILE__, __LINE__, __FUNCTION__, err); \
                                                      if (printcode) printCode(p_cpu, cpu_logname, err);              \
                                                      exit(err); /**/                                                 \
                                                  }


const size_t DEFAULT_STACK_CAPACITY = 8;
const double NIL  = 1e-7;

typedef struct CPU
{
    bcode_t bcode = {};
    TEMPLATE(stack, NUM_TYPE) stkCPU_NUM;
    TEMPLATE(stack, PTR_TYPE) stkCPU_PTR;
    NUM_TYPE registers[REG_NUM] = {};
} cpu_t;


//------------------------------------------------------------------------------

int CPUConstruct (cpu_t* p_cpu, const char* filename);

//------------------------------------------------------------------------------

int Execute (cpu_t* p_cpu);

//------------------------------------------------------------------------------

int CPUDestruct (cpu_t* p_cpu);

//------------------------------------------------------------------------------

void printCode(cpu_t* p_cpu, const char* logname, int err);

//------------------------------------------------------------------------------

void Pop1Number (cpu_t* p_cpu, NUM_TYPE* num);

//------------------------------------------------------------------------------

void Pop2Numbers (cpu_t* p_cpu, NUM_TYPE* num1, NUM_TYPE* num2);

//------------------------------------------------------------------------------

#endif //CPU_H_INCLUDED