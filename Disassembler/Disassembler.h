/*------------------------------------------------------------------------------
    * File:        Disassembler.h                                              *
    * Description: Declaration of functions and data types used for            *
    *              disassembling bibary programs.                              *
    * Created:     7 feb 2021                                                  *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#define _CRT_SECURE_NO_WARNINGS


#if defined (__GNUC__) || defined (__clang__) || defined (__clang_major__)
    #define __FUNC_NAME__   __PRETTY_FUNCTION__
    #define PRINT_PTR       "%p"

#elif defined (_MSC_VER)
    #define __FUNC_NAME__   __FUNCSIG__
    #define PRINT_PTR       "0x%p"

#else
    #define __FUNC_NAME__   __FUNCTION__
    #define PRINT_PTR       "%p"

#endif


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
    DSM_NOT_OK = -1                                                       ,
    DSM_OK = 0                                                            ,
    DSM_NO_MEMORY                                                         ,

    DSM_DESTRUCTED                                                        ,
    DSM_LABELS_DESTRUCTED                                                 ,
    DSM_NO_SPACE_FOR_NUMBER_INT                                           ,
    DSM_NO_SPACE_FOR_NUMBER_FLT                                           ,
    DSM_NO_SPACE_FOR_POINTER                                              ,
    DSM_NO_SPACE_FOR_REGISTER                                             ,
    DSM_NULL_INPUT_DISASSEMBLER_PTR                                       ,
    DSM_NULL_INPUT_FILENAME                                               ,
    DSM_NULL_INPUT_LABELS_PTR                                             ,
    DSM_UNIDENTIFIED_COMMAND                                              ,
    DSM_UNIDENTIFIED_REGISTER                                             ,
};

char const * const dsm_errstr[] =
{
    "ERROR"                                                               ,
    "OK"                                                                  ,
    "Failed to allocate memory"                                           ,

    "Disassembler has already destructed"                                 ,
    "Labels have already destructed"                                      ,
    "Not enough space to determine the int number"                        ,
    "Not enough space to determine the float number"                      ,
    "Not enough space to determine the pointer"                           ,
    "Not enough space to determine the register"                          ,
    "The input value of the disassembler pointer turned out to be zero"   ,
    "The input value of the disassembler filaneme turned out to be zero"  ,
    "The input value of the labels pointer turned out to be zero"         ,
    "Unidentified command"                                                ,
    "Unidentified register"                                               ,
};

char const * const DISASSEMBLER_LOGNAME = "disassembler.log";

#define DSM_ASSERTOK(cond, err, p_dsm) if (cond)                                                                      \
                                       {                                                                              \
                                         DsmPrintError(DISASSEMBLER_LOGNAME, __FILE__, __LINE__, __FUNC_NAME__, err); \
                                         if (p_dsm != nullptr) PrintCode(DISASSEMBLER_LOGNAME);                       \
                                         exit(err);                                                                   \
                                       } //

#define LABS_ASSERTOK(cond, err) if (cond)                                                                      \
                                 {                                                                              \
                                   DsmPrintError(DISASSEMBLER_LOGNAME, __FILE__, __LINE__, __FUNC_NAME__, err); \
                                   exit(err);                                                                   \
                                 } //


//==============================================================================
/*------------------------------------------------------------------------------
                   Labels constants and types                                  *
*///----------------------------------------------------------------------------
//==============================================================================


const size_t DEFAULT_LABELS_NUM = 8;

struct Label
{
    size_t code = 0;
    size_t addr = 0;
};

class Labels
{
public:

    int state_;

    Label* data_ = nullptr;
    size_t num_  = 0;
    size_t pos_  = 0;

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
/*! @brief   Increase the labels array by 2 times.
 * 
 *  @return  error code
 */

    int Expand ();

//------------------------------------------------------------------------------
};


//==============================================================================
/*------------------------------------------------------------------------------
                   Disassembler constants and types                            *
*///----------------------------------------------------------------------------
//==============================================================================


char const * const CODE_TYPE = "_dis.asm";
char const * const LABEL     = "L";
char const         COMMENT   = ';';

const size_t DEFAULT_LINES_NUM = 32;
const size_t MAX_CHARS_IN_LINE = 128;
const size_t SECOND_WORD_PLACE = 8;
const size_t COMMENT_PLACE     = 32;


class Disassembler
{
private:

    int state_;

    Text    output_;
    BinCode bcode_;
    Labels  labels_;

public:

//------------------------------------------------------------------------------
/*! @brief   Disassmebler constructor.
 *
 *  @param   filename    Name of a binary code file
 */

    Disassembler (const char* filename);

//------------------------------------------------------------------------------
/*! @brief   Disassmebler copy constructor (deleted).
 *
 *  @param   obj         Source disassmebler
 */

    Disassembler (const Disassembler& obj);

    Disassembler& operator = (const Disassembler& obj); // deleted

//------------------------------------------------------------------------------
/*! @brief   Disassmebler destructor.
 */

   ~Disassembler ();

//------------------------------------------------------------------------------
/*! @brief   Disassembly process.
 *
 *  @note    The program text is a binary code. The disassembler goes through the code
 *           and translating commands with their operands to the asm file. Also it
 *           writes section of code on the left like comment, for each command.
 *           If an error is found, the process stops and the code section with the error
 *           is output.
 *
 *  @return  error code
 */

    int Disassemble ();

//------------------------------------------------------------------------------
/*! @brief   Write disassembled program text to the file.
 *  @param   filename    File name for the program binary
 *
 *  @return  error code
 */

    int Write (char* filename);

/*------------------------------------------------------------------------------
                   Private functions                                           *
*///----------------------------------------------------------------------------

private:

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

    int writeOperands (Text* text, char cmd_code, char reg_code, INT_TYPE num_int, FLT_TYPE num_flt, ptr_t num_ptr, size_t lab_num, size_t line, size_t startpos, size_t endpos);

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
/*! @brief   Prints a section of code with an error to the console and to the log file.
 *
 *  @param   logname     Name of the log file
 */

    void PrintCode (const char* logname);

//------------------------------------------------------------------------------
};

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