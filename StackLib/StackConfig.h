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


#ifndef PTR_T
#define PTR_T
typedef size_t ptr_t;

#include <limits.h>
#define PTR_MAX UINT_MAX
#endif // PTR_T


#define double_PRINT_FORMAT  "%lf"
#define double_PRINT_TYPE    "double"
#define double_POISON         NAN

#define float_PRINT_FORMAT   "%f"
#define float_PRINT_TYPE     "float"
#define float_POISON          NAN

#define int_PRINT_FORMAT     "%d"
#define int_PRINT_TYPE       "int"
#define int_POISON            INT_MAX

#define size_t_PRINT_FORMAT  "%u"
#define size_t_PRINT_TYPE    "size_t"
#define size_t_POISON         UINT_MAX

#define ptr_t_PRINT_FORMAT   "0x%X"
#define ptr_t_PRINT_TYPE     "ptr_t"
#define ptr_t_POISON          PTR_MAX

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
    "Failed to allocate memory"                                     ,
    "Stack cracked, canary was killed"                              ,
    "Stack is empty"                                                ,
    "The pointer to the stack is null, data lost"                   ,
    "The pointer to the stack is null, stack lost"                  ,
    "Current size of stack data is wrong"                           ,
    "Stack cracked, hash corrupted"                                 ,
    "Stack did not constructed, operation is impossible"            ,
    "Stack already destructed"                                      ,
    "Stack already constructed"                                     ,
    "The size of the stack data is larger than the capacity"        ,
    "Bad size stack capacity"                                       ,
};

#endif // ERRORS_DEFINED

#endif // STACK_CONFIG_H_DEFINED
