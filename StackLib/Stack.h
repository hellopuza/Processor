/*------------------------------------------------------------------------------
    * File:        Stack.h                                                     *
    * Description: Stack library. You have to define stack TYPE before         *
                   including this file.                                        *
    * Created:     1 dec 2020                                                  *
    * Copyright:   (C) 2020 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#define _CRT_SECURE_NO_WARNINGS
//#define NDEBUG

#ifndef TYPE
    #error Stack.h: You have to define stack TYPE before including this file.
#else


#if defined(_WIN32)
    #include <windows.h>
#endif

#include "StackConfig.h"
#include "Template.h"
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


#define STACK_CHECK(p_stk) if (TEMPLATE(StackCheck, TYPE) (p_stk, __FUNCTION__))                                         \
                           {                                                                                             \
                             FILE* log = fopen(stack_logname, "a");                                                      \
                             assert (log != nullptr);                                                                    \
                             fprintf(log, "ERROR: file %s  line %d  function %s\n\n", __FILE__, __LINE__, __FUNCTION__); \
                             printf (     "ERROR: file %s  line %d  function %s\n",   __FILE__, __LINE__, __FUNCTION__); \
                             fclose(log);                                                                                \
                             TEMPLATE(StackDump, TYPE) (p_stk, __FUNCTION__, stack_logname);                             \
                             exit(p_stk->errCode); /**/                                                                  \
                           }


#define STACK_ASSERTOK(cond, err) if (cond)                                                                             \
                                  {                                                                                     \
                                    TEMPLATE(printError, TYPE) (stack_logname , __FILE__, __LINE__, __FUNCTION__, err); \
                                    exit(err); /**/                                                                     \
                                  }


#ifndef STACK_ID
    static int stack_id = 0;
    #define STACK_ID
#endif // STACK_ID


#ifndef STACK_NAME
    static char* stack_name = nullptr;
    #define STACK_NAME
#endif // STACK_NAME


#define StackConstruct(NAME, capacity, STK_TYPE)                              \
        TEMPLATE(stack, STK_TYPE) NAME;                                       \
        TEMPLATE(_StackConstruct, STK_TYPE) (&##NAME, capacity, (char*)#NAME);


typedef struct TEMPLATE(Stack, TYPE)
{
///////////////TRY-TO-HACK///////////////
#ifdef CANARY_PROTECT
    can_t canary1;
#endif //CANARY_PROTECT

    size_t  capacity;
    size_t  size_cur;
    const char* name;

    TYPE* data;

    int errCode = STACK_NOT_CONSTRUCTED;
    int id;

#ifdef HASH_PROTECT
    hash_t stackhash;
    hash_t datahash;
#endif // HASH_PROTECT

#ifdef CANARY_PROTECT
    can_t canary2;
#endif //CANARY_PROTECT
///////////////TRY-TO-HACK///////////////
} TEMPLATE(stack, TYPE);


//==============================================================================
/*------------------------------------------------------------------------------
                   StackLib definitions                                        *
*///----------------------------------------------------------------------------
//==============================================================================

//------------------------------------------------------------------------------
/*! @brief   Stack constructor.
 *
 *  @param   p_stk       Pointer to the stack
 *  @param   capacity    Capacity of the stack
 *  @param   stack_name  Stack variable name
 *
 *  @return  error code
 */

static int TEMPLATE(_StackConstruct, TYPE) (TEMPLATE(stack, TYPE)* p_stk, size_t capacity, char* stack_name);

//------------------------------------------------------------------------------
/*! @brief   Stack destructor.
 *
 *  @param   p_stk       Pointer to the stack
 *
 *  @return  error code
 */

static int TEMPLATE(StackDestruct, TYPE) (TEMPLATE(stack, TYPE)* p_stk);

//------------------------------------------------------------------------------
/*! @brief   Pushing a value onto the stack.
 *
 *  @param   p_stk       Pointer to the stack
 *  @param   value       Value to push
 *
 *  @return  error code
 */

static int TEMPLATE(StackPush, TYPE) (TEMPLATE(stack, TYPE)* p_stk, TYPE value);

//------------------------------------------------------------------------------
/*! @brief   Popping from stack.
 *
 *  @param   p_stk       Pointer to the stack
 *
 *  @return  value from the stack if present, otherwise POISON
 */

static TYPE TEMPLATE(StackPop, TYPE) (TEMPLATE(stack, TYPE)* p_stk);

//------------------------------------------------------------------------------
/*! @brief   Filling the stack data with POISON.
 *
 *  @param   p_stk       Pointer to the stack
 */

static void TEMPLATE(StackPoison, TYPE) (TEMPLATE(stack, TYPE)* p_stk);

//------------------------------------------------------------------------------
/*! @brief   Check if value is POISON.
 *
 *  @param   value       Value to be checked
 *
 *  @return 1 if value is POISON, else 0
 */

static int TEMPLATE(isPOISON, TYPE) (TYPE value);

//------------------------------------------------------------------------------
/*! @brief   Increase the stack by 2 times.
 *
 *  @param   p_stk       Pointer to the stack
 *
 *  @return  error code
 */

static int TEMPLATE(StackExpand, TYPE) (TEMPLATE(stack, TYPE)* p_stk);

//------------------------------------------------------------------------------
/*! @brief   Calculates the size of the structure stack without hash and second canary.
 *
 *  @param   p_stk       Pointer to the stack
 *
 *  @return  stack size for hash
 */

static size_t TEMPLATE(StackSizeForHash, TYPE) (TEMPLATE(stack, TYPE)* p_stk);

//------------------------------------------------------------------------------
/*! @brief   (THE BEST FUNCTION) Print the contents of the stack and its data to the logfile.
 *
 *  @param   p_stk       Pointer to the stack
 *  @param   funcname    Name of the function from which the StackDump was called
 *  @param   logname     Name of the logfile
 *
 *  @return  error code
 */

static int TEMPLATE(StackDump, TYPE) (TEMPLATE(stack, TYPE)* p_stk, const char* funcname = "@some function@", const char* logfile = stack_logname);

//------------------------------------------------------------------------------
/*! @brief   Check stack for problems, canaries, hash (if enabled).
 *
 *  @param   p_stk       Pointer to the stack
 *  @param   funcname    Name of the function from which the StackCheck was called
 *
 *  @return  error code
 */

static int TEMPLATE(StackCheck, TYPE) (TEMPLATE(stack, TYPE)* p_stk, const char* funcname);

//------------------------------------------------------------------------------
/*! @brief   Print information and error summary to log file and to console.
 *
 *  @param   p_stk       Pointer to the stack
 *  @param   fp          Pointer to the logfile
 *
 *  @return  error code
 */

static void TEMPLATE(ErrorPrint, TYPE) (TEMPLATE(stack, TYPE)* p_stk, FILE* fp);

//------------------------------------------------------------------------------
/*! @brief   Print error explanations to log file and to console.
 *
 *  @param   logname     Name of the log file
 *  @param   file        Name of the file from which the printError was called
 *  @param   line        Line of the code from which the printError was called
 *  @param   function    Name of the function from which the StackCheck was called
 *
 *  @return  error code
 */

static void TEMPLATE(printError, TYPE) (const char* logname, const char* file, int line, const char* function, int err);

//------------------------------------------------------------------------------
/*! @brief   Change canary (if enabled).
 *
 *  @param   canary      Canary input
 *
 *  @return  changed canary
 */

#ifdef CANARY_PROTECT

static can_t TEMPLATE(CanaryChange, TYPE) (can_t canary);

#endif // CANARY_PROTECT

//------------------------------------------------------------------------------
/*! @brief   Placing canaries in stack (if enabled).
 *
 *  @param   p_stk       Pointer to the stack
 */

#ifdef CANARY_PROTECT

static void TEMPLATE(CanaryPlacing, TYPE) (TEMPLATE(stack, TYPE)* p_stk);

#endif // CANARY_PROTECT

//------------------------------------------------------------------------------
/*! @brief   Check stack for canaries (if enabled).
 *
 *  @param   p_stk       Pointer to the stack
 *  @param   funcname    Name of the function from which the StackCheck was called
 *
 *  @return  OK if all canaries aren't damaged, otherwise NOT_OK
 */

#ifdef CANARY_PROTECT

static int TEMPLATE(CanaryCheck, TYPE) (TEMPLATE(stack, TYPE)* p_stk);

#endif // CANARY_PROTECT


//==============================================================================
/*------------------------------------------------------------------------------
                   StackLib implementations                                    *
*///----------------------------------------------------------------------------
//==============================================================================


static int TEMPLATE(_StackConstruct, TYPE) (TEMPLATE(stack, TYPE)* p_stk, size_t capacity, char* stack_name)
{
    STACK_ASSERTOK((p_stk == nullptr),                                                                  STACK_NULL_INPUT_STACK_PTR);
    STACK_ASSERTOK(((p_stk->errCode != STACK_NOT_CONSTRUCTED) && (p_stk->errCode != STACK_DESTRUCTED)), STACK_CONSTRUCTED);
    STACK_ASSERTOK((capacity > MAX_CAPACITY),                                                           STACK_WRONG_INPUT_CAPACITY_VALUE_BIG);
    STACK_ASSERTOK((capacity == 0),                                                                     STACK_WRONG_INPUT_CAPACITY_VALUE_NIL);
    STACK_ASSERTOK((stack_id == MAX_STACK_NUM),                                                         STACK_TOO_MANY_STACKS);

    void* temp = calloc(capacity * sizeof(TYPE) + 2 * sizeof(can_t), sizeof(char));
    STACK_ASSERTOK((temp == nullptr), STACK_NO_MEMORY);

    p_stk->data = (TYPE*)((char*)temp + sizeof(can_t));

    p_stk->size_cur = 0;
    p_stk->capacity = capacity;
    p_stk->name = (const char*)stack_name;

    TEMPLATE(StackPoison, TYPE) (p_stk);

    p_stk->errCode = STACK_OK;

    p_stk->id = stack_id;
    stack_id++;


#ifdef CANARY_PROTECT
    perfect_canary = TEMPLATE(CanaryChange, TYPE) (perfect_canary);
    canaries[p_stk->id] = perfect_canary;
    TEMPLATE(CanaryPlacing, TYPE) (p_stk);
#endif // CANARY_PROTECT

#ifdef HASH_PROTECT
    p_stk->datahash  = hash(p_stk->data, p_stk->capacity * sizeof(TYPE));
    p_stk->stackhash = hash(p_stk, TEMPLATE(StackSizeForHash, TYPE) (p_stk));
#endif // HASH_PROTECT

    STACK_CHECK(p_stk);

    DUMP_PRINT{ TEMPLATE(StackDump, TYPE) (p_stk, __FUNCTION__); }

    return STACK_OK;
}

//------------------------------------------------------------------------------

static int TEMPLATE(StackDestruct, TYPE) (TEMPLATE(stack, TYPE)* p_stk)
{
    STACK_CHECK(p_stk);

    p_stk->size_cur = 0;

    TEMPLATE(StackPoison, TYPE) (p_stk);

    free((char*)p_stk->data - sizeof(can_t));

    p_stk->capacity = 0;

#ifdef CANARY_PROTECT
    p_stk->canary1 = 0;
    p_stk->canary2 = 0;
#endif // CANARY_PROTECT

#ifdef HASH_PROTECT
    p_stk->datahash  = 0;
    p_stk->stackhash = 0;
#endif // HASH_PROTECT

    p_stk->data    = nullptr;
    p_stk->errCode = STACK_DESTRUCTED;

    return STACK_OK;
}

//------------------------------------------------------------------------------

static int TEMPLATE(StackPush, TYPE) (TEMPLATE(stack, TYPE)* p_stk, TYPE value)
{
    STACK_CHECK(p_stk);

    if (p_stk->size_cur == p_stk->capacity - 1)
    {
        if (TEMPLATE(StackExpand, TYPE) (p_stk) == STACK_NO_MEMORY)
        {
            p_stk->errCode = STACK_NO_MEMORY;

            DUMP_PRINT{ TEMPLATE(StackDump, TYPE) (p_stk, __FUNCTION__); }

#ifdef HASH_PROTECT
            p_stk->datahash  = hash(p_stk->data, p_stk->capacity * sizeof(TYPE));
            p_stk->stackhash = hash(p_stk, TEMPLATE(StackSizeForHash, TYPE) (p_stk));
#endif // HASH_PROTECT

            exit(STACK_NO_MEMORY);
        }
    }

    p_stk->data[p_stk->size_cur++] = value;


#ifdef CANARY_PROTECT
    canaries[p_stk->id] = TEMPLATE(CanaryChange, TYPE) (canaries[p_stk->id]);
    TEMPLATE(CanaryPlacing, TYPE) (p_stk);
#endif // CANARY_PROTECT

#ifdef HASH_PROTECT
    p_stk->datahash  = hash(p_stk->data, p_stk->capacity * sizeof(TYPE));
    p_stk->stackhash = hash(p_stk, TEMPLATE(StackSizeForHash, TYPE) (p_stk));
#endif // HASH_PROTECT

    STACK_CHECK(p_stk);

    DUMP_PRINT{ TEMPLATE(StackDump, TYPE) (p_stk, __FUNCTION__); }

    return STACK_OK;
}

//------------------------------------------------------------------------------

static TYPE TEMPLATE(StackPop, TYPE) (TEMPLATE(stack, TYPE)* p_stk)
{
    STACK_CHECK(p_stk);

    if (p_stk->errCode == STACK_EMPTY_STACK)
    {
        DUMP_PRINT{ TEMPLATE(StackDump, TYPE) (p_stk, __FUNCTION__); }

#ifdef HASH_PROTECT
        p_stk->datahash  = hash(p_stk->data, p_stk->capacity * sizeof(TYPE));
        p_stk->stackhash = hash(p_stk, TEMPLATE(StackSizeForHash, TYPE) (p_stk));
#endif // HASH_PROTECT

        return TEMPLATE(TYPE, POISON);
    }

    TYPE value = p_stk->data[--p_stk->size_cur];

    p_stk->data[p_stk->size_cur] = TEMPLATE(TYPE, POISON);


#ifdef CANARY_PROTECT
    canaries[p_stk->id] = TEMPLATE(CanaryChange, TYPE) (canaries[p_stk->id]);
    TEMPLATE(CanaryPlacing, TYPE) (p_stk);
#endif // CANARY_PROTECT

#ifdef HASH_PROTECT
    p_stk->datahash  = hash(p_stk->data, p_stk->capacity * sizeof(TYPE));
    p_stk->stackhash = hash(p_stk, TEMPLATE(StackSizeForHash, TYPE) (p_stk));
#endif // HASH_PROTECT

    DUMP_PRINT{ TEMPLATE(StackDump, TYPE) (p_stk, __FUNCTION__); }

    return value;
}

//------------------------------------------------------------------------------

static void TEMPLATE(StackPoison, TYPE) (TEMPLATE(stack, TYPE)* p_stk)
{
    assert(p_stk != nullptr);
    assert(p_stk->data != nullptr);
    assert(p_stk->size_cur < p_stk->capacity);

    for (int i = p_stk->size_cur; i < p_stk->capacity; ++i)
    {
        p_stk->data[i] = TEMPLATE(TYPE, POISON);
    }
}

//------------------------------------------------------------------------------

static int TEMPLATE(isPOISON, TYPE) (TYPE value)
{
    if (isnan((double)TEMPLATE(TYPE, POISON)))
        if (isnan((double)value))
            return 1;
        else
            return 0;
    return (value == TEMPLATE(TYPE, POISON));
}

//------------------------------------------------------------------------------

static int TEMPLATE(StackExpand, TYPE) (TEMPLATE(stack, TYPE)* p_stk)
{
    assert(p_stk != nullptr);

    p_stk->capacity *= 2;

    void* temp = calloc(p_stk->capacity * sizeof(TYPE) + 2 * sizeof(can_t), sizeof(char));
    if (temp == nullptr)
        return STACK_NO_MEMORY;

    void* oldtemp = (char*)p_stk->data - sizeof(can_t);
    memcpy(temp, (char*)p_stk->data - sizeof(can_t), p_stk->capacity * sizeof(TYPE) / 2 + 2 * sizeof(can_t));
    free(oldtemp);

    p_stk->data = (TYPE*)((char*)temp + sizeof(can_t));

    TEMPLATE(StackPoison, TYPE) (p_stk);

    return STACK_OK;
}

//------------------------------------------------------------------------------

static size_t TEMPLATE(StackSizeForHash, TYPE) (TEMPLATE(stack, TYPE)* p_stk)
{
    assert(p_stk != nullptr);

    size_t size = 0;

#ifdef CANARY_PROTECT
    size += sizeof(p_stk->canary1);
#endif //CANARY_PROTECT

    size += sizeof(p_stk->capacity);
    size += sizeof(p_stk->size_cur);
    size += sizeof(p_stk->name);
    size += sizeof(p_stk->data);
    size += sizeof(p_stk->errCode);
    size += sizeof(p_stk->id);

    return size;
}

//------------------------------------------------------------------------------

static int TEMPLATE(StackDump, TYPE) (TEMPLATE(stack, TYPE)* p_stk, const char* funcname, const char* logfile)
{
    const size_t linelen = 80;
    char divline[linelen + 1] = "********************************************************************************";

    FILE* fp = fopen(logfile, "a");
    if (fp == nullptr)
        return STACK_NOT_OK;

    if (funcname != nullptr)
        fprintf(fp, "This dump was called from a function %s\n", funcname);

#if defined(_WIN32)
    SYSTEMTIME localtime = {};
    GetLocalTime(&localtime);

    fprintf(fp, "TIME: %d-%02d-%02d %02d:%02d:%02d.%03d\n",
        localtime.wYear,
        localtime.wMonth,
        localtime.wDay,
        localtime.wHour,
        localtime.wMinute,
        localtime.wSecond,
        localtime.wMilliseconds);
#else
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(log, "TIME: %d-%02d-%02d %02d:%02d:%02d\n\n",
        tm.tm_year + 1900,
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec);
#endif

    if (p_stk == nullptr)
    {
        fprintf(fp, "\nStack (ERROR) [0x%p] \"unidentified stack\"\n", p_stk);
        fprintf(fp, "%s\n", stk_errstr[STACK_NULL_STACK_PTR + 1]);

        fprintf(fp, "%s\n", divline);
        fclose(fp);

        return STACK_OK;
    }

    if ( (p_stk->errCode == STACK_NOT_CONSTRUCTED)      ||
         (p_stk->errCode == STACK_DESTRUCTED)           ||
         (p_stk->errCode == STACK_NULL_DATA_PTR)        ||
         (p_stk->errCode == STACK_SIZE_BIGGER_CAPACITY) ||
         (p_stk->errCode == STACK_CAPACITY_WRONG_VALUE)   )
    {
        fprintf(fp, "\nStack (ERROR) [0x%p] \"%s\" id (%d)\n", p_stk, p_stk->name, p_stk->id);
        TEMPLATE(ErrorPrint, TYPE) (p_stk, fp);

        fprintf(fp, "%s\n", divline);
        fclose(fp);

        return STACK_OK;
    }

    char* StkState = (char*)stk_errstr[STACK_OK + 1];

    if (p_stk->errCode)
    {
        if ((p_stk->errCode == STACK_CANARY_DIED) || (p_stk->errCode == STACK_INCORRECT_HASH))
            StkState = (char*)stk_errstr[STACK_NOT_OK + 1];

        TEMPLATE(ErrorPrint, TYPE) (p_stk, fp);
    }

    fprintf(fp, "\nStack (%s) [0x%p] \"%s\", id (%d)\n", StkState, p_stk, p_stk->name, p_stk->id);
        fprintf(fp, "\t{\n");

        fprintf(fp, "\tType of data is %s\n\n", TEMPLATE(TYPE, PRINT_TYPE));

        fprintf(fp, "\tCapacity           = %u\n",   p_stk->capacity);
        fprintf(fp, "\tCurrent size       = %u\n\n", p_stk->size_cur);

#ifdef CANARY_PROTECT
        fprintf(fp, "\tCanary stack 1     = " CAN_PRINT_FORMAT "\n",   p_stk->canary1);
        fprintf(fp, "\tCanary stack 2     = " CAN_PRINT_FORMAT "\n\n", p_stk->canary2);
#endif // CANARY_PROTECT

#ifdef HASH_PROTECT
        fprintf(fp, "\tStack hash         = " HASH_PRINT_FORMAT "\n",   p_stk->stackhash);
        fprintf(fp, "\tData hash          = " HASH_PRINT_FORMAT "\n\n", p_stk->datahash);

        if ((p_stk->errCode != STACK_OK) && (p_stk->errCode != STACK_EMPTY_STACK) && (p_stk->errCode != STACK_NO_MEMORY))
        {
            fprintf(fp, "\tTrue stack hash    = " HASH_PRINT_FORMAT "\n",   hash(p_stk, TEMPLATE(StackSizeForHash, TYPE) (p_stk)));
            fprintf(fp, "\tTrue data hash     = " HASH_PRINT_FORMAT "\n\n", hash(p_stk->data, p_stk->capacity * sizeof(TYPE)));
        }
#endif // HASH_PROTECT

        fprintf(fp, "\tData [0x%p]\n", p_stk->data);
            fprintf(fp, "\t\t{\n");

#ifdef CANARY_PROTECT
            fprintf(fp, "\t\tCanary data 1: " CAN_PRINT_FORMAT "\n", ((can_t*)p_stk->data)[-1]);
#endif // CANARY_PROTECT

            for (int i = 0; i < p_stk->capacity; i++)
            {
                if (!(TEMPLATE(isPOISON, TYPE) (p_stk->data[i])))
                    fprintf(fp, "\t\t*[%d]: " TEMPLATE(TYPE, PRINT_FORMAT) "\n", i, p_stk->data[i]);
                else
                    fprintf(fp, "\t\t [%d]: " TEMPLATE(TYPE, PRINT_FORMAT) " (POISON)\n", i, p_stk->data[i]);
            }

#ifdef CANARY_PROTECT
            fprintf(fp, "\t\tCanary data 2: " CAN_PRINT_FORMAT "\n", ((can_t*)p_stk->data)[p_stk->capacity * sizeof(TYPE) / sizeof(can_t)]);
#endif // CANARY_PROTECT

            fprintf(fp, "\t\t}\n");

        fprintf(fp, "\t}\n");

    fprintf(fp, "%s\n", divline);
    fclose(fp);

    return STACK_OK;
}

//------------------------------------------------------------------------------

static int TEMPLATE(StackCheck, TYPE) (TEMPLATE(stack, TYPE)* p_stk, const char* funcname)
{
    if (p_stk == nullptr)
    {
        return STACK_NULL_STACK_PTR;
    }

    if (p_stk->errCode == STACK_NOT_CONSTRUCTED)
    {
        p_stk->name = "unidentified stack";
        p_stk->id = -666;
        return STACK_NOT_CONSTRUCTED;
    }

    if (p_stk->errCode == STACK_DESTRUCTED)
    {
        return STACK_DESTRUCTED;
    }

#ifdef HASH_PROTECT
    if (p_stk->stackhash != hash(p_stk, TEMPLATE(StackSizeForHash, TYPE) (p_stk)))
    {
        p_stk->errCode = STACK_INCORRECT_HASH;
        return STACK_INCORRECT_HASH;
    }
#endif // HASH_PROTECT

    if (p_stk->data == nullptr)
    {
        p_stk->errCode = STACK_NULL_DATA_PTR;
        return STACK_NULL_DATA_PTR;
    }

    if (p_stk->size_cur > p_stk->capacity)
    {
        p_stk->errCode = STACK_SIZE_BIGGER_CAPACITY;
        return STACK_SIZE_BIGGER_CAPACITY;
    }

    if ((p_stk->size_cur == 0) && (funcname == "StackPop_" TEMPLATE(TYPE, PRINT_TYPE)))
    {
        p_stk->errCode = STACK_EMPTY_STACK;

        return STACK_OK;
    }

    if ((p_stk->capacity == 0) || (p_stk->capacity > MAX_CAPACITY))
    {
        p_stk->errCode = STACK_CAPACITY_WRONG_VALUE;
        return STACK_CAPACITY_WRONG_VALUE;
    }

    if (!(TEMPLATE(isPOISON, TYPE) (p_stk->data[p_stk->size_cur])))
    {
        p_stk->errCode = STACK_WRONG_CUR_SIZE;
        return STACK_WRONG_CUR_SIZE;
    }

#ifdef CANARY_PROTECT
    if (TEMPLATE(CanaryCheck, TYPE) (p_stk))
    {
        p_stk->errCode = STACK_CANARY_DIED;
        return STACK_CANARY_DIED;
    }
#endif // CANARY_PROTECT

#ifdef HASH_PROTECT
    if (p_stk->datahash != hash(p_stk->data, p_stk->capacity * sizeof(TYPE)))
    {
        p_stk->errCode = STACK_INCORRECT_HASH;
        return STACK_INCORRECT_HASH;
    }
#endif // HASH_PROTECT

    else
    {
        p_stk->errCode = STACK_OK;
        return STACK_OK;
    }
}

//------------------------------------------------------------------------------

static void TEMPLATE(ErrorPrint, TYPE) (TEMPLATE(stack, TYPE)* p_stk, FILE* fp)
{
    assert(fp != nullptr);

    if (p_stk == nullptr)
    {
        CONSOLE_PRINT{ printf("%s\n", stk_errstr[STACK_NULL_STACK_PTR + 1]); }
    }

    else if (p_stk->errCode != STACK_OK)
    {
        CONSOLE_PRINT{ printf("%s\n", stk_errstr[p_stk->errCode + 1]); }

        fprintf(fp, "\n%s\n", stk_errstr[p_stk->errCode + 1]);
    }
}

//------------------------------------------------------------------------------

static void TEMPLATE(printError, TYPE) (const char* logname, const char* file, int line, const char* function, int err)
{
    assert(function != nullptr);
    assert(logname != nullptr);
    assert(file != nullptr);

    FILE* log = fopen(logname, "a");
    assert(log != nullptr);

    fprintf(log, "********************************************************************************\n");

#if defined(_WIN32)
    SYSTEMTIME localtime = {};
    GetLocalTime(&localtime);

    fprintf(log, "TIME: %d-%02d-%02d %02d:%02d:%02d.%03d\n\n",
        localtime.wYear,
        localtime.wMonth,
        localtime.wDay,
        localtime.wHour,
        localtime.wMinute,
        localtime.wSecond,
        localtime.wMilliseconds);
#else
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(log, "TIME: %d-%02d-%02d %02d:%02d:%02d\n\n",
        tm.tm_year + 1900,
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec);
#endif

    fprintf(log, "ERROR: file %s  line %d  function %s\n\n", file, line, function);
    fprintf(log, "%s\n", stk_errstr[err + 1]);

    printf (     "ERROR: file %s  line %d  function %s\n",   file, line, function);
    printf (     "%s\n\n", stk_errstr[err + 1]);

    fclose(log);
}

//------------------------------------------------------------------------------

static can_t TEMPLATE(CanaryChange, TYPE) (can_t canary)
{
    return (canary * 2 + rand() * ((can_t)rand() % 3 - 1)) % ULLONG_MAX;
}

//------------------------------------------------------------------------------

#ifdef CANARY_PROTECT

static void TEMPLATE(CanaryPlacing, TYPE) (TEMPLATE(stack, TYPE)* p_stk)
{
    assert(p_stk != nullptr);

    p_stk->canary1 = canaries[p_stk->id];
    p_stk->canary2 = canaries[p_stk->id];

    ((can_t*)p_stk->data)[-1] = canaries[p_stk->id];
    ((can_t*)p_stk->data)[p_stk->capacity * sizeof(TYPE) / sizeof(can_t)] = canaries[p_stk->id];
}

#endif //CANARY_PROTECT

//------------------------------------------------------------------------------

#ifdef CANARY_PROTECT

static int TEMPLATE(CanaryCheck, TYPE) (TEMPLATE(stack, TYPE)* p_stk)
{
    if (p_stk->canary1 != canaries[p_stk->id])
        return STACK_NOT_OK;

    if (p_stk->canary2 != canaries[p_stk->id])
        return STACK_NOT_OK;

    if (((can_t*)p_stk->data)[-1] != canaries[p_stk->id])
        return STACK_NOT_OK;

    if (((can_t*)p_stk->data)[p_stk->capacity * sizeof(TYPE) / sizeof(can_t)] != canaries[p_stk->id])
        return STACK_NOT_OK;

    return STACK_OK;
}

#endif //CANARY_PROTECT

//------------------------------------------------------------------------------

#endif // TYPE
