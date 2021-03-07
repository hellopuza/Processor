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
    NOT_OK = -1                                                     ,
    OK = 0                                                          ,
    NO_MEMORY                                                       ,

    ASM_CONSTRUCTED                                                 ,
    ASM_DESTRUCTED                                                  ,
    ASM_NOT_CONSTRUCTED                                             ,
    ASM_EXTRA_WORD                                                  ,
    ASM_INCORRECT_LABEL_INPUT                                       ,
    ASM_LABEL_DEFINITION_NOT_FOUND                                  ,
    ASM_LABEL_NEED                                                  ,
    ASM_LABEL_REDIFINITION                                          ,
    ASM_NULL_INPUT_ASSEMBLER_PTR                                    ,
    ASM_TOO_MANY_WORDS_IN_LINE                                      ,
    ASM_UNIDENTIFIED_COMMAND                                        ,
    ASM_WRONG_IN_OPERAND_REGISTER                                   ,
    ASM_WRONG_POP_OPERAND_REGISTER                                  ,
    ASM_WRONG_PUSH_OPERAND_NUMBER                                   ,
    ASM_WRONG_PUSH_OPERAND_REGISTER                                 ,

    CPU_CONSTRUCTED                                                 ,
    CPU_DESTRUCTED                                                  ,
    CPU_DIVISION_BY_ZERO                                            ,
    CPU_EMPTY_REGISTER                                              ,
    CPU_INCORRECT_INPUT                                             ,
    CPU_NO_RET_ADDRESS                                              ,
    CPU_NO_SPACE_FOR_NUMBER                                         ,
    CPU_NO_SPACE_FOR_POINTER                                        ,
    CPU_NO_SPACE_FOR_REGISTER                                       ,
    CPU_NOT_CONSTRUCTED                                             ,
    CPU_NULL_INPUT_CPU_PTR                                          ,
    CPU_ROOT_OF_A_NEG_NUMBER                                        ,
    CPU_UNIDENTIFIED_COMMAND                                        ,
    CPU_UNIDENTIFIED_REGISTER                                       ,

    STACK_CANARY_DIED                                               ,
    STACK_CAPACITY_WRONG_VALUE                                      ,
    STACK_EMPTY_STACK                                               ,
    STACK_INCORRECT_HASH                                            ,
    STACK_NULL_DATA_PTR                                             ,
    STACK_NULL_INPUT_STACK_PTR                                      ,
    STACK_NULL_STACK_PTR                                            ,
    STACK_SIZE_BIGGER_CAPACITY                                      ,
    STACK_CONSTRUCTED                                               ,
    STACK_DESTRUCTED                                                ,
    STACK_NOT_CONSTRUCTED                                           ,
    STACK_TOO_MANY_STACKS                                           ,
    STACK_WRONG_CUR_SIZE                                            ,
    STACK_WRONG_INPUT_CAPACITY_VALUE_BIG                            ,
    STACK_WRONG_INPUT_CAPACITY_VALUE_NIL                            ,

    STR_NO_LINES                                                    ,
    STR_NO_SYMB                                                     ,
};

static const char* errstr[] =
{
    "ERROR"                                                         ,
    "OK"                                                            ,
    "Failed to allocate memory"                                     ,

    "Assembler already constructed"                                 ,
    "Assembler already destructed"                                  ,
    "Assembler did not constructed, operation is impossible"        ,
    "Extra word found"                                              ,
    "Incorrect label input"                                         ,
    "Label definition not found"                                    ,
    "A label is needed here"                                        ,
    "Label redifinition"                                            ,
    "The input value of the assembler pointer turned out to be zero",
    "Too many words in line"                                        ,
    "Unidentified command"                                          ,
    "Wrong in operand register"                                     ,
    "Wrong pop operand register"                                    ,
    "Wrong push operand number"                                     ,
    "Wrong push operand register"                                   ,

    "CPU already constructed"                                       ,
    "CPU already destructed"                                        ,
    "Division by zero"                                              ,
    "Register is empty"                                             ,
    "Incorrect input"                                               ,
    "Function return address not found"                             ,
    "Not enough space to determine the number"                      ,
    "Not enough space to determine the pointer"                     ,
    "Not enough space to determine the register"                    ,
    "CPU did not constructed, operation is impossible"              ,
    "The input value of the CPU pointer turned out to be zero"      ,
    "Root of a negative number"                                     ,
    "Unidentified command"                                          ,
    "Unidentified register"                                         ,

    "Stack cracked, canary was killed"                              ,
    "Bad size stack capacity"                                       ,
    "Stack is empty"                                                ,
    "Stack cracked, hash corrupted"                                 ,
    "The pointer to the stack is null, data lost"                   ,
    "The input value of the stack pointer turned out to be zero"    ,
    "The pointer to the stack is null, stack lost"                  ,
    "The size of the stack data is larger than the capacity"        ,
    "Stack already constructed"                                     ,
    "Stack already destructed"                                      ,
    "Stack did not constructed, operation is impossible"            ,
    "Too many stacks created, cannot create a new one"              ,
    "Current size of stack data is wrong"                           ,
    "Wrong capacity value: - is too big"                            ,
    "Wrong capacity value: - is nil"                                ,

    "There are no lines with letters in text!"                      ,
    "The file has no any symbols!"                                  ,
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