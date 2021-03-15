/*------------------------------------------------------------------------------
    * File:        Disassembler.cpp                                            *
    * Description: Functions to disassembling bibary programs.                 *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#include "Disassembler.h"

//------------------------------------------------------------------------------

int DsmConstruct(dsm_t* p_dsm, const char* filename)
{
    assert(filename != nullptr);

    DSM_ASSERTOK((p_dsm == nullptr), DSM_NULL_INPUT_DISASSEMBLER_PTR, 0, p_dsm);
    DSM_ASSERTOK(((p_dsm->state != DSM_NOT_CONSTRUCTED) && (p_dsm->state != DSM_DESTRUCTED)), DSM_CONSTRUCTED, 0, p_dsm);

    int err = 0;

    err = BCodeConstruct(&p_dsm->bcode, filename);
    DSM_ASSERTOK(err, err, 0, p_dsm);

    err = TextConstruct(&p_dsm->output, DEFAULT_LINES_NUM, MAX_CHARS_IN_LINE);
    DSM_ASSERTOK(err, err, 0, p_dsm);

    p_dsm->state = DSM_CONSTRUCTED;

    return DSM_OK;
}

//------------------------------------------------------------------------------

int DsmDestruct(dsm_t* p_dsm)
{
    DSM_ASSERTOK((p_dsm == nullptr), DSM_NULL_INPUT_DISASSEMBLER_PTR, 0, p_dsm);
    DSM_ASSERTOK((p_dsm->state == DSM_DESTRUCTED), DSM_DESTRUCTED, 0, p_dsm);
    DSM_ASSERTOK((p_dsm->state != DSM_CONSTRUCTED), DSM_NOT_CONSTRUCTED, 0, p_dsm);

    BCodeDestruct(&p_dsm->bcode);
    TextDestruct (&p_dsm->output);

    p_dsm->state = DSM_DESTRUCTED;

    return DSM_OK;
}

//------------------------------------------------------------------------------

int Disassemble(dsm_t* p_dsm)
{
    DSM_ASSERTOK((p_dsm == nullptr), DSM_NULL_INPUT_DISASSEMBLER_PTR, 0, p_dsm);
    DSM_ASSERTOK((p_dsm->state != DSM_CONSTRUCTED), DSM_NOT_CONSTRUCTED, 0, p_dsm);

    assert(p_dsm->bcode.size != 0);

    p_dsm->bcode.ptr = 0;

    size_t i = 0;




    return DSM_OK;
}

//------------------------------------------------------------------------------

int DsmWrite(dsm_t* p_dsm, char* filename)
{
    DSM_ASSERTOK((p_dsm == nullptr), DSM_NULL_INPUT_DISASSEMBLER_PTR, 0, p_dsm);
    DSM_ASSERTOK((p_dsm->state != DSM_CONSTRUCTED), DSM_NOT_CONSTRUCTED, 0, p_dsm);

    assert(filename != nullptr);

    char name[30] = "";
    filename = GetTrueFileName(filename);
    strcpy(name, filename);
    strcat(name, CODE_TYPE);

    Write(p_dsm->output.lines, p_dsm->output.num, name);

    return DSM_OK;
}

//------------------------------------------------------------------------------

void DsmPrintCode(dsm_t* p_dsm, const char* logname, int err)
{
    assert(p_dsm   != nullptr);
    assert(logname != nullptr);

    FILE* log = fopen(logname, "a");
    assert(log != nullptr);

    --p_dsm->bcode.ptr;

    fprintf(log, " Address: %08X\n\n", p_dsm->bcode.ptr);
    printf (     " Address: %08X\n\n", p_dsm->bcode.ptr);

    fprintf(log, "//////////////////////--CODE--//////////////////////" "\n");
    printf (     "//////////////////////--CODE--//////////////////////" "\n");

    fprintf(log, "     Address ");
    printf (     "     Address ");
    for (char i = 0; i < 0x10; ++i)
    {
        fprintf(log, "| %X ", i);
        printf (     "| %X ", i);
    }
    fprintf(log, "\n");
    printf (     "\n");

    size_t line = p_dsm->bcode.ptr - (p_dsm->bcode.ptr % 0x10);
    size_t last_line = p_dsm->bcode.size - (p_dsm->bcode.size % 0x10);

    for (char i = -0x20; i <= 0x20; i += 0x10)
    {
        if ((line + i >= 0) && (line + i <= last_line))
        {
            fprintf(log, "%s%s%08X ", ((i == 0)? "=>" : "  "), "   ", line + i);
            printf (     "%s%s%08X ", ((i == 0)? "=>" : "  "), "   ", line + i);

            for (char b = 0; b < 0x10; ++b)
            {
                if (line + i + b == p_dsm->bcode.size) break;

                fprintf(log, "%02X  ", (unsigned char)p_dsm->bcode.data[line + i + b]);
                printf (     "%02X  ", (unsigned char)p_dsm->bcode.data[line + i + b]);
            }

            fprintf(log, "\n");
            printf (     "\n");
        }
    }

    for (int i = 0; i < 14 + 4*(p_dsm->bcode.ptr % 0x10); ++i)
    {
        fprintf(log, "=");
        printf (     "=");
    }
    fprintf(log, "/\\\n");
    printf (     "/\\\n");

    fprintf(log, "////////////////////////////////////////////////////" "\n\n");
    printf (     "////////////////////////////////////////////////////" "\n\n");

    fclose(log);
}

//------------------------------------------------------------------------------

void DsmPrintError(const char* logname, const char* file, int line, const char* function, int err)
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
    fprintf(log, "%s\n", asm_errstr[err + 1]);

    printf (     "ERROR: file %s  line %d  function %s\n",   file, line, function);
    printf (     "%s\n\n", asm_errstr[err + 1]);

    fclose(log);
}

//------------------------------------------------------------------------------
