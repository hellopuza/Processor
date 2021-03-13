/*------------------------------------------------------------------------------
    * File:        StringLib.h                                                 *
    * Description: String functions library                                    *
    * Created:     6 nov 2020                                                  *
    * Copyright:   (C) 2020 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#ifndef STRINGLIB_H_INCLUDED
#define STRINGLIB_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
//#define NDEBUG


#include <sys\stat.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>


//==============================================================================
/*------------------------------------------------------------------------------
                   StringLib errors                                            *
*///----------------------------------------------------------------------------
//==============================================================================


enum StringErrors
{
    STR_NOT_OK = -1                                                    ,
    STR_OK = 0                                                         ,
    STR_NO_MEMORY                                                      ,

    STR_NO_LINES                                                       ,
    STR_NO_SYMB                                                        ,
};

static const char* str_errstr[] =
{
    "ERROR"                                                            ,
    "OK"                                                               ,
    "Failed to allocate memory"                                        ,

    "There are no lines with letters in text!"                         ,
    "The file has no any symbols!"                                     ,
};

static const char* string_logname = "string.log";

#define STR_ASSERTOK(cond, err)  if (cond)                                                                 \
                                 {                                                                         \
                                     StrPrintError(string_logname, __FILE__, __LINE__, __FUNCTION__, err); \
                                     return err; /**/                                                      \
                                 }


//==============================================================================
/*------------------------------------------------------------------------------
                   StringLib constants and types                               *
*///----------------------------------------------------------------------------
//==============================================================================


#ifndef PTR_T
#define PTR_T
typedef size_t ptr_t;

#include <limits.h>
#define PTR_MAX UINT_MAX
#endif // PTR_T

typedef struct line
{
    char*  str = nullptr;
    size_t len = 0;
} line_t;

typedef struct text_info
{
   char*   text        = nullptr;
   size_t  size        = 0;
   
   size_t  num         = 0;
   line_t* lines       = nullptr;
} text_t;

typedef struct bin_code
{
    char*  data = nullptr;
    size_t size = 0;
    ptr_t  ptr  = 0;
} bcode_t;


//==============================================================================
/*------------------------------------------------------------------------------
                   StringLib implementations                                   *
*///----------------------------------------------------------------------------
//==============================================================================

//------------------------------------------------------------------------------
/*! @brief   Text structure constructor.
 *
 *  @param   txtstruct   Pointer to the text structure
 *  @param   filename    Name of the text file
 *
 *  @return  error code
 */

int TextConstruct (text_t* txtstruct, const char* filename);

//------------------------------------------------------------------------------
/*! @brief   Fill the structure of text
 *
 *  @param   txtstruct   Pointer to the text structure
 *  @param   fp          Pointer to the text file
 *
 *  @return  error code
 */

int fillinTextStruct (text_t* txtstruct, FILE* fp);

//------------------------------------------------------------------------------
/*! @brief   Text structure destructor.
 *
 *  @param   txtstruct   Pointer to the text structure
 *
 *  @return  error code
 */

int TextDestruct (text_t* txtstruct);

//------------------------------------------------------------------------------
/*! @brief   Binary code structure constructor.
 *
 *  @param   p_bcode     Pointer to the binary code structure
 *  @param   size        Size of the data
 *
 *  @return  error code
 */

int BCodeConstruct (bcode_t* p_bcode, size_t size);

//------------------------------------------------------------------------------
/*! @brief   Fill the binary code structure.
 *
 *  @param   p_bcode     Pointer to the binary code structure
 *  @param   filename    Name of the input file
 *
 *  @return  error code
 */

int fillinBCodeStruct (bcode_t* p_bcode, const char* filename);

//------------------------------------------------------------------------------
/*! @brief   Increase the binary code data size by 2 times.
 * 
 *  @param   p_bcode     Pointer to the binary code structure
 * 
 *  @return  error code
 */

int BCodeExpand (bcode_t* p_bcode);

//------------------------------------------------------------------------------
/*! @brief   Binary code structure destructor.
 *
 *  @param   p_bcode     Pointer to the binary code structure
 *
 *  @return  error code
 */

int BCodeDestruct (bcode_t* p_bcode);

//------------------------------------------------------------------------------
/*! @brief   Get name of a file from command line.
 *
 *  @param   argc        Number of command line arguments
 *  @param   argv        Arguments array
 *
 *  @return  name of the file, else argv[0]
 */

char* GetFileName (int argc, char** argv);

//------------------------------------------------------------------------------
/*! @brief   Get true name of a file (without path to the file and type).
 *
 *  @param   filename    name of the file
 *
 *  @return  true name of the file
 */

char* GetTrueFileName (char* filename);

//------------------------------------------------------------------------------
/*! @brief   Get a size of the file.
 *
 *  @param   fp          Pointer to the file
 *
 *  @return  size of file
 */

size_t CountSize (FILE* fp);

//------------------------------------------------------------------------------
/*! @brief   Get text of the file.
 *
 *  @param   fp          Pointer to the file
 *  @param   len         Length of the text
 *
 *  @return  pointer to text
 */

char* GetText (FILE* fp, size_t len);

//------------------------------------------------------------------------------
/*! @brief   Get number of lines in the text.
 *
 *  @param   text        C string contains text
 *  @param   len         Length of the text
 *
 *  @return  number of lines in the text
 */

size_t GetLineNum (char* text, size_t len);

//------------------------------------------------------------------------------
/*! @brief   Get pointers to start of lines and their lengths.
 *
 *  @param   text        C string contains text
 *  @param   num         Number of lines
 *
 *  @return  array of lines
 */

line_t* GetLine (char* text, size_t num);

//------------------------------------------------------------------------------
/*! @brief   Get number of words in string.
 *
 *  @param   line        Pointer to the line structure
 *
 *  @return  number of words
 */

size_t GetWordNum(line_t line);

//------------------------------------------------------------------------------
/*! @brief   Counting characters in string.
 *
 *  @param   str         C string
 *  @param   c           Character to be counted
 *
 *  @return  number of words
 */

size_t chrcnt (char* str, char c);

//------------------------------------------------------------------------------
/*! @brief   Compare two lines from left alphabetically.
 *
 *  @param   p1          Pointer to the first line
 *  @param   p2          Pointer to the second line
 *
 *  @return  positive integer if first line bigger then second
 *  @return  0 if first line the same as second
 *  @return  negative integer if first line smaller then second
 */

int CompareFromLeft (const void *p1, const void *p2);

//------------------------------------------------------------------------------
/*! @brief   Compare two lines from right alphabetically.
 *
 *  @param   p1          Pointer to the first line
 *  @param   p2          Pointer to the second line
 *
 *  @return  positive integer if first line bigger then second
 *  @return  0 if first line the same as second
 *  @return  negative integer if first line smaller then second
 */

int CompareFromRight (const void *p1, const void *p2);

//------------------------------------------------------------------------------
/*! @brief   Copmare two strings by letters.
 *
 *  @param   line1       First line
 *  @param   line2       Second line
 *  @param   dir         Direction of comparing (+1 - compare from left, -1 - compare from right)
 *
 *  @return  positive integer if first line bigger then second
 *  @return  0 if first line the same as second
 *  @return  negative integer if first line smaller then second
 */

int StrCompare (line_t line1, line_t line2, int dir);

//------------------------------------------------------------------------------
/*! @brief   Write lines to the file.
 *
 *  @param   lines       Array of lines
 *  @param   num         Number of lines
 *  @param   filename    Name of the file
 */

void Write (line_t* Lines, size_t num, const char* filename);

//------------------------------------------------------------------------------
/*! @brief   Write lines to the file.
 *
 *  @param   text        C string
 *  @param   num         Number of lines
 *  @param   filename    Name of the file
 */

void Print (char* text, size_t len, const char* filename);

//------------------------------------------------------------------------------
/*! @brief   Check that char is letter.
 *
 *  @param   c           Character to be checked
 *
 *  @return  1 if c is letter
 *  @return  0 if c is not letter
 */

int isAlpha (const unsigned char c);

//------------------------------------------------------------------------------
/*! @brief   Prints an error wih description to the console and to the log file.
 * 
 *  @param   logname     Name of the log file
 *  @param   file        Name of the program file
 *  @param   line        Number of line with an error
 *  @param   function    Name of the function with an error
 *  @param   err         Error code
 */

void StrPrintError (const char* logname, const char* file, int line, const char* function, int err);

//------------------------------------------------------------------------------

#endif // STRINGLIB_H_INCLUDED
