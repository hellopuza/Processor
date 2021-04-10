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
Stack<TYPE>::Stack (char* stack_name, size_t capacity) :
    data_     (),
    size_cur_ (0),
    capacity_ (capacity),
    name_     (stack_name),
    id_       (stack_id++),
    errCode_  (STACK_OK)
{
    STACK_ASSERTOK((capacity > MAX_CAPACITY),   STACK_WRONG_INPUT_CAPACITY_VALUE_BIG);
    STACK_ASSERTOK((capacity == 0),             STACK_WRONG_INPUT_CAPACITY_VALUE_NIL);
    STACK_ASSERTOK((stack_id == MAX_STACK_NUM), STACK_TOO_MANY_STACKS);

    try
    {
        data_ = new TYPE[capacity_];
    }
    catch (std::bad_alloc& err)
    {
        STACK_ASSERTOK(STACK_NO_MEMORY, STACK_NO_MEMORY);
    }

    fillPoison();

    STACK_CHECK;

    DUMP_PRINT{ Dump(__FUNC_NAME__); }
}

//------------------------------------------------------------------------------

template <typename TYPE>
Stack<TYPE>::Stack (const Stack& obj) :
    data_     (nullptr),
    size_cur_ (obj.size_cur_),
    capacity_ (obj.capacity_),
    name_     (obj.name_),
    id_       (stack_id++),
    errCode_  (STACK_OK)
{
    STACK_ASSERTOK((capacity_ > MAX_CAPACITY),   STACK_WRONG_INPUT_CAPACITY_VALUE_BIG);
    STACK_ASSERTOK((capacity_ == 0),             STACK_WRONG_INPUT_CAPACITY_VALUE_NIL);
    STACK_ASSERTOK((stack_id == MAX_STACK_NUM),  STACK_TOO_MANY_STACKS);

    try
    {
        data_ = new TYPE[capacity_];
    }
    catch (std::bad_alloc& err)
    {
        STACK_ASSERTOK(STACK_NO_MEMORY, STACK_NO_MEMORY);
    }

    for (int i = 0; i < capacity_; ++i) data_[i] = obj.data_[i];

    STACK_CHECK;

    DUMP_PRINT{ Dump(__FUNC_NAME__); }
}

//------------------------------------------------------------------------------

template <typename TYPE>
Stack<TYPE>::~Stack ()
{
    DUMP_PRINT{ Dump (__FUNC_NAME__); }

    if ((errCode_ != STACK_DESTRUCTED) && (errCode_ != STACK_NOT_CONSTRUCTED))
    {
        size_cur_ = 0;

        fillPoison();

        delete [] data_;

        capacity_ = 0;

        data_  = nullptr;
        errCode_ = STACK_DESTRUCTED;
    }
}

//------------------------------------------------------------------------------

template <typename TYPE>
int Stack<TYPE>::Push (TYPE value)
{
    STACK_CHECK;

    if (size_cur_ == capacity_ - 1) Expand();

    data_[size_cur_++] = value;

    STACK_CHECK;

    DUMP_PRINT{ Dump (__FUNC_NAME__); }

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
        DUMP_PRINT{ Dump (__FUNC_NAME__); }

        return POISON<TYPE>;
    }

    TYPE value = data_[--size_cur_];

    data_[size_cur_] = POISON<TYPE>;

    DUMP_PRINT{ Dump (__FUNC_NAME__); }

    return value;
}

//------------------------------------------------------------------------------

template <typename TYPE>
size_t Stack<TYPE>::getSize() const
{
    return size_cur_;
}

//------------------------------------------------------------------------------

template <typename TYPE>
TYPE& Stack<TYPE>::operator [] (size_t n)
{
    STACK_ASSERTOK((n >= capacity_), STACK_MEM_ACCESS_VIOLATION);

    return data_[n];
}

//------------------------------------------------------------------------------

template <typename TYPE>
const TYPE& Stack<TYPE>::operator [] (size_t n) const
{
    STACK_ASSERTOK((n >= capacity_), STACK_MEM_ACCESS_VIOLATION);
    
    return data_[n];
}
//------------------------------------------------------------------------------

template <typename TYPE>
Stack<TYPE>& Stack<TYPE>::operator = (const Stack& obj)
{
    Stack<TYPE> ret(obj);

    return ret;
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
    if (value == POISON<TYPE>) return 1;

    if (isnan(*(double*)&POISON<TYPE>))
        if (isnan(*(double*)&value))
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

    TYPE* temp = nullptr;
    try
    {
        temp = new TYPE[capacity_];
    }
    catch (std::bad_alloc& err)
    {
        STACK_ASSERTOK(STACK_NO_MEMORY, STACK_NO_MEMORY);
    }

    memcpy(temp, (char*)data_, capacity_ * sizeof(TYPE) / 2);

    delete [] data_;
    data_ = temp;

    fillPoison();

    return STACK_OK;
}

//------------------------------------------------------------------------------

template <typename TYPE>
int Stack<TYPE>::Dump (const char* funcname, const char* logfile)
{
    const size_t linelen = 80;
    char divline[linelen + 1] = "********************************************************************************";

    FILE* fp = stdout;
    if (funcname != nullptr)
    {
        fp = fopen(logfile, "a");
        if (fp == nullptr)
            return STACK_NOT_OK;

        if (funcname != nullptr)
            fprintf(fp, "This dump was called from a function \"%s\"\n", funcname);

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        fprintf(fp, "TIME: %d-%02d-%02d %02d:%02d:%02d\n\n",
                tm.tm_year + 1900,
                tm.tm_mon + 1,
                tm.tm_mday,
                tm.tm_hour,
                tm.tm_min,
                tm.tm_sec);
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
        if (fp != stdout) fclose(fp);

        return STACK_OK;
    }

    char* StkState = (char*)stk_errstr[STACK_OK + 1];

   if (errCode_) ErrorPrint(fp);

    fprintf(fp, "\nStack (%s) [0x%p] \"%s\", id (%d)\n", StkState, this, name_, id_);
    fprintf(fp, "\t{\n");

    fprintf(fp, "\tType of data is %s\n\n", PRINT_TYPE<TYPE>);

    fprintf(fp, "\tCapacity           = %u\n",   capacity_);
    fprintf(fp, "\tCurrent size       = %u\n\n", size_cur_);

    fprintf(fp, "\tData [0x%p]\n", data_);
    fprintf(fp, "\t\t{\n");

    for (int i = 0; i < capacity_; i++)
    {
        char ispois = isPOISON(data_[i]);

        fprintf(fp, "\t\t%s[%d]: ", (ispois) ? " ": "*", i);
        fprintf(fp, PRINT_FORMAT<TYPE>, data_[i]);
        fprintf(fp, "%s\n", (ispois) ? " (POISON)": "");
    }

    fprintf(fp, "\t\t}\n");

    fprintf(fp, "\t}\n");

    fprintf(fp, "%s\n", divline);
    if (fp != stdout) fclose(fp);

    return STACK_OK;
}

//------------------------------------------------------------------------------

template <typename TYPE>
int Stack<TYPE>::Check ()
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

    else
    {
        errCode_ = STACK_OK;
        return STACK_OK;
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

        if (fp != stdout) fprintf(fp, "\n%s\n", stk_errstr[errCode_ + 1]);
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

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(log, "TIME: %d-%02d-%02d %02d:%02d:%02d\n\n",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec);

    fprintf(log, "ERROR: file %s  line %d  function %s\n\n", file, line, function);
    fprintf(log, "%s\n", stk_errstr[err + 1]);

    printf("ERROR: file %s  line %d  function %s\n", file, line, function);
    printf("%s\n\n", stk_errstr[err + 1]);

    fclose(log);
}

//------------------------------------------------------------------------------
