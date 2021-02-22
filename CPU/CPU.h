/*------------------------------------------------------------------------------
    * File:        CPU.h                                                       *
    * Description: ***********************************                         *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
*///----------------------------------------------------------------------------

#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS


#include <assert.h>

#include "../Template.h"
#include "../Errors.h"
#include "../Commands.h"
#include "../StringLib/StringLib.h"
#include "../StackLib/Stack.h"


const size_t DEFAULT_STACK_CAPACITY = 8;


typedef struct CPU
{
    bcode_t bcode = {};
    TEMPLATE(stack, TYPE) stkCPU;
    TYPE registers[REG_NUM] = {};
} cpu_t;


//------------------------------------------------------------------------------

int CPUConstruct (cpu_t* p_cpu, const char* filename);

//------------------------------------------------------------------------------

int Execute (cpu_t* p_cpu);

//------------------------------------------------------------------------------

int CPUDestruct (cpu_t* p_cpu);

//------------------------------------------------------------------------------

#endif //CPU_H_INCLUDED