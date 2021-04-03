/*------------------------------------------------------------------------------
    * File:        Stack.ipp                                                   *
    * Description: Implementations of stack functions.                         *
    * Created:     1 dec 2020                                                  *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

template <typename TYPE>
Stack<TYPE>::Stack () : errCode_ (STACK_NOT_CONSTRUCTED) { }

//------------------------------------------------------------------------------

template <typename TYPE>
Stack<TYPE>::Stack (size_t capacity, char* stack_name) :
    data_     (),
    size_cur_ (0),
    //capacity_ (capacity),
    capacity_ (DATA_SIZE),
    name_     (stack_name),
    id_       (stack_id++),
    errCode_  (STACK_OK)
{
    STACK_ASSERTOK((capacity > MAX_CAPACITY),   STACK_WRONG_INPUT_CAPACITY_VALUE_BIG);
    STACK_ASSERTOK((capacity == 0),             STACK_WRONG_INPUT_CAPACITY_VALUE_NIL);
    STACK_ASSERTOK((stack_id == MAX_STACK_NUM), STACK_TOO_MANY_STACKS);

    /*
    void* temp = calloc(capacity_ * sizeof(TYPE) + 2 * sizeof(can_t), sizeof(char));
    STACK_ASSERTOK((temp == nullptr), STACK_NO_MEMORY);

    data_ = (TYPE*)((char*)temp + sizeof(can_t));
    */

    fillPoison();

#ifdef CANARY_PROTECT
    perfect_canary = CanaryChange (perfect_canary);
    canaries[id_]  = perfect_canary;
    CanaryPlacing();
#endif // CANARY_PROTECT

#ifdef HASH_PROTECT
    datahash_  = hash(data_, capacity_ * sizeof(TYPE));
    stackhash_ = hash(this, SizeForHash());
#endif // HASH_PROTECT

    STACK_CHECK;

    DUMP_PRINT{ Dump (__FUNCTION__); }
}

//------------------------------------------------------------------------------

template <typename TYPE>
Stack<TYPE>::~Stack ()
{
    STACK_CHECK;

    size_cur_ = 0;

    fillPoison();

    /*
    free((char*)data_ - sizeof(can_t));
    */

    capacity_ = 0;

#ifdef CANARY_PROTECT
    canary1_ = 0;
    canary2_ = 0;
#endif // CANARY_PROTECT

#ifdef HASH_PROTECT
    datahash_  = 0;
    stackhash_ = 0;
#endif // HASH_PROTECT

    //data_  = nullptr;
    errCode_ = STACK_DESTRUCTED;
}

//------------------------------------------------------------------------------

template <typename TYPE>
int Stack<TYPE>::Push (TYPE value)
{
    STACK_CHECK;

    if (size_cur_ == capacity_ - 1)
    {
        if (Expand() == STACK_NO_MEMORY)
        {
            errCode_ = STACK_NO_MEMORY;

            DUMP_PRINT{ Dump (__FUNCTION__); }

#ifdef HASH_PROTECT
            datahash_  = hash(data_, capacity_ * sizeof(TYPE));
            stackhash_ = hash(this, SizeForHash());
#endif // HASH_PROTECT

            exit(STACK_NO_MEMORY);
        }
    }

    data_[size_cur_++] = value;

#ifdef CANARY_PROTECT
    canaries[id_] = CanaryChange(canaries[id_]);
    CanaryPlacing();
#endif // CANARY_PROTECT

#ifdef HASH_PROTECT
    datahash_  = hash(data_, capacity_ * sizeof(TYPE));
    stackhash_ = hash(this, SizeForHash());
#endif // HASH_PROTECT

    STACK_CHECK;

    DUMP_PRINT{ Dump (__FUNCTION__); }

    return STACK_OK;
}

//------------------------------------------------------------------------------

template <typename TYPE>
TYPE Stack<TYPE>::Pop ()
{
    STACK_CHECK;

    if (size_cur_ == 0) errCode_ = STACK_EMPTY_STACK;

    if (errCode_ == STACK_EMPTY_STACK)
    {
        DUMP_PRINT{ Dump (__FUNCTION__); }

#ifdef HASH_PROTECT
        datahash_  = hash(data_, capacity_ * sizeof(TYPE));
        stackhash_ = hash(this, SizeForHash());
#endif // HASH_PROTECT

        return POISON<TYPE>;
    }

    TYPE value = data_[--size_cur_];

    data_[size_cur_] = POISON<TYPE>;


#ifdef CANARY_PROTECT
    canaries[id_] = CanaryChange(canaries[id_]);
    CanaryPlacing();
#endif // CANARY_PROTECT

#ifdef HASH_PROTECT
    datahash_  = hash(data_, capacity_ * sizeof(TYPE));
    stackhash_ = hash(this, SizeForHash());
#endif // HASH_PROTECT

    DUMP_PRINT{ Dump (__FUNCTION__); }

    return value;
}

//------------------------------------------------------------------------------

template <typename TYPE>
void Stack<TYPE>::fillPoison ()
{
    assert(this     != nullptr);
    assert(data_    != nullptr);
    assert(size_cur_ < capacity_);

    for (int i = size_cur_; i < capacity_; ++i)
    {
        data_[i] = POISON<TYPE>;
    }
}

//------------------------------------------------------------------------------

template <typename TYPE>
int isPOISON (TYPE value)
{
    if (isnan((double)POISON<TYPE>))
        if (isnan((double)value))
            return 1;
        else
            return 0;

    return (value == POISON<TYPE>);
}

//------------------------------------------------------------------------------

template <typename TYPE>
int Stack<TYPE>::Expand ()
{
    assert(this != nullptr);

    capacity_ *= 2;

    void* temp = calloc(capacity_ * sizeof(TYPE) + 2 * sizeof(can_t), sizeof(char));
    if (temp == nullptr)
        return STACK_NO_MEMORY;

    void* oldtemp = (char*)data_ - sizeof(can_t);
    memcpy(temp, (char*)data_ - sizeof(can_t), capacity_ * sizeof(TYPE) / 2 + 2 * sizeof(can_t));
    free(oldtemp);

    /*
    data_ = (TYPE*)((char*)temp + sizeof(can_t));
    */

    fillPoison();

    return STACK_OK;
}

//------------------------------------------------------------------------------

template <typename TYPE>
size_t Stack<TYPE>::SizeForHash ()
{
    assert(this != nullptr);

    size_t size = 0;

#ifdef CANARY_PROTECT
    size += sizeof(canary1_);
#endif //CANARY_PROTECT

    size += sizeof(capacity_);
    size += sizeof(size_cur_);
    size += sizeof(name_);
    size += sizeof(data_);
    size += sizeof(errCode_);
    size += sizeof(id_);

    return size;
}

//------------------------------------------------------------------------------

template <typename TYPE>
int Stack<TYPE>::Dump (const char* funcname, const char* logfile)
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

#endif // _WIN32

    if (this == nullptr)
    {
        fprintf(fp, "\nStack (ERROR) [0x%p] \"unidentified stack\"\n", this);
        fprintf(fp, "%s\n", stk_errstr[STACK_NULL_STACK_PTR + 1]);

        fprintf(fp, "%s\n", divline);
        fclose(fp);

        return STACK_OK;
    }

    if ((errCode_ == STACK_NOT_CONSTRUCTED)      ||
        (errCode_ == STACK_DESTRUCTED)           ||
        (errCode_ == STACK_NULL_DATA_PTR)        ||
        (errCode_ == STACK_SIZE_BIGGER_CAPACITY) ||
        (errCode_ == STACK_CAPACITY_WRONG_VALUE)   )
    {
        fprintf(fp, "\nStack (ERROR) [0x%p] \"%s\" id (%d)\n", this, name_, id_);
        ErrorPrint(fp);

        fprintf(fp, "%s\n", divline);
        fclose(fp);

        return STACK_OK;
    }

    char* StkState = (char*)stk_errstr[STACK_OK + 1];

    if (errCode_)
    {
        if ((errCode_ == STACK_CANARY_DIED) || (errCode_ == STACK_INCORRECT_HASH))
        StkState = (char*)stk_errstr[STACK_NOT_OK + 1];

        ErrorPrint(fp);
    }

    fprintf(fp, "\nStack (%s) [0x%p] \"%s\", id (%d)\n", StkState, this, name_, id_);
    fprintf(fp, "\t{\n");

    fprintf(fp, "\tType of data is %s\n\n", PRINT_TYPE<TYPE>);

    fprintf(fp, "\tCapacity           = %u\n",   capacity_);
    fprintf(fp, "\tCurrent size       = %u\n\n", size_cur_);

#ifdef CANARY_PROTECT
    fprintf(fp, "\tCanary stack 1     = " CAN_PRINT_FORMAT "\n",   canary1_);
    fprintf(fp, "\tCanary stack 2     = " CAN_PRINT_FORMAT "\n\n", canary2_);
#endif // CANARY_PROTECT

#ifdef HASH_PROTECT
    fprintf(fp, "\tStack hash         = " HASH_PRINT_FORMAT "\n",   stackhash_);
    fprintf(fp, "\tData hash          = " HASH_PRINT_FORMAT "\n\n", datahash_);

    if ((errCode_ != STACK_OK) && (errCode_ != STACK_EMPTY_STACK) && (errCode_ != STACK_NO_MEMORY))
    {
        fprintf(fp, "\tTrue stack hash    = " HASH_PRINT_FORMAT "\n",   hash(this, SizeForHash()));
        fprintf(fp, "\tTrue data hash     = " HASH_PRINT_FORMAT "\n\n", hash(data_, capacity_ * sizeof(TYPE)));
    }
#endif // HASH_PROTECT

    fprintf(fp, "\tData [0x%p]\n", data_);
    fprintf(fp, "\t\t{\n");

#ifdef CANARY_PROTECT
            fprintf(fp, "\t\tCanary data 1: " CAN_PRINT_FORMAT "\n", ((can_t*)data_)[-1]);
#endif // CANARY_PROTECT

    for (int i = 0; i < capacity_; i++)
    {
        fprintf(fp, "\t\t*[%d]: ", i);
        fprintf(fp, PRINT_FORMAT<TYPE>, data_[i]);
        fprintf(fp, "%s\n", (! isPOISON(data_[i])) ? " (POISON)": "");
    }

#ifdef CANARY_PROTECT
    fprintf(fp, "\t\tCanary data 2: " CAN_PRINT_FORMAT "\n", ((can_t*)data_)[capacity_ * sizeof(TYPE) / sizeof(can_t)]);
#endif // CANARY_PROTECT

    fprintf(fp, "\t\t}\n");

    fprintf(fp, "\t}\n");

    fprintf(fp, "%s\n", divline);
    fclose(fp);

    return STACK_OK;
}

//------------------------------------------------------------------------------

template <typename TYPE>
int Stack<TYPE>::Check (const char* funcname)
{
    if (this == nullptr)
    {
        return STACK_NULL_STACK_PTR;
    }

    if (errCode_ == STACK_NOT_CONSTRUCTED)
    {
        return STACK_NOT_CONSTRUCTED;
    }

    if (errCode_ == STACK_DESTRUCTED)
    {
        return STACK_DESTRUCTED;
    }

#ifdef HASH_PROTECT
    if (stackhash_ != hash(this, SizeForHash()))
    {
        errCode_ = STACK_INCORRECT_HASH;
        return STACK_INCORRECT_HASH;
    }
#endif // HASH_PROTECT

    if (data_ == nullptr)
    {
        errCode_ = STACK_NULL_DATA_PTR;
        return STACK_NULL_DATA_PTR;
    }

    if (size_cur_ > capacity_)
    {
        errCode_ = STACK_SIZE_BIGGER_CAPACITY;
        return STACK_SIZE_BIGGER_CAPACITY;
    }

    if ((capacity_ == 0) || (capacity_ > MAX_CAPACITY))
    {
        errCode_ = STACK_CAPACITY_WRONG_VALUE;
        return STACK_CAPACITY_WRONG_VALUE;
    }

    if (! isPOISON(data_[size_cur_]))
    {
        errCode_ = STACK_WRONG_CUR_SIZE;
        return STACK_WRONG_CUR_SIZE;
    }

#ifdef CANARY_PROTECT
    if (CanaryCheck())
    {
        errCode_ = STACK_CANARY_DIED;
        return STACK_CANARY_DIED;
    }
#endif // CANARY_PROTECT

#ifdef HASH_PROTECT
    if (datahash_ != hash(data_, capacity_ * sizeof(TYPE)))
    {
        errCode_ = STACK_INCORRECT_HASH;
        return STACK_INCORRECT_HASH;
    }
#endif // HASH_PROTECT

    else
    {
        errCode_ = STACK_OK;
        return errCode_;
    }
}

//------------------------------------------------------------------------------

template <typename TYPE>
void Stack<TYPE>::ErrorPrint (FILE* fp)
{
    assert(fp != nullptr);

    if (this == nullptr)
    {
        CONSOLE_PRINT{ printf("%s\n", stk_errstr[STACK_NULL_STACK_PTR + 1]); }
    }

    else if (errCode_ != STACK_OK)
    {
        CONSOLE_PRINT{ printf("%s\n", stk_errstr[errCode_ + 1]); }

        fprintf(fp, "\n%s\n", stk_errstr[errCode_ + 1]);
    }
}

//------------------------------------------------------------------------------

template <typename TYPE>
void Stack<TYPE>::printError (const char* logname, const char* file, int line, const char* function, int err)
{
    assert(function != nullptr);
    assert(logname  != nullptr);
    assert(file     != nullptr);

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
#endif // _WIN32

    fprintf(log, "ERROR: file %s  line %d  function %s\n\n", file, line, function);
    fprintf(log, "%s\n", stk_errstr[err + 1]);

    printf("ERROR: file %s  line %d  function %s\n", file, line, function);
    printf("%s\n\n", stk_errstr[err + 1]);

    fclose(log);
}

//------------------------------------------------------------------------------

#ifdef CANARY_PROTECT

template <typename TYPE>
can_t Stack<TYPE>::CanaryChange (can_t canary)
{
    return (canary * 2 + rand() * ((can_t)rand() % 3 - 1)) % ULLONG_MAX;
}

#endif // CANARY_PROTECT

//------------------------------------------------------------------------------

#ifdef CANARY_PROTECT

template <typename TYPE>
void Stack<TYPE>::CanaryPlacing ()
{
    assert(this != nullptr);

    canary1_ = canaries[id_];
    canary2_ = canaries[id_];


    /*
    ((can_t*)data_)[-1] = canaries[id_];
    ((can_t*)data_)[capacity_ * sizeof(TYPE) / sizeof(can_t)] = canaries[id_];
    */
}

#endif //CANARY_PROTECT

//------------------------------------------------------------------------------

#ifdef CANARY_PROTECT

template <typename TYPE>
int Stack<TYPE>::CanaryCheck ()
{
    if (canary1_ != canaries[id_])
        return STACK_NOT_OK;

    if (canary2_ != canaries[id_])
        return STACK_NOT_OK;

    /*
    if (((can_t*)data_)[-1] != canaries[id_])
        return STACK_NOT_OK;

    if (((can_t*)data_)[capacity_ * sizeof(TYPE) / sizeof(can_t)] != canaries[id_])
        return STACK_NOT_OK;
    */

    return STACK_OK;
}

#endif //CANARY_PROTECT

//------------------------------------------------------------------------------
