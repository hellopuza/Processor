/*------------------------------------------------------------------------------
    * File:        StackConfig.h                                               *
    * Description: Stack congigurations                                        *
    * Created:     1 dec 2020                                                  *
    * Copyright:   (C) 2020 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
*///----------------------------------------------------------------------------

#ifndef STACK_CONFIG_H_DEFINED
#define STACK_CONFIG_H_DEFINED


#include <limits.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define CONSOLE_PRINT if(1)
#define DUMP_PRINT    if(1)
static const char* logname = "stack.log";

#define CANARY_PROTECT
#define HASH_PROTECT


#define double_PRINT_FORMAT  "%lf"
#define double_PRINT_TYPE    "double"
#define double_POISON         NAN

#define float_PRINT_FORMAT   "%f"
#define float_PRINT_TYPE     "float"
#define float_POISON          NAN

#define int_PRINT_FORMAT     "%d"
#define int_PRINT_TYPE       "int"
#define int_POISON            INT_MAX

#define char_PRINT_FORMAT    "%c"
#define char_PRINT_TYPE      "char"
#define char_POISON          '\0'


typedef size_t error_t;

static const size_t MAX_STACK_NUM = 100;
static const size_t MAX_CAPACITY  = 100000;


#ifdef CANARY_PROTECT

typedef unsigned long long can_t;
#define CAN_PRINT_FORMAT "%llu"

static can_t perfect_canary = (srand(time(NULL)), rand());
static can_t canaries[MAX_STACK_NUM] = {};

#endif //CANARY_PROTECT

#ifndef ERRORS_DEFINED
#define ERRORS_DEFINED

enum Errors
{
    NOT_OK = -1                                                     ,
    OK = 0                                                          ,
    NO_MEMORY                                                       ,
    CANARY_DIED                                                     ,
    EMPTY_STACK                                                     ,
    NULL_DATA_PTR                                                   ,
    NULL_STACK_PTR                                                  ,
    WRONG_CUR_SIZE                                                  ,
    INCORRECT_HASH                                                  ,
    NOT_CONSTRUCTED                                                 ,
    STACK_DESTRUCTED                                                ,
    STACK_CONSTRUCTED                                               ,
    SIZE_BIGGER_CAPACITY                                            ,
    CAPACITY_WRONG_VALUE                                            ,
};

static const char* errstr[] =
{
    "ERROR"                                                         ,
    "OK"                                                            ,
    "\nFailed to allocate memory\n"                                 ,
    "\nStack cracked, canary was killed\n"                          ,
    "\nStack is empty\n"                                            ,
    "\nThe pointer to the stack is null, data lost\n"               ,
    "\nThe pointer to the stack is null, stack lost\n"              ,
    "\nCurrent size of stack data is wrong\n"                       ,
    "\nStack cracked, hash corrupted\n"                             ,
    "\nStack did not constructed, operation is impossible\n"        ,
    "\nStack already destructed\n"                                  ,
    "\nStack already constructed\n"                                 ,
    "\nThe size of the stack data is larger than the capacity\n"    ,
    "\nBad size stack capacity\n"                                   ,
};

#endif // ERRORS_DEFINED

#endif // STACK_CONFIG_H_DEFINED
