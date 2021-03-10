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

#include "../TXLib.h"

#include <assert.h>

#include "../Template.h"
#include "../Errors.h"
#include "../Commands.h"
#include "../StringLib/StringLib.h"

#define TYPE NUM_INT_TYPE
#include "../StackLib/Stack.h"
#undef TYPE

#define TYPE NUM_FLT_TYPE
#include "../StackLib/Stack.h"
#undef TYPE

#define TYPE PTR_TYPE
#include "../StackLib/Stack.h"
#undef TYPE


static const char* cpu_logname = "cpu.log";

#define CPU_ASSERTOK(cond, err, printcode, p_cpu) if (cond)                                                         \
                                                  {                                                                 \
                                                    printError(cpu_logname, __FILE__, __LINE__, __FUNCTION__, err); \
                                                    if (printcode) printCode(p_cpu, cpu_logname, err);              \
                                                    exit(err); /**/                                                 \
                                                  }


const size_t DEFAULT_STACK_CAPACITY = 8;
const double NIL  = 1e-7;
const size_t RAM_SIZE   = 2097152; // 2 MB
const size_t PIXEL_SIZE = 3;

typedef struct CPU
{
    int state = CPU_NOT_CONSTRUCTED;

    bcode_t bcode = {};

    char* RAM;

    TEMPLATE(stack, NUM_INT_TYPE) stkCPU_NUM_INT;
    TEMPLATE(stack, NUM_FLT_TYPE) stkCPU_NUM_FLT;

    TEMPLATE(stack, PTR_TYPE) stkCPU_PTR;

    NUM_FLT_TYPE registers[REG_NUM] = {};
} cpu_t;


//------------------------------------------------------------------------------
/*! @brief   CPU constructor.
 *
 *  @param   p_cpu       Pointer to the CPU
 *  @param   filename    Name of a binary code file
 *
 *  @return  error code
 */

int CPUConstruct (cpu_t* p_cpu, const char* filename);

//------------------------------------------------------------------------------
/*! @brief   Execution process.
 *
 *  @note    The program text is a binary code. The processor goes through the code,
 *           executes commands, goes to the addresses in the code. If an error is found,
 *           the process stops and the code section with the error is output.
 *
 *  @param   p_cpu       Pointer to the CPU
 *  @param   filename    Name of a binary code file
 *
 *  @return  error code
 */

int Execute (cpu_t* p_cpu, char* filename);

//------------------------------------------------------------------------------
/*! @brief   CPU destructor.
 *
 *  @param   p_cpu       Pointer to the CPU
 *
 *  @return  error code
 */

int CPUDestruct (cpu_t* p_cpu);

//------------------------------------------------------------------------------
/*! @brief   Prints a section of code with an error to the console and to the log file.
 * 
 *  @param   p_cpu       Pointer to the CPU
 *  @param   logname     Name of the log file
 *  @param   err         Error code
 */

void printCode (cpu_t* p_cpu, const char* logname, int err);

//------------------------------------------------------------------------------
/*! @brief   Pop one int number from stack.
 * 
 *  @param   num         Pointer to the number
 */

void Pop1IntNumber (cpu_t* p_cpu, NUM_INT_TYPE* num);

//------------------------------------------------------------------------------
/*! @brief   Pop two int numbers from stack.
 * 
 *  @param   num1        Pointer to the first number of stack (top of the stack)
 *  @param   num2        Pointer to the second number of stack
 */

void Pop2IntNumbers (cpu_t* p_cpu, NUM_INT_TYPE* num1, NUM_INT_TYPE* num2);

//------------------------------------------------------------------------------
/*! @brief   Pop one float number from stack.
 * 
 *  @param   num         Pointer to the number
 */

void Pop1FloatNumber (cpu_t* p_cpu, NUM_FLT_TYPE* num);

//------------------------------------------------------------------------------
/*! @brief   Pop two float numbers from stack.
 * 
 *  @param   num1        Pointer to the first number of stack (top of the stack)
 *  @param   num2        Pointer to the second number of stack
 */

void Pop2FloatNumbers (cpu_t* p_cpu, NUM_FLT_TYPE* num1, NUM_FLT_TYPE* num2);

//------------------------------------------------------------------------------

#endif //CPU_H_INCLUDED