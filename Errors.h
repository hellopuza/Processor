/*------------------------------------------------------------------------------
    * File:        Errors.h                                                    *
    * Description: Error descriptions                                          *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#ifndef ERRORS_DEFINED
#define ERRORS_DEFINED

#include <assert.h>
#include <stdio.h>
#include <time.h>

enum Errors
{
    NOT_OK = -1                                                        ,
    OK = 0                                                             ,
    NO_MEMORY                                                          ,

    ASM_CONSTRUCTED                                                    ,
    ASM_DESTRUCTED                                                     ,
    ASM_NOT_CONSTRUCTED                                                ,
    ASM_EXTRA_WORD                                                     ,
    ASM_INCORRECT_LABEL_INPUT                                          ,
    ASM_LABEL_DEFINITION_NOT_FOUND                                     ,
    ASM_LABEL_NEED                                                     ,
    ASM_LABEL_REDIFINITION                                             ,
    ASM_NULL_INPUT_ASSEMBLER_PTR                                       ,
    ASM_TOO_MANY_WORDS_IN_LINE                                         ,
    ASM_UNIDENTIFIED_COMMAND                                           ,
    ASM_WRONG_IN_OPERAND_REGISTER                                      ,
    ASM_WRONG_OUT_OPERAND_REGISTER                                     ,
    ASM_WRONG_POP_OPERAND_POINTER                                      ,
    ASM_WRONG_POP_OPERAND_REGISTER                                     ,
    ASM_WRONG_PUSH_OPERAND_NUMBER                                      ,
    ASM_WRONG_PUSH_OPERAND_POINTER                                     ,
    ASM_WRONG_PUSH_OPERAND_REGISTER                                    ,
    ASM_WRONG_PUSHQ_OPERAND_NUMBER                                     ,
    ASM_WRONG_SCREEN_OPERAND_REGISTER                                  ,

    CPU_CONSTRUCTED                                                    ,
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
    CPU_NOT_CONSTRUCTED                                                ,
    CPU_NO_VIDEO_MEMORY                                                ,
    CPU_NULL_INPUT_CPU_PTR                                             ,
    CPU_ROOT_OF_A_NEG_NUMBER                                           ,
    CPU_UNIDENTIFIED_COMMAND                                           ,
    CPU_UNIDENTIFIED_REGISTER                                          ,
    CPU_WRONG_ADDR                                                     ,

    STACK_CONSTRUCTED                                                  ,
    STACK_CANARY_DIED                                                  ,
    STACK_CAPACITY_WRONG_VALUE                                         ,
    STACK_DESTRUCTED                                                   ,
    STACK_EMPTY_STACK                                                  ,
    STACK_INCORRECT_HASH                                               ,
    STACK_NOT_CONSTRUCTED                                              ,
    STACK_NULL_DATA_PTR                                                ,
    STACK_NULL_INPUT_STACK_PTR                                         ,
    STACK_NULL_STACK_PTR                                               ,
    STACK_SIZE_BIGGER_CAPACITY                                         ,
    STACK_TOO_MANY_STACKS                                              ,
    STACK_WRONG_CUR_SIZE                                               ,
    STACK_WRONG_INPUT_CAPACITY_VALUE_BIG                               ,
    STACK_WRONG_INPUT_CAPACITY_VALUE_NIL                               ,

    STR_NO_LINES                                                       ,
    STR_NO_SYMB                                                        ,
};

static const char* errstr[] =
{
    "ERROR"                                                            ,
    "OK"                                                               ,
    "Failed to allocate memory"                                        ,

    "Assembler already constructed"                                    ,
    "Assembler already destructed"                                     ,
    "Assembler did not constructed, operation is impossible"           ,
    "Extra word found"                                                 ,
    "Incorrect label input"                                            ,
    "Label definition not found"                                       ,
    "A label is needed here"                                           ,
    "Label redifinition"                                               ,
    "The input value of the assembler pointer turned out to be zero"   ,
    "Too many words in line"                                           ,
    "Unidentified command"                                             ,
    "Wrong in operand register"                                        ,
    "Wrong out operand register"                                       ,
    "Wrong pop operand pointer"                                        ,
    "Wrong pop operand register"                                       ,
    "Wrong push operand number. Operand can only be an int number"     ,
    "Wrong push operand pointer"                                       ,
    "Wrong push operand register"                                      ,
    "Wrong pushq operand number. Operand can only be a float number"   ,
    "Wrong screen operand. Operand can only be a register"             ,

    "CPU already constructed"                                          ,
    "CPU already destructed"                                           ,
    "Division by zero"                                                 ,
    "Register is empty"                                                ,
    "Incorrect input"                                                  ,
    "Incorrect window sizes received"                                  ,
    "Function return address not found"                                ,
    "Not enough space to determine the int number"                     ,
    "Not enough space to determine the float number"                   ,
    "Not enough space to determine the pointer"                        ,
    "Not enough space to determine the register"                       ,
    "CPU did not constructed, operation is impossible"                 ,
    "No video memory"                                                  ,
    "The input value of the CPU pointer turned out to be zero"         ,
    "Root of a negative number"                                        ,
    "Unidentified command"                                             ,
    "Unidentified register"                                            ,
    "Memory access violation"                                          ,

    "Stack already constructed"                                        ,
    "Stack cracked, canary was killed"                                 ,
    "Bad size stack capacity"                                          ,
    "Stack already destructed"                                         ,
    "Stack is empty"                                                   ,
    "Stack cracked, hash corrupted"                                    ,
    "Stack did not constructed, operation is impossible"               ,
    "The pointer to the stack is null, data lost"                      ,
    "The input value of the stack pointer turned out to be zero"       ,
    "The pointer to the stack is null, stack lost"                     ,
    "The size of the stack data is larger than the capacity"           ,
    "Too many stacks created, cannot create a new one"                 ,
    "Current size of stack data is wrong"                              ,
    "Wrong capacity value: - is too big"                               ,
    "Wrong capacity value: - is nil"                                   ,

    "There are no lines with letters in text!"                         ,
    "The file has no any symbols!"                                     ,
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

static void printError(const char* logname, const char* file, int line, const char* function, int err)
{
    assert(function != nullptr);
    assert(logname != nullptr);
    assert(file != nullptr);

    FILE* log = fopen(logname, "a");
    assert(log != nullptr);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    fprintf(log, "###############################################################################\n");
    fprintf(log, "TIME: %d-%02d-%02d %02d:%02d:%02d\n\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fprintf(log, "ERROR: file %s  line %d  function %s\n\n", file, line, function);
    fprintf(log, "%s\n", errstr[err + 1]);

    printf (     "ERROR: file %s  line %d  function %s\n",   file, line, function);
    printf (     "%s\n\n", errstr[err + 1]);

    fclose(log);
}

//------------------------------------------------------------------------------

#endif // ERRORS_DEFINED