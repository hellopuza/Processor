/*------------------------------------------------------------------------------
    * File:        Assembler.h                                                 *
    * Description: Declaration of functions and data types used for parsing    *
                   programs on the assembler language.                         *
    * Created:     7 feb 2021                                                  *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

#ifndef ASSEMBLER_H_INCLUDED
#define ASSEMBLER_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
//#define NDEBUG


#if defined (__GNUC__) || defined (__clang__) || defined (__clang_major__)
    #define __FUNC_NAME__   __PRETTY_FUNCTION__

#elif defined (_MSC_VER)
    #define __FUNC_NAME__   __FUNCSIG__

#else
    #define __FUNC_NAME__   __FUNCTION__

#endif


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

    ASM_DESTRUCTED                                                     ,
    ASM_EXTRA_WORD                                                     ,
    ASM_INCORRECT_LABEL_INPUT                                          ,
    ASM_LABEL_DEFINITION_NOT_FOUND                                     ,
    ASM_LABEL_NEED                                                     ,
    ASM_LABEL_REDIFINITION                                             ,
    ASM_LABELS_DESTRUCTED                                              ,
    ASM_NULL_INPUT_ASSEMBLER_PTR                                       ,
    ASM_NULL_INPUT_FILENAME                                            ,
    ASM_NULL_INPUT_LABELS_PTR                                          ,
    ASM_TOO_MANY_WORDS_IN_LINE                                         ,
    ASM_UNIDENTIFIED_COMMAND                                           ,
    ASM_WRONG_IN_OPERAND_REGISTER                                      ,
    ASM_WRONG_OUT_OPERAND_REGISTER                                     ,
    ASM_WRONG_POP_OPERAND_POINTER                                      ,
    ASM_WRONG_POP_OPERAND_REGISTER                                     ,
    ASM_WRONG_PUSH_OPERAND_NULL                                        ,
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

    "Assembler has already destructed"                                 ,
    "Extra word found"                                                 ,
    "Incorrect label input"                                            ,
    "Label definition not found"                                       ,
    "A label is needed here"                                           ,
    "Label redifinition"                                               ,
    "Labels have already destructed"                                   ,
    "The input value of the assembler pointer turned out to be zero"   ,
    "The input value of the assembler filename turned out to be zero"  ,
    "The input value of the labels pointer turned out to be zero"      ,
    "Too many words in line"                                           ,
    "Unidentified command"                                             ,
    "Wrong in operand register"                                        ,
    "Wrong out operand register"                                       ,
    "Wrong pop operand pointer"                                        ,
    "Wrong pop operand register"                                       ,
    "Instruction push/pushq implies operand"                           ,
    "Wrong push operand number. Operand can only be an int number"     ,
    "Wrong push operand pointer"                                       ,
    "Wrong push operand register"                                      ,
    "Wrong pushq operand number. Operand can only be a float number"   ,
    "Wrong screen operand. Operand can only be a register"             ,
};

static const char* ASSEMBLER_LOGNAME = "assembler.log";

#define ASM_ASSERTOK(cond, err, line) if (cond)                                                                   \
                                      {                                                                           \
                                        AsmPrintError(ASSEMBLER_LOGNAME, __FILE__, __LINE__, __FUNC_NAME__, err); \
                                        if (line > -1) PrintCode(line, ASSEMBLER_LOGNAME, err);                   \
                                        exit(err); /**/                                                           \
                                      }

#define LABS_ASSERTOK(cond, err) if (cond)                                                                   \
                                 {                                                                           \
                                   AsmPrintError(ASSEMBLER_LOGNAME, __FILE__, __LINE__, __FUNC_NAME__, err); \
                                   exit(err); /**/                                                           \
                                 }


//==============================================================================
/*------------------------------------------------------------------------------
                   Labels constants and types                                  *
*///----------------------------------------------------------------------------
//==============================================================================


const size_t DEFAULT_LABELS_NUM = 8;

struct Label
{
    char   name[128] = "";
    ptr_t  ptr       = 0;
    size_t line      = 0;
};

class Labels
{
    int state_;

public:

    Label* labels_ = nullptr;
    size_t num_    = 0;
    size_t pos_    = 0;

//------------------------------------------------------------------------------
/*! @brief   Labels constructor.
 */

    Labels ();

//------------------------------------------------------------------------------
/*! @brief   Labels copy constructor (deleted).
 *
 *  @param   obj         Source labels
 */

    Labels (const Labels& obj);

    Labels& operator = (const Labels& obj); // deleted

//------------------------------------------------------------------------------
/*! @brief   Labels destructor.
 */

   ~Labels ();

//------------------------------------------------------------------------------
/*! @brief   Checking if the line is a label.
 *
 *  @note    If the label is defined, the address of the label in the code and its name locate to the labels array.
 *
 *  @param   line        Line structure of the program code
 *  @param   pos         Number of the line in the code
 *
 *  @return  error code
 */

    int Check (Line line, size_t pos);

//------------------------------------------------------------------------------
/*! @brief   Increase the labels array by 2 times.
 * 
 *  @return  error code
 */

    int Expand ();

//------------------------------------------------------------------------------
};


//==============================================================================
/*------------------------------------------------------------------------------
                   Assembler constants and types                               *
*///----------------------------------------------------------------------------
//==============================================================================


const size_t DEFAULT_BCODE_SIZE = 1024;
const size_t MAX_WORDS_IN_LINE  = 2;

static const char* CODE_TYPE = ".code";
static const char  COMMENT   = ';';

static const char* DELIMETERS = " \t\r\0";


class Assembler
{
private:

    int state_;

    Text    input_;
    BinCode bcode_;

    Labels def_labs_;
    Labels undef_labs_;

    char* prev_line_ = nullptr;

public:

//------------------------------------------------------------------------------
/*! @brief   Assmebler constructor.
 *
 *  @param   filename    Name of a program file on the assembler language
 */

    Assembler (const char* filename);

//------------------------------------------------------------------------------
/*! @brief   Assembler copy constructor (deleted).
 *
 *  @param   obj         Source assembler
 */

    Assembler (const Assembler& obj);

    Assembler& operator = (const Assembler& obj); // deleted

//------------------------------------------------------------------------------
/*! @brief   Assmebler destructor.
 */

   ~Assembler ();

//------------------------------------------------------------------------------
/*! @brief   Assembly process.
 *
 *  @note    The program code is divided by lines and then into tokens. Commands, labels, numbers, registers
 *           are recognized, comments are deleted. The program is translated into binary code. Each command or register
 *           has its own binary code, and the labels have addresses to the place in the code.
 *           If a syntax error is found, the process stops and the program section with the error is output.
 *
 *  @return  error code
 */

    int Assemble ();

//------------------------------------------------------------------------------
/*! @brief   Write binary program text to the file.
 *
 *  @param   filename    File name for the binary program
 *
 *  @return  error code
 */

    int Write (char* filename);

/*------------------------------------------------------------------------------
                   Private functions                                           *
*///----------------------------------------------------------------------------

private:

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
 *  @param   op_word     C string to be recognized as a number
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 *  @param   flag        Additional command flag
 */

    void WriteIntNumber (char* op_word, size_t line, int err);

//------------------------------------------------------------------------------
/*! @brief   Write command without any operands to the binary code.
 *
 *  @param   cmd_code    Command code
 *  @param   flag        Additional command flag
 */

    void WriteCommandSingle (char cmd_code, char flag);

//------------------------------------------------------------------------------
/*! @brief   Write command with an int number operand to the binary code.
 *
 *  @param   cmd_code    Command code
 *  @param   op_word     Operand word to be recognized as a number
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 *  @param   flag        Additional command flag
 */

    void WriteCommandWithIntNumber (char cmd_code, char* op_word, size_t line, int err, char flag);

//------------------------------------------------------------------------------
/*! @brief   Write command with a float number operand to the binary code.
 *
 *  @param   cmd_code    Command code
 *  @param   op_word     Operand word to be recognized as a number
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 */

    void WriteCommandWithFloatNumber (char cmd_code, char* op_word, size_t line, int err);

//------------------------------------------------------------------------------
/*! @brief   Write command with register operand to the binary code.
 *
 *  @param   cmd_code    Command code
 *  @param   op_word     Operand word to be recognized as a register
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 *  @param   flag        Additional command flag
 */

    void WriteCommandWithRegister (char cmd_code, char* op_word, size_t line, int err, char flag);

//------------------------------------------------------------------------------
/*! @brief   Write register operand to the binary code.
 *
 *  @param   op_word     Operand word to be recognized as a register
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 */

    void WriteRegister (char* op_word, size_t line, int err);

//------------------------------------------------------------------------------
/*! @brief   Write command with pointer operand to the binary code.
 *
 *  @param   cmd         Command code
 *  @param   op_word     Operand word to be recognized as a pointer
 *  @param   line        Number of line in the program text
 *  @param   err         Error code
 */

    void WriteCommandWithPointer (char cmd, char* op_word, size_t line, int err);

//------------------------------------------------------------------------------
/*! @brief   Defining labels.
 *
 *  @note    If the label is found in the defined_labels, then the address of this label
 *           locates in the program code, otherwise this label and its position locate in undefined_labels
 *
 *  @param   lab_name    Name of a label
 *  @param   line        Number of the line in the code
 *
 *  @return  error code
 */

    int LabelDefining (char* lab_name, size_t line);

//------------------------------------------------------------------------------
/*! @brief   Redefinition of undefined labels.
 *
 *  @note    There is a process of redefinition by an array of undefined labels.
 *           Each label is searched for among the defined ones, if not found then an error is displayed.
 *
 *  @return  -1 if ok, else the number of the undefined label in the array
 */

    int LabelRedefine ();

//------------------------------------------------------------------------------
/*! @brief   Prints a section of code with an error to the console and to the log file.
 *
 *  @param   line        Number of line with an error
 *  @param   logname     Name of the log file
 *  @param   err         Error code
 */

    void PrintCode (size_t line, const char* logname, int err);

//------------------------------------------------------------------------------
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

void AsmPrintError (const char* logname, const char* file, int line, const char* function, int err);

//------------------------------------------------------------------------------

#endif // ASSEMBLER_H_INCLUDED
