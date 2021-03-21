/*------------------------------------------------------------------------------
    * File:        Disassembler.h                                              *
    * Description: Declaration of functions and data types used for            *
    *              disassembling bibary programs.                              *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#define _CRT_SECURE_NO_WARNINGS
//#define NDEBUG

#include <time.h>
#include <assert.h>
#include "../Commands.h"
#include "../StringLib/StringLib.h"


//==============================================================================
/*------------------------------------------------------------------------------
                   Disassembler errors                                         *
*///----------------------------------------------------------------------------
//==============================================================================


enum DisassemblerErrors
{
    DSM_NOT_OK = -1                                                    ,
    DSM_OK = 0                                                         ,
    DSM_NO_MEMORY                                                      ,

    DSM_CONSTRUCTED                                                    ,
    DSM_DESTRUCTED                                                     ,
    DSM_NO_SPACE_FOR_NUMBER_INT                                        ,
    DSM_NO_SPACE_FOR_NUMBER_FLT                                        ,
    DSM_NO_SPACE_FOR_POINTER                                           ,
    DSM_NO_SPACE_FOR_REGISTER                                          ,
    DSM_NOT_CONSTRUCTED                                                ,
    DSM_NULL_INPUT_DISASSEMBLER_PTR                                    ,
    DSM_UNIDENTIFIED_COMMAND                                           ,
    DSM_UNIDENTIFIED_REGISTER                                          ,
};

static const char* asm_errstr[] =
{
    "ERROR"                                                            ,
    "OK"                                                               ,
    "Failed to allocate memory"                                        ,

    "Disassembler already constructed"                                 ,
    "Disassembler already destructed"                                  ,
    "Not enough space to determine the int number"                     ,
    "Not enough space to determine the float number"                   ,
    "Not enough space to determine the pointer"                        ,
    "Not enough space to determine the register"                       ,
    "Disassembler did not constructed, operation is impossible"        ,
    "The input value of the disassembler pointer turned out to be zero",
    "Unidentified command"                                             ,
    "Unidentified register"                                            ,
};

static const char* DISASSEMBLER_LOGNAME = "disassembler.log";

#define DSM_ASSERTOK(cond, err, printcode, p_dsm) if (cond)                                                                     \
                                                  {                                                                             \
                                                    DsmPrintError(DISASSEMBLER_LOGNAME, __FILE__, __LINE__, __FUNCTION__, err); \
                                                    if (printcode) DsmPrintCode(p_dsm, DISASSEMBLER_LOGNAME, err);              \
                                                    exit(err); /**/                                                             \
                                                  }


//==============================================================================
/*------------------------------------------------------------------------------
                   Disassembler constants and types                            *
*///----------------------------------------------------------------------------
//==============================================================================


static const char* CODE_TYPE = "_dis.asm";
static const char* LABEL     = "L";
static const char COMMENT    = ';';

const size_t DEFAULT_LABEL_NUM = 8;
const size_t DEFAULT_LINES_NUM = 32;
const size_t MAX_CHARS_IN_LINE = 128;
const size_t SECOND_WORD_PLACE = 8;
const size_t COMMENT_PLACE     = 32;

struct Label
{
    size_t code = 0;
    size_t addr = 0;
};

struct Labels
{
    Label* data = nullptr;
    size_t num  = DEFAULT_LABEL_NUM;
    size_t pos  = 0;
};

struct Disassembler
{
    int state = DSM_NOT_CONSTRUCTED;

    Text    output = {};
    BinCode bcode  = {};
    Labels  labels = {};
};


//==============================================================================
/*------------------------------------------------------------------------------
                   Disassembler implementations                                *
*///----------------------------------------------------------------------------
//==============================================================================

//------------------------------------------------------------------------------
/*! @brief   Disassmebler constructor.
 *
 *  @param   p_dsm       Pointer to the disassembler
 *  @param   filename    Name of a binary code file
 *
 *  @return  error code
 */

int DsmConstruct (Disassembler* p_dsm, const char* filename);

//------------------------------------------------------------------------------
/*! @brief   Disassmebler destructor.
 *
 *  @param   p_dsm       Pointer to the disassembler
 *
 *  @return  error code
 */

int DsmDestruct (Disassembler* p_dsm);

//------------------------------------------------------------------------------
/*! @brief   Disassembly process.
 *
 *  @note    The program text is a binary code. The disassembler goes through the code
 *           and translating commands with their operands to the asm file. Also it
 *           writes section of code on the left like comment, for each command.
 *           If an error is found, the process stops and the code section with the error
 *           is output.
 *
 *  @param   p_dsm       Pointer to the disassembler
 *
 *  @return  error code
 */

int Disassemble (Disassembler* p_dsm);

//------------------------------------------------------------------------------
/*! @brief   Write command to the text line.
 *
 *  @param   text        Pointer to the text
 *  @param   cmd_code    Command code
 *  @param   line        Line number
 *  @param   endpos      End position in the line
 *
 *  @return  error code
 */

int writeCMD (Text* text, char cmd_code, size_t line, size_t endpos);

//------------------------------------------------------------------------------
/*! @brief   Write operands to the text line.
 *
 *  @param   text        Pointer to the text
 *  @param   cmd_code    Command code
 *  @param   reg_code    Register code
 *  @param   num_int     Int number
 *  @param   num_flt     Float number
 *  @param   num_ptr     Pointer number
 *  @param   line        Line number
 *  @param   ptr         Pointer to the binary code
 *  @param   startpos    Start position in the line
 *  @param   endpos      End position in the line
 *
 *  @return  error code
 */

int writeOperands (Text* text, char cmd_code, char reg_code, NUM_INT_TYPE num_int, NUM_FLT_TYPE num_flt, ptr_t num_ptr, size_t lab_num, size_t line, size_t startpos, size_t endpos);

//------------------------------------------------------------------------------
/*! @brief   Prints a section of code with command and operands to the text line like comment.
 *
 *  @param   text        Pointer to the text
 *  @param   bcode       Pointer to the binary code
 *  @param   line        Line number
 *  @param   pos         Start position in the line
 *  @param   ptr         Pointer to the binary code
 *  @param   size        Section of the code size
 *  @param   comment     Comment character
 */

void writeCode (Text* text, BinCode* bcode, size_t line, size_t pos, ptr_t ptr, size_t size, const char comment);

//------------------------------------------------------------------------------
/*! @brief   Write disassembled program text to the file.
 *
 *  @param   p_dsm       Pointer to the disassembler
 *  @param   filename    File name for the program binary
 *
 *  @return  error code
 */

int DsmWrite (Disassembler* p_dsm, char* filename);

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
/*! @brief   Increase the labels array by 2 times.
 * 
 *  @param   p_labs      Pointer to the labels array
 * 
 *  @return  error code
 */

int LabelsExpand (Labels* p_labs);

//------------------------------------------------------------------------------
/*! @brief   Compare two labels by their addresses.
 *
 *  @param   p1          Pointer to the first label
 *  @param   p2          Pointer to the second label
 *
 *  @return  positive integer if first label address bigger then second
 *  @return  0 if first label address the same as second
 *  @return  negative integer if first label address smaller then second
 */

int labcmp (const void* p1, const void* p2);

//------------------------------------------------------------------------------
/*! @brief   Prints a section of code with an error to the console and to the log file.
 * 
 *  @param   p_dsm       Pointer to the disassembler
 *  @param   logname     Name of the log file
 *  @param   err         Error code
 */

void DsmPrintCode (Disassembler* p_dsm, const char* logname, int err);

//------------------------------------------------------------------------------
/*! @brief   Prints an error wih description to the console and to the log file.
 * 
 *  @param   logname     Name of the log file
 *  @param   file        Name of the program file
 *  @param   line        Number of line with an error
 *  @param   function    Name of the function with an error
 *  @param   err         Error code
 */

void DsmPrintError (const char* logname, const char* file, int line, const char* function, int err);

//------------------------------------------------------------------------------

#endif // DISASSEMBLER_H