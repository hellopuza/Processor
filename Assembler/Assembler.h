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

static const char* ASSEMBLER_LOGNAME = "assembler.log";

#define ASM_ASSERTOK(cond, err, printcode, text, i) if (cond)                                                                  \
                                                    {                                                                          \
                                                      AsmPrintError(ASSEMBLER_LOGNAME, __FILE__, __LINE__, __FUNCTION__, err); \
                                                      if (printcode) AsmPrintCode(text, i, ASSEMBLER_LOGNAME, err);            \
                                                      exit(err); /**/                                                          \
                                                    }


//==============================================================================
/*------------------------------------------------------------------------------
                   Assembler constants and types                               *
*///----------------------------------------------------------------------------
//==============================================================================


const size_t DEFAULT_BCODE_SIZE = 1024;
const size_t DEFAULT_LABEL_NUM  = 8;
const size_t MAX_WORDS_IN_LINE  = 2;

static const char* CODE_TYPE = ".code";
static const char  COMMENT   = ';';

static const char* DELIMETERS = " \t\r\0";

struct Label
{
    char   name[128] = "";
    ptr_t  ptr       = 0;
    size_t line      = 0;
};

struct Labels
{
    Label* labels;
    size_t num = DEFAULT_LABEL_NUM;
    size_t pos = 0;
};

struct Assembler
{
    int state = ASM_NOT_CONSTRUCTED;

    Text    input = {};
    BinCode bcode = {};
    
    Labels defined_labels   = {};
    Labels undefined_labels = {};
};


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

int AsmConstruct (Assembler* p_asm, const char* filename);

//------------------------------------------------------------------------------
/*! @brief   Assmebler destructor.
 *
 *  @param   p_asm       Pointer to the assembler
 *
 *  @return  error code
 */

int AsmDestruct (Assembler* p_asm);

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

int Assemble (Assembler* p_asm);

//------------------------------------------------------------------------------
/*! @brief   Write binary program text to the file.
 *
 *  @param   p_asm       Pointer to the assembler
 *  @param   filename    File name for the binary program
 *
 *  @return  error code
 */

int AsmWrite (Assembler* p_asm, char* filename);

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
 *  @return  pointer to comment in the line if found, else pointer to line, NULL if comment at the begin of the line
 */

char* DeleteComments (Line* line, const char comment);

//------------------------------------------------------------------------------
/*! @brief   Write an int number to the binary code.
 * 
 *  @param   p_asm       Pointer to the assembler
 *  @param   op_word     C string to be recognized as a number
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 *  @param   flag        Additional command flag
 */

void WriteIntNumber (Assembler* p_asm, char* op_word, size_t line, int err);

//------------------------------------------------------------------------------
/*! @brief   Write command without any operands to the binary code.
 * 
 *  @param   p_asm       Pointer to the assembler
 *  @param   cmd_code    Command code
 *  @param   flag        Additional command flag
 */

void WriteCommandSingle (Assembler* p_asm, char cmd_code, char flag);

//------------------------------------------------------------------------------
/*! @brief   Write command with an int number operand to the binary code.
 * 
 *  @param   p_asm       Pointer to the assembler
 *  @param   cmd_code    Command code
 *  @param   op_word     Operand word to be recognized as a number
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 *  @param   flag        Additional command flag
 */

void WriteCommandWithIntNumber (Assembler* p_asm, char cmd_code, char* op_word, size_t line, int err, char flag);

//------------------------------------------------------------------------------
/*! @brief   Write command with a float number operand to the binary code.
 * 
 *  @param   p_asm       Pointer to the assembler
 *  @param   cmd_code    Command code
 *  @param   op_word     Operand word to be recognized as a number
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 */

void WriteCommandWithFloatNumber (Assembler* p_asm, char cmd_code, char* op_word, size_t line, int err);

//------------------------------------------------------------------------------
/*! @brief   Write command with register operand to the binary code.
 * 
 *  @param   p_asm       Pointer to the assembler
 *  @param   cmd_code    Command code
 *  @param   op_word     Operand word to be recognized as a register
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 *  @param   flag        Additional command flag
 */

void WriteCommandWithRegister (Assembler* p_asm, char cmd_code, char* op_word, size_t line, int err, char flag);

//------------------------------------------------------------------------------
/*! @brief   Write register operand to the binary code.
 * 
 *  @param   p_asm       Pointer to the assembler
 *  @param   op_word     Operand word to be recognized as a register
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 */

void WriteRegister (Assembler* p_asm, char* op_word, size_t line, int err);

//------------------------------------------------------------------------------
/*! @brief   Write command with pointer operand to the binary code.
 * 
 *  @param   p_asm       Pointer to the assembler
 *  @param   cmd         Command code
 *  @param   op_word     Operand word to be recognized as a pointer
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 */

void WriteCommandWithPointer (Assembler* p_asm, char cmd, char* op_word, size_t line, int err);

//------------------------------------------------------------------------------
/*! @brief   Labels constructor.
 *
 *  @param   p_labs      Pointer to the labels array
 *  @param   num         Number of labels to be constructed
 *
 *  @return  error code
 */

int LabelsConstruct (Labels* p_labs, size_t num);

//------------------------------------------------------------------------------
/*! @brief   Labels destructor.
 *
 *  @param   p_labs      Pointer to the labels array
 *
 *  @return  error code
 */

int LabelsDestruct (Labels* p_labs);

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

int LabelCheck (Labels* p_labs, Line line, size_t pos);

//------------------------------------------------------------------------------
/*! @brief   Defining labels.
 * 
 *  @note    If the label is found in the defined_labels, then the address of this label
 *           locates in the program code, otherwise this label and its position locate in undefined_labels
 *
 *  @param   p_asm       Pointer to the assembler.
 *  @param   lab_name    Name of a label
 *  @param   line        Number of the line in the code
 *
 *  @return  error code
 */

int LabelDefining (Assembler* p_asm, char* lab_name, size_t line);

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

int LabelRedefine (Assembler* p_asm);

//------------------------------------------------------------------------------
/*! @brief   Increase the labels array by 2 times.
 * 
 *  @param   p_labs      Pointer to the labels array
 * 
 *  @return  error code
 */

int LabelsExpand (Labels* p_labs);

//------------------------------------------------------------------------------
/*! @brief   Prints a section of code with an error to the console and to the log file.
 * 
 *  @param   text        Text structure of the progran code
 *  @param   line        Number of line with an error
 *  @param   logname     Name of the log file
 *  @param   err         Error code
 */

void AsmPrintCode (Text text, size_t line, const char* logname, int err);

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