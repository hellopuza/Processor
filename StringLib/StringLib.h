/*------------------------------------------------------------------------------
    * File:        StringLib.h                                                 *
    * Description: String functions library                                    *
    * Created:     6 nov 2020                                                  *
    * Copyright:   (C) 2020 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
*///----------------------------------------------------------------------------

#ifndef STRINGLIB_H_INCLUDED
#define STRINGLIB_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
//#define NDEBUG


#include "../Errors.h"
#include <sys\stat.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>


#ifndef PTR_T
#define PTR_T
typedef size_t ptr_t;

#include <limits.h>
#define PTR_MAX UINT_MAX
#endif // PTR_T


static const char* string_logname = "string.log";

#define STR_ASSERTOK(cond, err)  if (cond)                                                              \
                                 {                                                                      \
                                     printError(string_logname, __FILE__, __LINE__, __FUNCTION__, err); \
                                     return err; /**/                                                   \
                                 }


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

   size_t  sloc        = 0;
   line_t* sclines     = nullptr;

   size_t* line_numbers = nullptr;
} text_t;

typedef struct byte_code
{
    char*  data = nullptr;
    size_t size = 0;
    ptr_t  ptr  = 0;
} bcode_t;


//------------------------------------------------------------------------------
/*! @brief   Construct the structure of text
 *
 *  @param   txtstruct is the structure to be constructed
 *  @param   filename is input file name
 *
 *  @return  error code
 */

int TextConstruct (text_t* txtstruct, const char* filename);

//------------------------------------------------------------------------------
/*! @brief   Fill the structure of text
 *
 *  @param   txtstruct is the structure to be filled
 *  @param   fp is pointer to file
 *
 *  @return  error code
 */

int fillinTextStruct (text_t* txtstruct, FILE* fp);

//------------------------------------------------------------------------------
/*! @brief   Clean the structure of text
 *
 *  @param   txtstruct is the structure to be destructed
 *
 *  @return  error code
 */

int TextDestruct (text_t* txtstruct);

//------------------------------------------------------------------------------
/*! @brief   Construct the structure of byte code
 *
 *  @param   p_bcode is the structure to be constructed
 *  @param   size is size of data
 *
 *  @return  error code
 */

int BCodeConstruct (bcode_t* p_bcode, size_t size);

//------------------------------------------------------------------------------
/*! @brief   Fill the structure of byte code
 *
 *  @param   p_bcode is the structure to be filled
 *  @param   filename is input file name
 *
 *  @return  error code
 */

int fillinBCodeStruct (bcode_t* p_bcode, const char* filename);

int BCodeExpand (bcode_t* p_bcode);
//------------------------------------------------------------------------------
/*! @brief   Clean the structure of byte code
 *
 *  @param   p_bcode is the structure to be destructed
 *
 *  @return  error code
 */

int BCodeDestruct (bcode_t* p_bcode);

//------------------------------------------------------------------------------
/*! @brief   Get name of file from command line
 *
 *  @param   argc is number of command line arguments
 *  @param   argv is arguments
 *
 *  @return  name of file
 */

char* GetFileName (int argc, char** argv);

//------------------------------------------------------------------------------
/*! @brief   Get size of file
 *
 *  @param   fp is pointer to file
 *
 *  @return  size of file
 */

size_t CountSize (FILE* fp);

//------------------------------------------------------------------------------
/*! @brief   Get text of file
 *
 *  @param   fp is pointer to file
 *  @param   len is length of text
 *
 *  @return  pointer to text
 */

char* GetText (FILE* fp, size_t len);

//------------------------------------------------------------------------------
/*! @brief   Get number of lines in text
 *
 *  @param   text contains text
 *  @param   len  length of text
 *
 *  @return  number of lines in text
 */

size_t GetLineNum (char* text, size_t len);

size_t GetSLOC (char* text, size_t len);

//------------------------------------------------------------------------------
/*! @brief   Get pointers to start of lines and their lengths
 *
 *  @param   text contains text
 *  @param   num is number of strings
 *
 *  @return  array of lines
 */

line_t** GetLine (char* text, size_t num, size_t sloc, size_t** line_numbers);

//------------------------------------------------------------------------------
/*! @brief   Get number of words in string
 *
 *  @param   line is pointer to struct line
 *
 *  @return  number of words
 */

size_t GetWordNum(line_t line);

//------------------------------------------------------------------------------
/*! @brief   Counting characters in string
 *
 *  @param   str is pointer to string
 *  @param   c Character to be located
 *
 *  @return  number of words
 */

size_t chrcnt (char* str, char c);

//------------------------------------------------------------------------------
/*! @brief   Compare two lines from left alphabetically
 *
 *  @param   p1 is pointer to first line
 *  @param   p2 is pointer to second line
 *
 *  @return  positive integer if first line bigger then second
 *  @return  0 if first line the same as second
 *  @return  negative integer if first line smaller then second
 */

int CompareFromLeft (const void *p1, const void *p2);

//------------------------------------------------------------------------------
/*! @brief   Compare two lines from right alphabetically
 *
 *  @param   p1 is pointer to first line
 *  @param   p2 is pointer to second line
 *
 *  @return  positive integer if first line bigger then second
 *  @return  0 if first line the same as second
 *  @return  negative integer if first line smaller then second
 */

int CompareFromRight (const void *p1, const void *p2);

//------------------------------------------------------------------------------
/*! @brief   Copmare two strings by letters
 *
 *  @param   line1 is first line
 *  @param   line2 is second line
 *  @param   dir is direction of comparing (+1 - compare from left, -1 - compare from right)
 *
 *  @return  positive integer if first line bigger then second
 *  @return  0 if first line the same as second
 *  @return  negative integer if first line smaller then second
 */

int StrCompare (line_t line1, line_t line2, int dir);

//------------------------------------------------------------------------------
/*! @brief   Write lines to file
 *
 *  @param   lines is array of lines
 *  @param   num is number of lines
 *  @param   filename is name of file
 */

void Write (line_t* Lines, size_t num, const char* filename);

//------------------------------------------------------------------------------
/*! @brief   Print text to file
 *
 *  @param   text contains text
 *  @param   len is length of text
 *  @param   filename is name of file
 */

void Print (char* text, size_t len, const char* filename);

//------------------------------------------------------------------------------
/*! @brief   Check that char is letter
 *
 *  @param   c is char
 *
 *  @return  1 if c is letter
 *  @return  0 if c is not letter
 */

int isAlpha (const unsigned char c);

//------------------------------------------------------------------------------

#endif // STRINGLIB_H_INCLUDED
