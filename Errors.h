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
    "\nThe file has no any symbols!\n"                              ,
    "\nThere are no lines with letters in text!"                    ,
    "\nFailed to allocate memory\n"                                 ,
    "\nStack cracked, canary was killed\n\n"                        ,
      "Stack is empty\n"                                            ,
    "\nThe pointer to the stack is null, data lost\n\n"             ,
    "\nThe pointer to the stack is null, stack lost\n\n"            ,
    "\nCurrent size of stack data is wrong\n\n"                     ,
    "\nStack cracked, hash corrupted\n\n"                           ,
    "",
    "\nStack did not constructed, operation is impossible\n"        ,
    "",
    "\nStack already destructed\n\n"                                ,
    "\nStack already constructed\n"                                 ,
    "",
    "\nThe size of the stack data is larger than the capacity\n\n"  ,
    "\nBad size stack capacity\n\n"                                 ,
};

#endif // ERRORS_DEFINED