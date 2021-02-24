/*------------------------------------------------------------------------------
    * File:        Errors.h                                                    *
    * Description: Error descriptions                                          *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
*///----------------------------------------------------------------------------

#ifndef ERRORS_DEFINED
#define ERRORS_DEFINED

enum Errors
{
    NOT_OK = -1                                                     ,
    OK = 0                                                          ,
    NO_SYMB                                                         ,
    NO_LINES                                                        ,
    NO_MEMORY                                                       ,
    CANARY_DIED                                                     ,
    EMPTY_STACK                                                     ,
    NULL_DATA_PTR                                                   ,
    NULL_STACK_PTR                                                  ,
    WRONG_CUR_SIZE                                                  ,
    INCORRECT_HASH                                                  ,
    INCORRECT_INPUT                                                 ,
    NOT_CONSTRUCTED                                                 ,
    DIVISION_BY_ZERO                                                ,
    STACK_DESTRUCTED                                                ,
    STACK_CONSTRUCTED                                               ,
    ROOT_OF_A_NEG_NUMBER                                            ,
    SIZE_BIGGER_CAPACITY                                            ,
    CAPACITY_WRONG_VALUE                                            ,
};

static const char* errstr[] =
{
    "ERROR"                                                         ,
    "OK"                                                            ,
    "The file has no any symbols!"                                  ,
    "There are no lines with letters in text!"                      ,
    "Failed to allocate memory"                                     ,
    "Stack cracked, canary was killed"                              ,
    "Stack is empty"                                                ,
    "The pointer to the stack is null, data lost"                   ,
    "The pointer to the stack is null, stack lost"                  ,
    "Current size of stack data is wrong"                           ,
    "Stack cracked, hash corrupted"                                 ,
    "",
    "Stack did not constructed, operation is impossible"            ,
    "",
    "Stack already destructed"                                      ,
    "Stack already constructed"                                     ,
    "",
    "The size of the stack data is larger than the capacity"        ,
    "Bad size stack capacity"                                       ,
};

#endif // ERRORS_DEFINED