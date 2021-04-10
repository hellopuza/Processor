/*------------------------------------------------------------------------------
    * File:        Stack.h                                                     *
    * Description: Stack library.                                              *
    * Created:     1 dec 2020                                                  *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS
//#define NDEBUG


#if defined(_WIN32)
    #include <windows.h>
#endif

#include "StackConfig.h"
#include <assert.h>
#include <limits.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <new>


#if defined (__GNUC__) || defined (__clang__) || defined (__clang_major__)
    #define __FUNC_NAME__   __PRETTY_FUNCTION__

#elif defined (_MSC_VER)
    #define __FUNC_NAME__   __FUNCSIG__

#else
    #define __FUNC_NAME__   __FUNCTION__

#endif


#define STACK_CHECK if (Check ())                                                                                      \
                    {                                                                                                  \
                      FILE* log = fopen(stack_logname, "a");                                                           \
                      assert (log != nullptr);                                                                         \
                      fprintf(log, "ERROR: file %s  line %d  function \"%s\"\n\n", __FILE__, __LINE__, __FUNC_NAME__); \
                      printf (     "ERROR: file %s  line %d  function \"%s\"\n",   __FILE__, __LINE__, __FUNC_NAME__); \
                      fclose(log);                                                                                     \
                      Dump( __FUNC_NAME__, stack_logname);                                                             \
                      exit(errCode_); /**/                                                                             \
                    }


#define STACK_ASSERTOK(cond, err) if (cond)                                                              \
                                  {                                                                      \
                                    printError (stack_logname , __FILE__, __LINE__, __FUNC_NAME__, err); \
                                    exit(err); /**/                                                      \
                                  }

const size_t DEFAULT_STACK_CAPACITY = 8;
static int   stack_id   = 0;
static char* stack_name = nullptr;

#define newStack_size(NAME, capacity, STK_TYPE) \
        Stack<STK_TYPE> NAME ((char*)#NAME, capacity);

#define newStack(NAME, STK_TYPE) \
        Stack<STK_TYPE> NAME ((char*)#NAME);


template <typename TYPE>
class Stack
{
private:

    const char* name_ = nullptr;
    size_t  capacity_ = 0;
    size_t  size_cur_ = 0;

    TYPE* data_ = nullptr;

    int errCode_;
    int id_ = 0;

public:

//------------------------------------------------------------------------------
/*! @brief   Stack default constructor.
 */

    Stack ();

//------------------------------------------------------------------------------
/*! @brief   Stack constructor.
 *
 *  @param   stack_name  Stack variable name
 *  @param   capacity    Capacity of the stack
 */

    Stack (char* stack_name, size_t capacity = DEFAULT_STACK_CAPACITY);

//------------------------------------------------------------------------------
/*! @brief   Stack copy constructor.
 *
 *  @param   obj         Source stack
 */

    Stack (const Stack& obj);

//------------------------------------------------------------------------------
/*! @brief   Stack destructor.
 */

   ~Stack ();

//------------------------------------------------------------------------------
/*! @brief   Pushing a value onto the stack.
 *
 *  @param   value       Value to push
 *
 *  @return  error code
 */

    int Push (TYPE value);

//------------------------------------------------------------------------------
/*! @brief   Popping from stack.
 *
 *  @return  value from the stack if present, otherwise POISON
 */

    TYPE Pop ();

//------------------------------------------------------------------------------
/*! @brief   Get size of the stack data.
 *
 *  @return  stack data size
 */

    size_t getSize() const;

    TYPE& operator [] (size_t n);

    const TYPE& operator [] (size_t n) const;

    Stack& operator = (const Stack& obj);

//------------------------------------------------------------------------------
/*! @brief   Print the contents of the stack and its data to the logfile.
 *
 *  @param   funcname    Name of the function from which the StackDump was called
 *  @param   logname     Name of the logfile
 *
 *  @return  error code
 */

    int Dump (const char* funcname = nullptr, const char* logfile = stack_logname);

/*------------------------------------------------------------------------------
                   Private functions                                           *
*///----------------------------------------------------------------------------

private:

//------------------------------------------------------------------------------
/*! @brief   Filling the stack data with POISON.
 */

    void fillPoison ();

 //------------------------------------------------------------------------------
 /*! @brief   Increase the stack by 2 times.
 *
 *  @return  error code
 */

    int Expand ();

//------------------------------------------------------------------------------
/*! @brief   Check stack for problems, canaries, hash (if enabled).
 *
 *  @return  error code
 */

    int Check ();

//------------------------------------------------------------------------------
/*! @brief   Print information and error summary to log file and to console.
 *
 *  @param   fp          Pointer to the logfile
 *
 *  @return  error code
 */

    void ErrorPrint (FILE * fp);

 //------------------------------------------------------------------------------
/*! @brief   Print error explanations to log file and to console.
 *
 *  @param   logname     Name of the log file
 *  @param   file        Name of the file from which this function was called
 *  @param   line        Line of the code from which this function was called
 *  @param   function    Name of the function from which this function was called
 *
 *  @return  error code
 */

    void printError (const char* logname, const char* file, int line, const char* function, int err);

//------------------------------------------------------------------------------
};

//------------------------------------------------------------------------------
/*! @brief   Check if value is POISON.
 *
 *  @param   value       Value to be checked
 *
 *  @return 1 if value is POISON, else 0
 */

template <typename TYPE>
int isPOISON (TYPE value);

//------------------------------------------------------------------------------

#include "Stack.ipp"

#endif // STACK_H_INCLUDED
