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


#include <time.h>
#include <assert.h>
#include "../Commands.h"
#include "../StringLib/StringLib.h"


//==============================================================================
/*------------------------------------------------------------------------------
                   Assembler errors                                            *
*///----------------------------------------------------------------------------
//==============================================================================


enum AssemblerErrors
{
    ASM_NOT_OK = -1                                                    ,
    ASM_OK = 0                                                         ,
    ASM_NO_MEMORY                                                      ,

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
};

static const char* asm_errstr[] =
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
};

#define ASM_ASSERTOK(cond, err, printcode, p_asm, i) if (cond)                                                                  \
                                                     {                                                                          \
                                                       AsmPrintError(assembler_logname, __FILE__, __LINE__, __FUNCTION__, err); \
                                                       if (printcode) AsmPrintCode(p_asm, i, assembler_logname, err);           \
                                                       exit(err); /**/                                                          \
                                                     }


//==============================================================================
/*------------------------------------------------------------------------------
                   Assembler constants and types                               *
*///----------------------------------------------------------------------------
//==============================================================================


static const char* assembler_logname = "assembler.log";

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


//==============================================================================
/*------------------------------------------------------------------------------
                   Assembler implementations                                   *
*///----------------------------------------------------------------------------
//==============================================================================

//------------------------------------------------------------------------------
/*! @brief   Assmebler constructor.
 *
 *  @param   p_asm       Pointer to the assembler
 *  @param   filename    Name of a program file on the assembler language
 *
 *  @return  error code
 */

int AsmConstruct (asm_t* p_asm, const char* filename);

//------------------------------------------------------------------------------
/*! @brief   Assmebler destructor.
 *
 *  @param   p_asm       Pointer to the assembler
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
 *  @param   p_asm       Pointer to the assembler
 *
 *  @return  error code
 */

int Assemble (asm_t* p_asm);

//------------------------------------------------------------------------------
/*! @brief   Write binary program text to file.
 *
 *  @param   p_asm       Pointer to the assembler
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
/*! @brief   Delete comments in the line.
 * 
 *  @param   line        Pointer to the line structure
 *  @param   comment     Comment char
 * 
 *  @return  position of comment in the line if found, else pointer to line, NULL if comment at the begin of the line
 */

char* DeleteComments (line_t* line, const char comment);

//------------------------------------------------------------------------------
/*! @brief   Write an int number to the binary code.
 * 
 *  @param   p_asm       Pointer to the assembler
 *  @param   word        C string to be recognized as a number
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 *  @param   flag        Additional command flag
 */

void WriteIntNumber (asm_t* p_asm, char* word, size_t line, int err);

//------------------------------------------------------------------------------
/*! @brief   Write command without any operands to the binary code.
 * 
 *  @param   p_asm       Pointer to the assembler
 *  @param   cmd         Command code
 *  @param   flag        Additional command flag
 */

void WriteCommandSingle (asm_t* p_asm, char cmd, char flag);

//------------------------------------------------------------------------------
/*! @brief   Write command with an int number operand to the binary code.
 * 
 *  @param   p_asm       Pointer to the assembler
 *  @param   cmd         Command code
 *  @param   word        C string to be recognized as a number
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 *  @param   flag        Additional command flag
 */

void WriteCommandWithIntNumber (asm_t* p_asm, char cmd, char* word, size_t line, int err, char flag);

//------------------------------------------------------------------------------
/*! @brief   Write command with a float number operand to the binary code.
 * 
 *  @param   p_asm       Pointer to the assembler
 *  @param   cmd         Command code
 *  @param   word        C string to be recognized as a number
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 */

void WriteCommandWithFloatNumber (asm_t* p_asm, char cmd, char* word, size_t line, int err);

//------------------------------------------------------------------------------
/*! @brief   Write command with register operand to the binary code.
 * 
 *  @param   p_asm       Pointer to the assembler
 *  @param   cmd         Command code
 *  @param   word        C string to be recognized as a register
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 *  @param   flag        Additional command flag
 */

void WriteCommandWithRegister (asm_t* p_asm, char cmd, char* word, size_t line, int err, char flag);

//------------------------------------------------------------------------------
/*! @brief   Write command with pointer operand to the binary code.
 * 
 *  @param   p_asm       Pointer to the assembler
 *  @param   cmd         Command code
 *  @param   word        C string to be recognized as a register
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 */

void WriteCommandWithPointer (asm_t* p_asm, char cmd, char* word, size_t line, int err);

//------------------------------------------------------------------------------
/*! @brief   Labels constructor.
 *
 *  @param   p_labs      Pointer to the labels array
 *  @param   num         Number of labels to be constructed
 *
 *  @return  error code
 */

int LabelsConstruct (labs_t* p_labs, size_t num);

//------------------------------------------------------------------------------
/*! @brief   Labels destructor.
 *
 *  @param   p_labs      Pointer to the labels array
 *
 *  @return  error code
 */

int LabelsDestruct (labs_t* p_labs);

//------------------------------------------------------------------------------
/*! @brief   Checking if the line is a label.
 * 
 *  @note    If the label is defined, the address of the label in the code and its name locate to the labels array.
 *
 *  @param   p_labs      Pointer to the labels array
 *  @param   line        Line structure of the program code
 *  @param   pos         Number of the line in the code
 *
 *  @return  error code
 */

int LabelCheck (labs_t* p_labs, line_t line, size_t pos);

//------------------------------------------------------------------------------
/*! @brief   Defining labels.
 * 
 *  @note    If the label is found in the defined_labels, then the address of this label
 *           locates in the program code, otherwise this label and its position locate in undefined_labels
 *
 *  @param   p_asm       Pointer to the assembler.
 *  @param   name        Name of a label
 *  @param   num_scline  Number of a source code line
 *
 *  @return  error code
 */

int LabelDefining (asm_t* p_asm, char* name, size_t num_scline);

//------------------------------------------------------------------------------
/*! @brief   Redefinition of undefined labels.
 * 
 *  @note    There is a process of redefinition by an array of undefined labels.
 *           Each label is searched for among the defined ones, if not found then an error is displayed.
 *
 *  @param   p_asm       Pointer to the assembler
 * 
 *  @return  -1 if ok, else the number of the undefined label in the array
 */

int LabelRedefine (asm_t* p_asm);

//------------------------------------------------------------------------------
/*! @brief   Increase the labels array by 2 times.
 * 
 *  @param   p_labs      Pointer to the labels array
 * 
 *  @return  error code
 */

int LabelsExpand (labs_t* p_labs);

//------------------------------------------------------------------------------
/*! @brief   Prints a section of code with an error to the console and to the log file.
 * 
 *  @param   text        Text structure of the progran code
 *  @param   line        Number of line with an error
 *  @param   logname     Name of the log file
 *  @param   err         Error code
 */

void AsmPrintCode (text_t text, size_t line, const char* logname, int err);

//------------------------------------------------------------------------------
/*! @brief   Prints an error wih description to the console and to the log file.
 * 
 *  @param   logname     Name of the log file
 *  @param   file        Name of the program file
 *  @param   line        Number of line with an error
 *  @param   function    Name of the function with an error
 *  @param   err         Error code
 */

void AsmPrintError (const char* logname, const char* file, int line, const char* function, int err);

//------------------------------------------------------------------------------

#endif // ASSEMBLER_H