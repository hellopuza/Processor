/*------------------------------------------------------------------------------
    * File:        StringLib.cpp                                               *
    * Description: Implementations of string functions                         *
    * Created:     6 nov 2020                                                  *
    * Copyright:   (C) 2020 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#include "StringLib.h"

//------------------------------------------------------------------------------

int TextConstruct(text_t* txtstruct, const char* filename)
{
    assert(txtstruct != nullptr);
    assert(filename  != nullptr);

    FILE* fp = nullptr;
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("\n ERROR. Input file \"%s\" is not found\n", filename);

        return STR_NOT_OK;
    }

    int err = fillinTextStruct(txtstruct, fp);
    STR_ASSERTOK(err, err);

    fclose(fp);

    return STR_OK;
}

//------------------------------------------------------------------------------

int fillinTextStruct(text_t* txtstruct, FILE* fp)
{
    assert(txtstruct != nullptr);
    assert(fp != nullptr);

    txtstruct->size = CountSize(fp);
    if (txtstruct->size == 0)
        return STR_NO_SYMB;

    txtstruct->text = GetText(fp, txtstruct->size);
    if (txtstruct->text == nullptr)
        return STR_NO_MEMORY;

    txtstruct->num = GetLineNum(txtstruct->text, txtstruct->size);
    if (txtstruct->num == 0)
        return STR_NO_LINES;

    txtstruct->lines = GetLine(txtstruct->text, txtstruct->num);
    if (txtstruct->lines == nullptr)
        return STR_NO_MEMORY;

    return STR_OK;
}

//------------------------------------------------------------------------------

int TextDestruct(text_t* txtstruct)
{
    assert(txtstruct != nullptr);
    assert(txtstruct->lines != nullptr);
    assert(txtstruct->text  != nullptr);

    if (txtstruct->num != 0)
    {
        free(txtstruct->lines);
        txtstruct->lines = nullptr;
        txtstruct->num   = 0;
    }

    if (txtstruct->size != 0)
    {
        free(txtstruct->text);
        txtstruct->text = nullptr;
        txtstruct->size = 0;
    }

    return STR_OK;
}

//------------------------------------------------------------------------------

int BCodeConstruct(bcode_t* p_bcode, size_t size)
{
    assert(p_bcode != nullptr);
    assert(size);

    p_bcode->data = (char*)calloc(size + 2, 1);
    STR_ASSERTOK((p_bcode->data == nullptr) , STR_NO_MEMORY);

    p_bcode->ptr = 0;
    p_bcode->size = size;

    return STR_OK;
}

//------------------------------------------------------------------------------

int fillinBCodeStruct(bcode_t* p_bcode, const char* filename)
{
    assert(p_bcode  != nullptr);
    assert(filename != nullptr);

    FILE* fp = nullptr;
    if ((fp = fopen(filename, "rb")) == NULL)
    {
        printf("\n ERROR. Input file \"%s\" is not found\n", filename);

        return STR_NOT_OK;
    }

    p_bcode->size = CountSize(fp);
    STR_ASSERTOK((p_bcode->size == 0) , STR_NO_MEMORY);

    p_bcode->data = GetText(fp, p_bcode->size);
    STR_ASSERTOK((p_bcode->data == nullptr) , STR_NO_MEMORY);

    fclose(fp);

    p_bcode->ptr = 0;

    return STR_OK;
}

//------------------------------------------------------------------------------

int BCodeExpand(bcode_t* p_bcode)
{
    assert(p_bcode != nullptr);

    p_bcode->size*= 2;

    void* temp = calloc(p_bcode->size + 2, 1);
    if (temp == nullptr)
        return STR_NO_MEMORY;

    void* oldtemp = p_bcode->data;
    memcpy(temp, p_bcode->data, p_bcode->size / 2);
    free(oldtemp);

    p_bcode->data = (char*)temp;

    return STR_OK;
}

//------------------------------------------------------------------------------

int BCodeDestruct(bcode_t* p_bcode)
{
    assert(p_bcode != nullptr);

    if (p_bcode->size != 0)
    {
        free(p_bcode->data);
        p_bcode->ptr  = 0;
        p_bcode->size = 0;
    }

    return STR_OK;
}

//------------------------------------------------------------------------------

char* GetFileName(int argc, char** argv)
{
    assert(argc);
    assert(argv != nullptr);

    if (argc > 1)
    {
        return argv[1];
    }

    return argv[0];
}

//------------------------------------------------------------------------------

char* GetTrueFileName(char* filename)
{
    assert(filename != nullptr);

    int ptr_end = strlen(filename) - 1;

    for (int i = 0; i < strlen(filename); ++i)
    {
        if (filename[ptr_end - i] == '.')
            filename[ptr_end - i] = '\0';
        else
        if ((filename[ptr_end - i] == '/') ||
            (filename[ptr_end - i] == '\\'))
            return filename + ptr_end - i + 1;
    }

    return filename;
}

//------------------------------------------------------------------------------

size_t CountSize(FILE* fp)
{
    assert(fp != nullptr);

    struct stat prop;
    fstat(_fileno(fp), &prop);

    return prop.st_size;
}

//------------------------------------------------------------------------------

char* GetText(FILE* fp, size_t len)
{
    assert(fp != nullptr);
    assert(len);

    char* text = (char*)calloc(len + 2, 1);
    if (text == nullptr)
        return nullptr;

    fread(text, 1, len, fp);

    return text;
}

//------------------------------------------------------------------------------

size_t GetLineNum(char* text, size_t len)
{
    assert(text != nullptr);
    assert(len);

    char* start = text;

    size_t num = 0;

    while (text - start <= len)
    {
        ++num;

        text = strchr(text, '\n') + 1;
        if ((int)text == 1)
            break;
    }

    return num;
}

//------------------------------------------------------------------------------

line_t* GetLine(char* text, size_t num)
{
    assert(text != nullptr);
    assert(num);

    line_t* Lines = (line_t*)calloc(num + 2, sizeof(line_t));
    if (Lines == nullptr)
        return nullptr;

    line_t* temp1 = Lines;

    while (num-- > 0)
    {
        while (isspace(*text) && (*text != '\n'))
            ++text;

        char* start = text;
        text = strchr(text, '\n');

        if (text != 0) *text = '\0';

        temp1->str = (char*)start;
        temp1->len = strlen(start);

        ++temp1;
        ++text;
    }

    return Lines;
}

//------------------------------------------------------------------------------

size_t GetWordNum(line_t line)
{
    assert(line.str != nullptr);

    int num = 0;
    char f  = 0;
    for (int i = 0; i <= line.len; ++i)
    {
        char c = *(line.str + i);

        if (isgraph(c))
            f = 1;
        else
            if ((f == 1) && (isspace(c) || (c == '\0')))
            {
                f = 0;
                ++num;
            }
    }

    return num;
}

//------------------------------------------------------------------------------

size_t chrcnt(char* str, char c)
{
    assert(str != nullptr);

    size_t count = 0;

    str = strchr(str, c);
    while (str != NULL)
    {
        ++count;
        str = strchr(str + 1, c);
        if ((int)str == 0)
            break;
    }

    return count;
}

//------------------------------------------------------------------------------

int CompareFromLeft(const void* p1, const void* p2)
{
    assert(p1 != nullptr);
    assert(p2 != nullptr);
    assert(p1 != p2);

    return StrCompare(*(line_t*)p1, *(line_t*)p2, 1);
}

//------------------------------------------------------------------------------

int CompareFromRight(const void* p1, const void* p2)
{
    assert(p1 != nullptr);
    assert(p2 != nullptr);
    assert(p1 != p2);

    return StrCompare(*(line_t*)p1, *(line_t*)p2, -1);
}

//------------------------------------------------------------------------------

int StrCompare(line_t line1, line_t line2, int dir)
{
    assert((dir == 1) || (dir == -1));

    int i1 = 0;
    int i2 = 0;

    if (dir == -1)
    {
        i1 = line1.len - 1;
        i2 = line2.len - 1;
    }

    while ((line1.str[i1] != '\0') && (line2.str[i2] != '\0'))
    {
        if (not(isAlpha(line1.str[i1])))
        {
            i1 += dir;
            continue;
        }

        if (not(isAlpha(line2.str[i2])))
        {
            i2 += dir;
            continue;
        }

        if ((unsigned char)line1.str[i1] == (unsigned char)line2.str[i2])
        {
            i1 += dir;
            i2 += dir;
            continue;
        }

        else return ((unsigned char)line1.str[i1] - (unsigned char)line2.str[i2]);
    }

    if (dir == 1)
        return ((unsigned char)line1.str[i1] - (unsigned char)line2.str[i2]);
    else
        return ((unsigned char)line1.str[i2] - (unsigned char)line2.str[i1]);
}

//------------------------------------------------------------------------------

int isAlpha(const unsigned char c)
{
    return (   ((unsigned char)'a' <= c) && (c <= (unsigned char)'z')
            || ((unsigned char)'A' <= c) && (c <= (unsigned char)'Z')
            || ((unsigned char)'à' <= c) && (c <= (unsigned char)'ÿ')
            || ((unsigned char)'À' <= c) && (c <= (unsigned char)'ß'));
}

//------------------------------------------------------------------------------

void Write(line_t* lines, size_t num, const char* filename)
{
    assert(lines != nullptr);
    assert(num);
    assert(filename);

    FILE* fp = fopen(filename, "w");

    for(int i = 0; i < num; ++i)
        fprintf(fp, "%s\n", lines[i].str);

    fclose(fp);
}

//------------------------------------------------------------------------------

void Print(char* text, size_t len, const char* filename)
{
    assert(text != nullptr);
    assert(len);
    assert(filename);

    FILE* fp = fopen(filename, "w");

    for (int i = 0; i < len; ++i)
        fputc(text[i], fp);

    fclose(fp);
}

//------------------------------------------------------------------------------

void StrPrintError(const char* logname, const char* file, int line, const char* function, int err)
{
    assert(function != nullptr);
    assert(logname != nullptr);
    assert(file != nullptr);

    FILE* log = fopen(logname, "a");
    assert(log != nullptr);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    fprintf(log, "###############################################################################\n");
    fprintf(log, "TIME: %d-%02d-%02d %02d:%02d:%02d\n\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fprintf(log, "ERROR: file %s  line %d  function %s\n\n", file, line, function);
    fprintf(log, "%s\n", str_errstr[err + 1]);

    printf (     "ERROR: file %s  line %d  function %s\n",   file, line, function);
    printf (     "%s\n\n", str_errstr[err + 1]);

    fclose(log);
}

//------------------------------------------------------------------------------
