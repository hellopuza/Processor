/*------------------------------------------------------------------------------
    * File:        Assembler.h                                                 *
    * Description: Declaration of functions and data types used for parsing    *
                   programs on the assembler language.                         *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#define _CRT_SECURE_NO_WARNINGS
//#define NDEBUG


#include "../Errors.h"
#include "../Commands.h"
#include "../StringLib/StringLib.h"
#include <assert.h>
#include <stdio.h>


static const char* assembler_logname = "assembler.log";

#define ASM_ASSERTOK(cond, err, printcode, p_asm, i) if (cond)                                                                 \
                                                     {                                                                         \
                                                         printError(assembler_logname, __FILE__, __LINE__, __FUNCTION__, err); \
                                                         if (printcode) printCode(p_asm, i, assembler_logname, err);           \
                                                         exit(err); /**/                                                       \
                                                     }


const size_t DEFAULT_BCODE_SIZE = 1024;
const size_t DEFAULT_LABEL_NUM  = 8;
const size_t MAX_WORDS_IN_LINE  = 2;

static const char* DELIMETERS = " \t\r\0";

static const char COMMENT = ';';


typedef struct label
{
    char*  name;
    size_t ptr;
    size_t line;
} label_t;

typedef struct labels
{
    label_t* labels;
    size_t   num = DEFAULT_LABEL_NUM;
    size_t   pos = 0;
} labs_t;

typedef struct assembler
{
    int state = ASM_NOT_CONSTRUCTED;

    text_t   input = {};
    bcode_t  bcode = {};
    
    labs_t defined_labels   = {};
    labs_t undefined_labels = {};
} asm_t;


//------------------------------------------------------------------------------
/*! @brief   Assmebler constructor
 *
 *  @param   p_asm       Pointer to assembler
 *  @param   filename    Name of a program file on the assembler language
 *
 *  @return  error code
 */

int AsmConstruct (asm_t* p_asm, const char* filename);

//------------------------------------------------------------------------------
/*! @brief   Assmebler destructor
 *
 *  @param   p_asm       Pointer to assembler
 *
 *  @return  error code
 */

int AsmDestruct (asm_t* p_asm);

//------------------------------------------------------------------------------
/*! @brief   Assembly process.
 *
 *  @note    The program code is divided by lines and then into tokens. Commands, labels, numbers, registers
 *           are recognized, comments are deleted. The program is translated into binary code. Each command or register
 *           has its own binary code, and the labels have addresses to the place in the code.
 *           If a syntax error is found, the process stops and the program section with the error is output.
 *
 *  @param   p_asm       Pointer to assembler
 *
 *  @return  error code
 */

int Assemble (asm_t* p_asm);

//------------------------------------------------------------------------------
/*! @brief   Write binary program text to file.
 *
 *  @param   p_asm       Pointer to assembler
 *  @param   filename    File name for the program binary
 *
 *  @return  error code
 */

int AsmWrite (asm_t* p_asm, const char* filename);

//------------------------------------------------------------------------------
/*! @brief   Command identifier.
 *
 *  @param   word        C string to be recognized
 *
 *  @return  command code if found else NOT_OK
 */

char CMDIdentify (const char* word);

//------------------------------------------------------------------------------
/*! @brief   Register identifier.
 *
 *  @param   word        C string to be recognized
 *
 *  @return  register code if found else NOT_OK
 */

char REGIdentify (const char* word);

//------------------------------------------------------------------------------
/*! @brief   Labels constructor
 *
 *  @param   p_labs      Pointer to labels array
 *  @param   num         Number of labels to be constructed
 *
 *  @return  error code
 */

int LabelsConstruct (labs_t* p_labs, size_t num);

//------------------------------------------------------------------------------
/*! @brief   Labels destructor
 *
 *  @param   p_labs      Pointer to labels array
 *
 *  @return  error code
 */

int LabelsDestruct (labs_t* p_labs);

//------------------------------------------------------------------------------
/*! @brief   Checking if the line is a label
 * 
 *  @note    If the label is defined, the address of the label in the code and its name locate to the labels array.
 *
 *  @param   p_labs      Pointer to labels array
 *  @param   line        Line of the line_t type
 *  @param   pos         Number of the line in the code
 *
 *  @return  error code
 */

int LabelCheck (labs_t* p_labs, line_t line, size_t pos);

//------------------------------------------------------------------------------
/*! @brief   Defining labels
 * 
 *  @note    If the label is found in the defined_labels, then the address of this label
 *           locates in the program code, otherwise this label and its position locate in undefined_labels
 *
 *  @param   p_asm       Pointer to assembler
 *  @param   name        Name of a label
 *  @param   num_scline  Number of a source code line
 *
 *  @return  error code
 */

int LabelDefining (asm_t* p_asm, char* name, size_t num_scline);

//------------------------------------------------------------------------------
/*! @brief   Defining labels
 * 
 *  @note    If the label is found in the defined_labels, then the address of this label
 *           locates in the program code, otherwise this label and its position locate in undefined_labels
 *
 *  @param   p_asm       Pointer to assembler
 *  @param   name        Name of a label
 *  @param   num_scline  Number of a source code line
 *
 *  @return  error code
 */

int LabelRedefine (asm_t* p_asm);

//------------------------------------------------------------------------------

int LabelsExpand (labs_t* p_labs);

//------------------------------------------------------------------------------

char* DeleteComments (line_t* line, const char comment);

//------------------------------------------------------------------------------

void WriteCommandSingle (asm_t* p_asm, char cmd);

//------------------------------------------------------------------------------

void WriteCommandWithNumber (asm_t* p_asm, char cmd, char* word, size_t line, int err);

//------------------------------------------------------------------------------

void WriteCommandWithRegister (asm_t* p_asm, char cmd, char* word, size_t line, int err);

//------------------------------------------------------------------------------

void printCode (text_t text, size_t line, const char* logname, int err);

//------------------------------------------------------------------------------

#endif // ASSEMBLER_H