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
    DSM_NOT_CONSTRUCTED                                                ,
    DSM_NULL_INPUT_DISASSEMBLER_PTR                                    ,
    DSM_UNIDENTIFIED_COMMAND                                           ,
};

static const char* asm_errstr[] =
{
    "ERROR"                                                            ,
    "OK"                                                               ,
    "Failed to allocate memory"                                        ,

    "Disassembler already constructed"                                 ,
    "Disassembler already destructed"                                  ,
    "Disassembler did not constructed, operation is impossible"        ,
    "The input value of the disassembler pointer turned out to be zero",
    "Unidentified command"                                             ,
};

#define DSM_ASSERTOK(cond, err, printcode, p_dsm) if (cond)                                                                     \
                                                  {                                                                             \
                                                    DsmPrintError(disassembler_logname, __FILE__, __LINE__, __FUNCTION__, err); \
                                                    if (printcode) DsmPrintCode(p_dsm, disassembler_logname, err);              \
                                                    exit(err); /**/                                                             \
                                                  }


//==============================================================================
/*------------------------------------------------------------------------------
                   Disassembler constants and types                            *
*///----------------------------------------------------------------------------
//==============================================================================


static const char* disassembler_logname = "disassembler.log";
static const char* CODE_TYPE            = "_dis.asm";

const size_t DEFAULT_LINES_NUM = 32;
const size_t MAX_CHARS_IN_LINE = 64;

static const char  COMMENT   = ';';

typedef struct disassembler
{
    int state = DSM_NOT_CONSTRUCTED;

    text_t   output = {};
    bcode_t  bcode  = {};
} dsm_t;


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

int DsmConstruct (dsm_t* p_dsm, const char* filename);

//------------------------------------------------------------------------------
/*! @brief   Disassmebler destructor.
 *
 *  @param   p_dsm       Pointer to the disassembler
 *
 *  @return  error code
 */

int DsmDestruct (dsm_t* p_dsm);

//------------------------------------------------------------------------------
/*! @brief   Disassembly process.
 *
 *  @note    """""""
 *
 *  @param   p_dsm       Pointer to the disassembler
 *
 *  @return  error code
 */

int Disassemble (dsm_t* p_dsm);

//------------------------------------------------------------------------------
/*! @brief   Write disassembled program text to the file.
 *
 *  @param   p_dsm       Pointer to the disassembler
 *  @param   filename    File name for the program binary
 *
 *  @return  error code
 */

int DsmWrite (dsm_t* p_dsm, char* filename);

//------------------------------------------------------------------------------
/*! @brief   Prints a section of code with an error to the console and to the log file.
 * 
 *  @param   p_dsm       Pointer to the disassembler
 *  @param   logname     Name of the log file
 *  @param   err         Error code
 */

void DsmPrintCode (dsm_t* p_dsm, const char* logname, int err);

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