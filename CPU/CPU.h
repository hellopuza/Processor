/*------------------------------------------------------------------------------
    * File:        CPU.h                                                       *
    * Description: Declaration of functions and data types used for executing  *
                   binary programs                                             *
    * Created:     7 feb 2021                                                  *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
//#define NDEBUG

#include "../TXLib.h"

#include <assert.h>

#include "../Commands.h"
#include "../StringLib/StringLib.h"

#define NO_DUMP
#include "../StackLib/Stack.h"
#undef NO_DUMP


//==============================================================================
/*------------------------------------------------------------------------------
                   CPU errors                                                  *
*///----------------------------------------------------------------------------
//==============================================================================


enum CPUErrors
{
    CPU_NOT_OK = -1                                                    ,
    CPU_OK = 0                                                         ,
    CPU_NO_MEMORY                                                      ,

    CPU_DESTRUCTED                                                     ,
    CPU_DIVISION_BY_ZERO                                               ,
    CPU_EMPTY_REGISTER                                                 ,
    CPU_INCORRECT_INPUT                                                ,
    CPU_INCORRECT_WINDOW_SIZES                                         ,
    CPU_NO_RET_ADDRESS                                                 ,
    CPU_NO_SPACE_FOR_NUMBER_INT                                        ,
    CPU_NO_SPACE_FOR_NUMBER_FLT                                        ,
    CPU_NO_SPACE_FOR_POINTER                                           ,
    CPU_NO_SPACE_FOR_REGISTER                                          ,
    CPU_NO_VIDEO_MEMORY                                                ,
    CPU_NULL_INPUT_CPU_PTR                                             ,
    CPU_NULL_INPUT_FILENAME                                            ,
    CPU_ROOT_OF_A_NEG_NUMBER                                           ,
    CPU_UNIDENTIFIED_COMMAND                                           ,
    CPU_UNIDENTIFIED_REGISTER                                          ,
    CPU_WRONG_ADDR                                                     ,
};

static const char* cpu_errstr[] =
{
    "ERROR"                                                            ,
    "OK"                                                               ,
    "Failed to allocate memory"                                        ,

    "CPU has already destructed"                                       ,
    "Division by zero"                                                 ,
    "Register is empty"                                                ,
    "Incorrect input"                                                  ,
    "Incorrect window sizes received"                                  ,
    "Function return address not found"                                ,
    "Not enough space to determine the int number"                     ,
    "Not enough space to determine the float number"                   ,
    "Not enough space to determine the pointer"                        ,
    "Not enough space to determine the register"                       ,
    "No video memory"                                                  ,
    "The input value of the CPU pointer turned out to be zero"         ,
    "The input value of the CPU filename turned out to be zero"        ,
    "Root of a negative number"                                        ,
    "Unidentified command"                                             ,
    "Unidentified register"                                            ,
    "Memory access violation"                                          ,
};

static const char* CPU_LOGNAME = "cpu.log";

#define CPU_ASSERTOK(cond, err, p_cpu) if (cond)                                                            \
                                       {                                                                    \
                                         CPUPrintError(CPU_LOGNAME, __FILE__, __LINE__, __FUNCTION__, err); \
                                         if (p_cpu != nullptr) PrintCode(CPU_LOGNAME, err);                 \
                                         stkCPU_INT_.Dump(__FUNCTION__, CPU_LOGNAME);                       \
                                         stkCPU_FLT_.Dump(__FUNCTION__, CPU_LOGNAME);                       \
                                         stkCPU_PTR_.Dump(__FUNCTION__, CPU_LOGNAME);                       \
                                         exit(err); /**/                                                    \
                                       }


//==============================================================================
/*------------------------------------------------------------------------------
                   CPU constants and types                                     *
*///----------------------------------------------------------------------------
//==============================================================================


const double NIL                    = 1e-7;
const size_t RAM_SIZE               = 2097152; // 2 MB
const size_t PIXEL_SIZE             = 3;

class CPU
{
private:

    int state_;
    int screens_num_ = 0;

    char* filename_ = nullptr;

    BinCode bcode_;

    char* RAM_ = nullptr;

    Stack<INT_TYPE> stkCPU_INT_;
    Stack<FLT_TYPE> stkCPU_FLT_;
    Stack<PTR_TYPE> stkCPU_PTR_;

    FLT_TYPE registers_[REG_NUM] = {};

public:

//------------------------------------------------------------------------------
/*! @brief   CPU constructor.
 *
 *  @param   filename    Name of a binary code file
 */

    CPU (char* filename);

//------------------------------------------------------------------------------
/*! @brief   CPU destructor.
 */

   ~CPU ();

//------------------------------------------------------------------------------
/*! @brief   Execution process.
 *
 *  @note    The program text is a binary code. The processor goes through the code,
 *           executes commands, goes to the addresses in the code. If an error is found,
 *           the process stops and the code section with the error is output.
 *
 *  @return  error code
 */

    int Execute ();

/*------------------------------------------------------------------------------
                   Private functions                                           *
*///----------------------------------------------------------------------------

private:

//------------------------------------------------------------------------------
/*! @brief   Pop one int number from stack.
 * 
 *  @param   num         Pointer to the number
 */

    void Pop1IntNumber (INT_TYPE* num);

//------------------------------------------------------------------------------
/*! @brief   Pop two int numbers from stack.
 * 
 *  @param   num1        Pointer to the first number of stack (top of the stack)
 *  @param   num2        Pointer to the second number of stack
 */

    void Pop2IntNumbers (INT_TYPE* num1, INT_TYPE* num2);

//------------------------------------------------------------------------------
/*! @brief   Pop one float number from stack.
 * 
 *  @param   num         Pointer to the number
 */

    void Pop1FloatNumber (FLT_TYPE* num);

//------------------------------------------------------------------------------
/*! @brief   Pop two float numbers from stack.
 * 
 *  @param   num1        Pointer to the first number of stack (top of the stack)
 *  @param   num2        Pointer to the second number of stack
 */

    void Pop2FloatNumbers (FLT_TYPE* num1, FLT_TYPE* num2);

//------------------------------------------------------------------------------
/*! @brief   Prints a section of code with an error to the console and to the log file.
 * 
 *  @param   logname     Name of the log file
 *  @param   err         Error code
 */

    void PrintCode (const char* logname, int err);

//------------------------------------------------------------------------------
};

//------------------------------------------------------------------------------
/*! @brief   Prints an error wih description to the console and to the log file.
 * 
 *  @param   logname     Name of the log file
 *  @param   file        Name of the program file
 *  @param   line        Number of line with an error
 *  @param   function    Name of the function with an error
 *  @param   err         Error code
 */

void CPUPrintError (const char* logname, const char* file, int line, const char* function, int err);

//------------------------------------------------------------------------------

#endif //CPU_H_INCLUDED
