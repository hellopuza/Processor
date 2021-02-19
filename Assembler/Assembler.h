/*------------------------------------------------------------------------------
    * File:        Assembler.h                                                 *
    * Description: ***********************************                         *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
*///----------------------------------------------------------------------------

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "../Errors.h"
#include "../Commands.h"
#include "../StringLib/StringLib.h"
#include <assert.h>
#include <stdio.h>


const size_t DEFAULT_BCODE_SIZE = 1024;
const size_t MAX_WORDS_IN_LINE  = 2;

static const char* delimeters = " \n\r\0";


typedef struct assembler
{
    text_t  input = {};
    bcode_t bcode = {};
} asm_t;


//------------------------------------------------------------------------------

int AsmConstruct (asm_t* p_asm, const char* filename);

//------------------------------------------------------------------------------

int AsmDestruct (asm_t* p_asm);

//------------------------------------------------------------------------------

int Assemble (asm_t* p_asm);

//------------------------------------------------------------------------------

int AsmWrite (asm_t* p_asm, const char* filename);

//------------------------------------------------------------------------------

char CMDIdentify (const char* word);

//------------------------------------------------------------------------------

char REGIdentify (const char* word);

//------------------------------------------------------------------------------

#endif // ASSEMBLER_H