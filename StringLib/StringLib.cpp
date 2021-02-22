/*------------------------------------------------------------------------------
    * File:        StringLib.cpp                                               *
    * Description: Implementations of string functions                         *
    * Created:     6 nov 2020                                                  *
    * Copyright:   (C) 2020 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
*///----------------------------------------------------------------------------

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
        printf(" \"%s\" will be opened instead\n", default_name);

        fp = fopen(default_name, "r");
    }

    int err = fillinTextStruct(txtstruct, fp);

    if (err)
    {
        printf(errstr[err + 1]);
        return err;
    }

    fclose(fp);

    return OK;
}

//------------------------------------------------------------------------------

int fillinTextStruct(text_t* txtstruct, FILE* fp)
{
    assert(txtstruct != nullptr);
    assert(fp != nullptr);


    txtstruct->size = CountSize(fp);
    if (txtstruct->size == 0)
        return NO_SYMB;

    txtstruct->text = GetText(fp, txtstruct->size);
    if (txtstruct->text == nullptr)
        return NO_MEMORY;

    txtstruct->num = GetLineNum(txtstruct->text, txtstruct->size);
    if (txtstruct->num == 0)
        return NO_LINES;

    txtstruct->lines = GetLine(txtstruct->text, txtstruct->num);
    if (txtstruct->lines == nullptr)
        return NO_MEMORY;


    return OK;
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
        txtstruct->lines = 0;
    }

    if (txtstruct->size != 0)
    {
        free(txtstruct->text);
        txtstruct->text = nullptr;
        txtstruct->size = 0;
    }

    return OK;
}

//------------------------------------------------------------------------------

int BCodeConstruct(bcode_t* p_bcode, size_t size)
{
    assert(p_bcode != nullptr);
    assert(size);

    p_bcode->data = (char*)calloc(size, 1);
    if (p_bcode->data == nullptr)
        return NO_MEMORY;

    p_bcode->ptr = 0;
    p_bcode->size = size;

    return OK;
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

        return 0;
    }

    p_bcode->size = CountSize(fp);
    if (p_bcode->size == 0)
        return NO_SYMB;

    p_bcode->data = GetText(fp, p_bcode->size);
    if (p_bcode->data == nullptr)
        return NO_MEMORY;

    fclose(fp);

    p_bcode->ptr = 0;

    return OK;
}

//------------------------------------------------------------------------------

int BCodeDestruct(bcode_t* p_bcode)
{
    assert(p_bcode != nullptr);


    if (p_bcode->size != 0)
    {
        free(p_bcode->data);
        p_bcode->ptr = 0;
        p_bcode->size = 0;
    }

    return OK;
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

    return (char*)"";
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
        while (isspace(*text++) && (text - start < len));

        if ((! isspace(*(text - 1))) && (*(text - 1) != '\0'))
            ++num;

        text = strchr(text, '\n') + 1;
        if ((int)text == 1)
            break;
        *(text - 1) = '\0';
    }

    return num;
}

//------------------------------------------------------------------------------

struct line* GetLine(const char* text, size_t num)
{
    assert(text != nullptr);
    assert(num);

    struct line* Lines = (struct line*)calloc(num + 2, sizeof(struct line));
    struct line* temp = Lines;

    while (num-- > 0)
    {
        while (isspace(*text++));
        --text;

        temp->str = (char*)text;
        temp->len = strlen(text);
        temp++;

        text = strchr(text, '\0') + 1;
    }

    return Lines;
}

//------------------------------------------------------------------------------

size_t GetWordNum(line_t line)
{
    assert(line.str != nullptr);

    int num = 0;
    char f = 0;
    for (int i = 0; i <= line.len; ++i)
    {
        char c = *(line.str + i);

        if (isAlpha(c) || isdigit(c))
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
    }

    return count;
}

//------------------------------------------------------------------------------

int CompareFromLeft(const void* p1, const void* p2)
{
    assert(p1);
    assert(p2);
    assert(p1 != p2);

    return StrCompare(*(line_t*)p1, *(line_t*)p2, 1);
}

//------------------------------------------------------------------------------

int CompareFromRight(const void* p1, const void* p2)
{
    assert(p1);
    assert(p2);
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

void Write(line_t* Lines, size_t num, const char* filename)
{
    assert(Lines);
    assert(num);
    assert(filename);

    FILE* fp = fopen(filename, "w");

    for(int i = 0; i < num; ++i)
        fprintf(fp, "%s\n", Lines[i].str);

    fclose(fp);
}

//------------------------------------------------------------------------------

void Print(char* text, size_t len, const char* filename)
{
    assert(text);
    assert(len);
    assert(filename);

    FILE* fp = fopen(filename, "w");

    for (int i = 0; i < len; ++i)
        fputc(text[i], fp);

    fclose(fp);
}

//------------------------------------------------------------------------------
