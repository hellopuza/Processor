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

#define _CRT_SECURE_NO_WARNINGS
//#define NDEBUG


#include "../Errors.h"
#include "../Commands.h"
#include "../StringLib/StringLib.h"
#include <assert.h>
#include <stdio.h>


const size_t DEFAULT_BCODE_SIZE = 1024;
const size_t DEFAULT_LABEL_NUM  = 8;
const size_t MAX_WORDS_IN_LINE  = 2;

static const char* delimeters = " \t\r\0";


typedef struct label
{
    char*  name;
    size_t ptr;
} label_t;

typedef struct labels
{
    label_t* labels;
    size_t   num = DEFAULT_LABEL_NUM;
    size_t   pos = 0;
} labs_t;

typedef struct assembler
{
    text_t   input = {};
    bcode_t  bcode = {};
    
    labs_t defined_labels   = {};
    labs_t undefined_labels = {};
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

int LabelsConstruct (labs_t* p_labs, size_t size);

//------------------------------------------------------------------------------

int LabelsDestruct (labs_t* p_labs);

//------------------------------------------------------------------------------

int LabelCheck (labs_t* p_labs, line_t line, size_t pos);

//------------------------------------------------------------------------------

int LabelFind (asm_t* p_asm, char* name);

//------------------------------------------------------------------------------

int LabelReturn (asm_t* p_asm);

//------------------------------------------------------------------------------

int LabelsExpand (labs_t* p_labs);

//------------------------------------------------------------------------------

#endif // ASSEMBLER_H