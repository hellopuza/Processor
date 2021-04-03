/*------------------------------------------------------------------------------
    * File:        Disassembler.cpp                                            *
    * Description: Functions to disassembling bibary programs.                 *
    * Created:     7 feb 2021                                                  *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

#include "Disassembler.h"

//------------------------------------------------------------------------------

int DsmConstruct(Disassembler* p_dsm, const char* filename)
{
    assert(filename != nullptr);

    DSM_ASSERTOK((p_dsm == nullptr), DSM_NULL_INPUT_DISASSEMBLER_PTR, 0, p_dsm);
    DSM_ASSERTOK(((p_dsm->state != DSM_NOT_CONSTRUCTED) && (p_dsm->state != DSM_DESTRUCTED)), DSM_CONSTRUCTED, 0, p_dsm);

    int err = 0;

    err = BinCodeConstruct(&p_dsm->bcode, filename);
    DSM_ASSERTOK(err, err, 0, p_dsm);

    err = TextConstruct(&p_dsm->output, DEFAULT_LINES_NUM, MAX_CHARS_IN_LINE);
    DSM_ASSERTOK(err, err, 0, p_dsm);

    err = LabelsConstruct(&p_dsm->labels, DEFAULT_LABEL_NUM);
    DSM_ASSERTOK(err, err, 0, p_dsm);

    p_dsm->state = DSM_CONSTRUCTED;

    return DSM_OK;
}

//------------------------------------------------------------------------------

int DsmDestruct(Disassembler* p_dsm)
{
    DSM_ASSERTOK((p_dsm == nullptr),                DSM_NULL_INPUT_DISASSEMBLER_PTR, 0, p_dsm);
    DSM_ASSERTOK((p_dsm->state == DSM_DESTRUCTED),  DSM_DESTRUCTED,                  0, p_dsm);
    DSM_ASSERTOK((p_dsm->state != DSM_CONSTRUCTED), DSM_NOT_CONSTRUCTED,             0, p_dsm);

    BinCodeDestruct(&p_dsm->bcode);
    TextDestruct   (&p_dsm->output);
    LabelsDestruct (&p_dsm->labels);

    p_dsm->state = DSM_DESTRUCTED;

    return DSM_OK;
}

//------------------------------------------------------------------------------

int Disassemble(Disassembler* p_dsm)
{
    DSM_ASSERTOK((p_dsm == nullptr),                DSM_NULL_INPUT_DISASSEMBLER_PTR, 0, p_dsm);
    DSM_ASSERTOK((p_dsm->state != DSM_CONSTRUCTED), DSM_NOT_CONSTRUCTED,             0, p_dsm);

    assert(p_dsm->bcode.size != 0);

    int err = 0;

    p_dsm->bcode.ptr = 0;

    size_t line_cur = 0;

    while (p_dsm->bcode.ptr < p_dsm->bcode.size)
    {
        if (p_dsm->output.num - line_cur < 2)
            DSM_ASSERTOK((TextExpand(&p_dsm->output, MAX_CHARS_IN_LINE) == DSM_NO_MEMORY), DSM_NO_MEMORY, 0, p_dsm);

        unsigned char cmd_code = p_dsm->bcode.data[p_dsm->bcode.ptr++];
        unsigned char reg_code = 0;

        ptr_t        cmd_ptr = p_dsm->bcode.ptr - 1;
        size_t       lab_num = 0;
        PTR_TYPE     num_ptr = 0;
        NUM_INT_TYPE num_int = 0;
        NUM_FLT_TYPE num_flt = 0;

        if ((cmd_code & PTR_FLAG) && (cmd_code & REG_FLAG) && (cmd_code & NUM_FLAG))
        {
            DSM_ASSERTOK((p_dsm->bcode.size - p_dsm->bcode.ptr < 1 + NUMBER_INT_SIZE), DSM_NO_SPACE_FOR_POINTER, 1, p_dsm);

            reg_code = p_dsm->bcode.data[p_dsm->bcode.ptr++];
            DSM_ASSERTOK(((reg_code > REG_NUM) || (reg_code == 0)), DSM_UNIDENTIFIED_REGISTER, 1, p_dsm);

            num_int = *(NUM_INT_TYPE*)(p_dsm->bcode.data + p_dsm->bcode.ptr);
            p_dsm->bcode.ptr += NUMBER_INT_SIZE;
        }
        else if ( ((cmd_code & PTR_FLAG) && (cmd_code & NUM_FLAG)) || (isJUMP(cmd_code)) )
        {
            DSM_ASSERTOK((p_dsm->bcode.size - p_dsm->bcode.ptr < POINTER_SIZE), DSM_NO_SPACE_FOR_POINTER, 1, p_dsm);

            num_ptr = *(PTR_TYPE*)(p_dsm->bcode.data + p_dsm->bcode.ptr);
            p_dsm->bcode.ptr += POINTER_SIZE;

            if (isJUMP(cmd_code))
            {
                if (p_dsm->labels.num - p_dsm->labels.pos < 2)
                    DSM_ASSERTOK((LabelsExpand(&p_dsm->labels) == DSM_NO_MEMORY), DSM_NO_MEMORY, 0, p_dsm);

                lab_num = p_dsm->labels.pos;
                p_dsm->labels.data[p_dsm->labels.pos].addr = num_ptr;
                p_dsm->labels.data[p_dsm->labels.pos].code = p_dsm->labels.pos++;
            }
        }
        else if ((cmd_code & REG_FLAG) || (cmd_code == CMD_SCREEN))
        {
            DSM_ASSERTOK((p_dsm->bcode.size - p_dsm->bcode.ptr < 1), DSM_NO_SPACE_FOR_REGISTER, 1, p_dsm);

            reg_code = p_dsm->bcode.data[p_dsm->bcode.ptr++];
            DSM_ASSERTOK(((reg_code > REG_NUM) || (reg_code == 0)), DSM_UNIDENTIFIED_REGISTER, 1, p_dsm);
        }
        else if (cmd_code == (CMD_PUSHQ | NUM_FLAG))
        {
            DSM_ASSERTOK((p_dsm->bcode.size - p_dsm->bcode.ptr < NUMBER_FLT_SIZE), DSM_NO_SPACE_FOR_NUMBER_FLT, 1, p_dsm);

            num_flt = *(NUM_FLT_TYPE*)(p_dsm->bcode.data + p_dsm->bcode.ptr);
            p_dsm->bcode.ptr += NUMBER_FLT_SIZE;
        }
        else if (cmd_code == (CMD_PUSH | NUM_FLAG))
        {
            DSM_ASSERTOK((p_dsm->bcode.size - p_dsm->bcode.ptr < NUMBER_INT_SIZE), DSM_NO_SPACE_FOR_NUMBER_INT, 1, p_dsm);

            num_int = *(NUM_INT_TYPE*)(p_dsm->bcode.data + p_dsm->bcode.ptr);
            p_dsm->bcode.ptr += NUMBER_INT_SIZE;
        }

        ptr_t temp_ptr = p_dsm->bcode.ptr;
        p_dsm->bcode.ptr = cmd_ptr;

        err = writeCMD(&p_dsm->output, cmd_code, line_cur, SECOND_WORD_PLACE);
        DSM_ASSERTOK(err, err, 1, p_dsm);

        ++p_dsm->bcode.ptr;
        
        err = writeOperands(&p_dsm->output, cmd_code, reg_code, num_int, num_flt, num_ptr, lab_num, line_cur, SECOND_WORD_PLACE, COMMENT_PLACE);
        DSM_ASSERTOK(err, err, 1, p_dsm);

        p_dsm->bcode.ptr = temp_ptr;

        writeCode(&p_dsm->output, &p_dsm->bcode, line_cur, COMMENT_PLACE, cmd_ptr, p_dsm->bcode.ptr - cmd_ptr, COMMENT);

        // from now on, the length of the line means its binary code equivalent
        p_dsm->output.lines[line_cur].len = p_dsm->bcode.ptr - cmd_ptr;

        ++line_cur;
    }

    qsort(p_dsm->labels.data, p_dsm->labels.pos, sizeof(p_dsm->labels.data[0]), labcmp);

    p_dsm->output.num = line_cur;
    p_dsm->labels.num = p_dsm->labels.pos;

    return DSM_OK;
}

//------------------------------------------------------------------------------

int writeCMD(Text* text, char cmd_code, size_t line, size_t endpos)
{
    assert(text != nullptr);

    if ( !((cmd_code & PTR_FLAG) && !(cmd_code & (NUM_FLAG | REG_FLAG))) )
        cmd_code = cmd_code & ~(NUM_FLAG | REG_FLAG | PTR_FLAG);

    char* cmd_word = nullptr;

    for (int i = 0; i < CMD_NUM; ++i)
        if (cmd_names[i].code == cmd_code)
        {
            cmd_word = (char*)cmd_names[i].word;
            break;
        }
    if (cmd_word == nullptr)
        return DSM_UNIDENTIFIED_COMMAND;

    strcpy(text->lines[line].str, "\t");
    strcpy(text->lines[line].str + 1, cmd_word);

    for (int i = 1 + strlen(cmd_word); i < endpos; ++i)
    {
        text->lines[line].str[i] = ' ';
    }

    return DSM_OK;
}

//------------------------------------------------------------------------------

int writeOperands(Text* text, char cmd_code, char reg_code, NUM_INT_TYPE num_int, NUM_FLT_TYPE num_flt, ptr_t num_ptr, size_t lab_num, size_t line, size_t startpos, size_t endpos)
{
    assert(text != nullptr);

    int flags = cmd_code & (NUM_FLAG | REG_FLAG | PTR_FLAG);

    if ( !((cmd_code & PTR_FLAG) && !(cmd_code & (NUM_FLAG | REG_FLAG))) )
        cmd_code = cmd_code & ~(NUM_FLAG | REG_FLAG | PTR_FLAG);

    size_t len = 0;

    if ( (flags & PTR_FLAG) && ((flags & NUM_FLAG) || (flags & REG_FLAG)) )
        strcpy(text->lines[line].str + startpos + len++, "[");

    if ((flags & REG_FLAG) || (cmd_code == CMD_SCREEN))
    {
        char* reg_word = nullptr;

        for (int i = 0; i < REG_NUM; ++i)
            if (reg_names[i].code == reg_code)
            {
                reg_word = (char*)reg_names[i].word;
                break;
            }
        if (reg_word == nullptr)
            return DSM_UNIDENTIFIED_REGISTER;

        strcpy(text->lines[line].str + startpos + len, reg_word);
        len += strlen(reg_word);
    }

    if ((flags & PTR_FLAG) && (flags & NUM_FLAG))
    {
        char num_word[13] = "";
        if (flags & REG_FLAG) sprintf(num_word, INT_PRINT_FORMAT,     num_int);
        else                  sprintf(num_word, POINTER_PRINT_FORMAT, num_ptr);

        if ((flags & REG_FLAG) && (num_int >= 0))
            strcpy(text->lines[line].str + startpos + len++, "+");

        strcpy(text->lines[line].str + startpos + len, num_word);
        len += strlen(num_word);
    }
    else if (flags & NUM_FLAG)
    {
        char num_word[25] = "";
        if (cmd_code == CMD_PUSHQ) sprintf(num_word, FLT_PRINT_FORMAT, num_flt);
        else                       sprintf(num_word, INT_PRINT_FORMAT, num_int);

        strcpy(text->lines[line].str + startpos + len, num_word);
        len += strlen(num_word);
    }

    if (isJUMP(cmd_code))
    {
        char lab_word[13] = "";
        sprintf(lab_word, "%u", lab_num);
        strcpy(text->lines[line].str + startpos + len, LABEL);
        len += strlen(LABEL);

        strcpy(text->lines[line].str + startpos + len, lab_word);
        len += strlen(lab_word);
    }

    if ( (flags & PTR_FLAG) && ((flags & NUM_FLAG) || (flags & REG_FLAG)) )
        strcpy(text->lines[line].str + startpos + len++, "]");

    for (int i = startpos + len; i < endpos; ++i)
    {
        text->lines[line].str[i] = ' ';
    }

    return DSM_OK;
}

//------------------------------------------------------------------------------

void writeCode(Text* text, BinCode* bcode, size_t line, size_t pos, ptr_t ptr, size_t size, const char comment)
{
    assert(text  != nullptr);
    assert(bcode != nullptr);

    text->lines[line].str[pos++] = comment;
    text->lines[line].str[pos++] = ' ';

    char addr[9] = {};
    sprintf(addr, "%08X", ptr);
    strcpy(text->lines[line].str + pos, addr);
    pos += 8;

    text->lines[line].str[pos++] = ':';
    text->lines[line].str[pos++] = ' ';

    for (char byte = 0; byte < size; ++byte)
    {
        sprintf(text->lines[line].str + pos + byte*3, "%02X ", (unsigned char)bcode->data[ptr + byte]);
    }
}

//------------------------------------------------------------------------------

int DsmWrite(Disassembler* p_dsm, char* filename)
{
    DSM_ASSERTOK((p_dsm == nullptr),                DSM_NULL_INPUT_DISASSEMBLER_PTR, 0, p_dsm);
    DSM_ASSERTOK((p_dsm->state != DSM_CONSTRUCTED), DSM_NOT_CONSTRUCTED,             0, p_dsm);

    assert(filename != nullptr);

    char newname[65] = "";
    filename = GetTrueFileName(filename);
    strcpy(newname, filename);
    strcat(newname, CODE_TYPE);

    FILE* fp = fopen(newname, "w");

    fprintf(fp, ";;;; %s Disassembler ;;;;\n\n", filename);
    fprintf(fp, "%s\n\n", "main:");

    p_dsm->labels.pos = 0;
    p_dsm->bcode.ptr  = 0;
    
    for (int line = 0; line < p_dsm->output.num; ++line)
    {
        char flag = 0;
        while (p_dsm->labels.data[p_dsm->labels.pos].addr == p_dsm->bcode.ptr)
        {
            if (!flag) fprintf(fp, "\n");
            fprintf(fp, "%s%u:\n", LABEL, p_dsm->labels.data[p_dsm->labels.pos++].code);
            flag = 1;
        }
        if (flag) fprintf(fp, "\n");

        fprintf(fp, "%s\n", p_dsm->output.lines[line].str);
        p_dsm->bcode.ptr += p_dsm->output.lines[line].len;
    }

    fclose(fp);

    return DSM_OK;
}

//------------------------------------------------------------------------------

int LabelsConstruct(Labels* p_labs, size_t num)
{
    assert(p_labs != nullptr);
    assert(num);

    p_labs->data = (Label*)calloc(p_labs->num + 2, sizeof(Label));
    if (p_labs->data == nullptr)
        return DSM_NO_MEMORY;

    p_labs->num = num;

    for (int i = p_labs->pos; i < p_labs->num; ++i)
    {
        p_labs->data[i].code = -1;
        p_labs->data[i].addr = -1;
    }

    return DSM_OK;
}

//------------------------------------------------------------------------------

int LabelsDestruct(Labels* p_labs)
{
    assert(p_labs != nullptr);

    free(p_labs->data);

    p_labs->num = 0;
    p_labs->pos = 0;

    return DSM_OK;
}

//------------------------------------------------------------------------------

int LabelsExpand(Labels* p_labs)
{
    assert(p_labs != nullptr);

    p_labs->num *= 2;

    void* temp = calloc(p_labs->num + 2, sizeof(Label));
    if (temp == nullptr)
        return DSM_NO_MEMORY;

    void* oldtemp = p_labs->data;
    memcpy(temp, p_labs->data, p_labs->num * sizeof(Label) / 2);
    free(oldtemp);

    p_labs->data = (Label*)temp;

    for (int i = p_labs->pos; i < p_labs->num; ++i)
    {
        p_labs->data[i].code = -1;
        p_labs->data[i].addr = -1;
    }

    return DSM_OK;
}

//------------------------------------------------------------------------------

int labcmp(const void* p1, const void* p2)
{
    assert(p1 != nullptr);
    assert(p2 != nullptr);
    assert(p1 != p2);

    return ((Label*)p1)->addr - ((Label*)p2)->addr;
}

//------------------------------------------------------------------------------

void DsmPrintCode(Disassembler* p_dsm, const char* logname, int err)
{
    assert(p_dsm   != nullptr);
    assert(logname != nullptr);

    FILE* log = fopen(logname, "a");
    assert(log != nullptr);

    fprintf(log, " Address: %08X\n\n", p_dsm->bcode.ptr);
    printf (     " Address: %08X\n\n", p_dsm->bcode.ptr);

    fprintf(log, "//////////////////////////////////--CODE--//////////////////////////////////" "\n");
    printf (     "//////////////////////////////////--CODE--//////////////////////////////////" "\n");

    fprintf(log, "     Address ");
    printf (     "     Address ");
    for (char i = 0; i < 0x10; ++i)
    {
        fprintf(log, "| %X ", i);
        printf (     "| %X ", i);
    }
    fprintf(log, "\n");
    printf (     "\n");

    size_t line      = p_dsm->bcode.ptr  - (p_dsm->bcode.ptr  % 0x10);
    size_t last_line = p_dsm->bcode.size - (p_dsm->bcode.size % 0x10);

    for (char i = -0x20; i <= 0x20; i += 0x10)
    {
        if ((line + i >= 0) && (line + i <= last_line))
        {
            fprintf(log, "%s%s%08X ", ((i == 0)? "=>" : "  "), "   ", line + i);
            printf (     "%s%s%08X ", ((i == 0)? "=>" : "  "), "   ", line + i);

            for (char byte = 0; byte < 0x10; ++byte)
            {
                if (line + i + byte == p_dsm->bcode.size) break;

                fprintf(log, "%02X  ", (unsigned char)p_dsm->bcode.data[line + i + byte]);
                printf (     "%02X  ", (unsigned char)p_dsm->bcode.data[line + i + byte]);
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

    fprintf(log, "////////////////////////////////////////////////////////////////////////////" "\n\n");
    printf (     "////////////////////////////////////////////////////////////////////////////" "\n\n");

    fclose(log);
}

//------------------------------------------------------------------------------

void DsmPrintError(const char* logname, const char* file, int line, const char* function, int err)
{
    assert(function != nullptr);
    assert(logname  != nullptr);
    assert(file     != nullptr);

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
