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

#ifdef HASH_PROTECT
#include "hash.h"
#endif // HASH_PROTECT


#define STACK_CHECK if (Check (__FUNCTION__))                                                                     \
                    {                                                                                             \
                      FILE* log = fopen(stack_logname, "a");                                                      \
                      assert (log != nullptr);                                                                    \
                      fprintf(log, "ERROR: file %s  line %d  function %s\n\n", __FILE__, __LINE__, __FUNCTION__); \
                      printf (     "ERROR: file %s  line %d  function %s\n",   __FILE__, __LINE__, __FUNCTION__); \
                      fclose(log);                                                                                \
                      Dump( __FUNCTION__, stack_logname);                                                         \
                      exit(errCode_); /**/                                                                        \
                    }


#define STACK_ASSERTOK(cond, err) if (cond)                                                             \
                                  {                                                                     \
                                    printError (stack_logname , __FILE__, __LINE__, __FUNCTION__, err); \
                                    exit(err); /**/                                                     \
                                  }

const size_t DATA_SIZE = 20;

static int   stack_id   = 0;
static char* stack_name = nullptr;

#define StackConstruct(NAME, capacity, STK_TYPE) \
        Stack<STK_TYPE> NAME (capacity, (char*)#NAME);


template <typename TYPE>
class Stack
{
private:

#ifdef CANARY_PROTECT
    can_t canary1_ = 0;
#endif //CANARY_PROTECT

    size_t  capacity_ = 0;
    size_t  size_cur_ = 0;
    const char* name_ = nullptr;

    TYPE data_ [DATA_SIZE];

    int errCode_;
    int id_ = 0;

#ifdef HASH_PROTECT
    hash_t stackhash_ = 0;
    hash_t datahash_  = 0;
#endif // HASH_PROTECT

#ifdef CANARY_PROTECT
    can_t canary2_ = 0;
#endif //CANARY_PROTECT

public:

//------------------------------------------------------------------------------
/*! @brief   Stack constructor.
 */

    Stack ();

//------------------------------------------------------------------------------
/*! @brief   Stack constructor.
 *
 *  @param   capacity    Capacity of the stack
 *  @param   stack_name  Stack variable name
 */

    Stack (size_t capacity, char* stack_name);

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
/*! @brief   Print the contents of the stack and its data to the logfile.
 *
 *  @param   funcname    Name of the function from which the StackDump was called
 *  @param   logname     Name of the logfile
 *
 *  @return  error code
 */

    int Dump (const char* funcname = "@some function@", const char* logfile = stack_logname);

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
/*! @brief   Calculates the size of the structure stack without hash and second canary.
 *
 *  @return  stack size for hash
 */

    size_t SizeForHash ();

//------------------------------------------------------------------------------
/*! @brief   Check stack for problems, canaries, hash (if enabled).
 *
 *  @param   funcname    Name of the function from which the StackCheck was called
 *
 *  @return  error code
 */

    int Check (const char* funcname);

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
/*! @brief   Change canary (if enabled).
 *
 *  @param   canary      Canary input
 *
 *  @return  changed canary
 */

#ifdef CANARY_PROTECT

    can_t CanaryChange (can_t canary);

#endif // CANARY_PROTECT

//------------------------------------------------------------------------------
/*! @brief   Placing canaries in stack (if enabled).
 */

#ifdef CANARY_PROTECT

    void CanaryPlacing ();

#endif // CANARY_PROTECT

//------------------------------------------------------------------------------
/*! @brief   Check stack for canaries (if enabled).
 *
 *  @return  OK if all canaries aren't damaged, otherwise NOT_OK
 */

#ifdef CANARY_PROTECT

    int CanaryCheck ();

#endif // CANARY_PROTECT

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
