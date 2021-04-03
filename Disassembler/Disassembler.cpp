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

Disassembler::Disassembler (const char* filename)
{
    DSM_ASSERTOK((filename == nullptr), DSM_NULL_INPUT_FILENAME, nullptr);

    // bcode_ ?? BinCode(filename);
    // output_ ?? Text(DEFAULT_LINES_NUM, MAX_CHARS_IN_LINE);

    state_ = DSM_OK;
}

//------------------------------------------------------------------------------

Disassembler::~Disassembler ()
{
    DSM_ASSERTOK((this == nullptr),          DSM_NULL_INPUT_DISASSEMBLER_PTR, nullptr);
    DSM_ASSERTOK((state_ == DSM_DESTRUCTED), DSM_DESTRUCTED,                  nullptr);

    bcode_.~BinCode();
    output_.~Text();
    labels_.~Labels();

    state_ = DSM_DESTRUCTED;
}

//------------------------------------------------------------------------------

int Disassembler::Disassemble ()
{
    DSM_ASSERTOK((this == nullptr), DSM_NULL_INPUT_DISASSEMBLER_PTR, nullptr);

    int err = 0;

    bcode_.ptr_ = 0;

    size_t line_cur = 0;

    while (bcode_.ptr_ < bcode_.size_)
    {
        if (output_.num_ - line_cur < 2) DSM_ASSERTOK((output_.Expand(MAX_CHARS_IN_LINE) == DSM_NO_MEMORY), DSM_NO_MEMORY, nullptr);

        unsigned char cmd_code = bcode_.data_[bcode_.ptr_++];
        unsigned char reg_code = 0;

        ptr_t    cmd_ptr = bcode_.ptr_ - 1;
        size_t   lab_num = 0;
        PTR_TYPE num_ptr = 0;
        INT_TYPE num_int = 0;
        FLT_TYPE num_flt = 0;

        if ((cmd_code & PTR_FLAG) && (cmd_code & REG_FLAG) && (cmd_code & NUM_FLAG))
        {
            DSM_ASSERTOK((bcode_.size_ - bcode_.ptr_ < 1 + NUMBER_INT_SIZE), DSM_NO_SPACE_FOR_POINTER, this);

            reg_code = bcode_.data_[bcode_.ptr_++];
            DSM_ASSERTOK(((reg_code > REG_NUM) || (reg_code == 0)), DSM_UNIDENTIFIED_REGISTER, this);

            num_int = *(INT_TYPE*)(bcode_.data_ + bcode_.ptr_);
            bcode_.ptr_ += NUMBER_INT_SIZE;
        }
        else if ( ((cmd_code & PTR_FLAG) && (cmd_code & NUM_FLAG)) || (isJUMP(cmd_code)) )
        {
            DSM_ASSERTOK((bcode_.size_ - bcode_.ptr_ < POINTER_SIZE), DSM_NO_SPACE_FOR_POINTER, this);

            num_ptr = *(PTR_TYPE*)(bcode_.data_ + bcode_.ptr_);
            bcode_.ptr_ += POINTER_SIZE;

            if (isJUMP(cmd_code))
            {
                if (labels_.num_ - labels_.pos_ < 2)
                    DSM_ASSERTOK((labels_.Expand() == DSM_NO_MEMORY), DSM_NO_MEMORY, nullptr);

                lab_num = labels_.pos_;
                labels_.data_[labels_.pos_].addr = num_ptr;
                labels_.data_[labels_.pos_].code = labels_.pos_++;
            }
        }
        else if ((cmd_code & REG_FLAG) || (cmd_code == CMD_SCREEN))
        {
            DSM_ASSERTOK((bcode_.size_ - bcode_.ptr_ < 1), DSM_NO_SPACE_FOR_REGISTER, this);

            reg_code = bcode_.data_[bcode_.ptr_++];
            DSM_ASSERTOK(((reg_code > REG_NUM) || (reg_code == 0)), DSM_UNIDENTIFIED_REGISTER, this);
        }
        else if (cmd_code == (CMD_PUSHQ | NUM_FLAG))
        {
            DSM_ASSERTOK((bcode_.size_ - bcode_.ptr_ < NUMBER_FLT_SIZE), DSM_NO_SPACE_FOR_NUMBER_FLT, this);

            num_flt = *(FLT_TYPE*)(bcode_.data_ + bcode_.ptr_);
            bcode_.ptr_ += NUMBER_FLT_SIZE;
        }
        else if (cmd_code == (CMD_PUSH | NUM_FLAG))
        {
            DSM_ASSERTOK((bcode_.size_ - bcode_.ptr_ < NUMBER_INT_SIZE), DSM_NO_SPACE_FOR_NUMBER_INT, this);

            num_int = *(INT_TYPE*)(bcode_.data_ + bcode_.ptr_);
            bcode_.ptr_ += NUMBER_INT_SIZE;
        }

        ptr_t temp_ptr = bcode_.ptr_;
        bcode_.ptr_ = cmd_ptr;

        err = writeCMD(&output_, cmd_code, line_cur, SECOND_WORD_PLACE);
        DSM_ASSERTOK(err, err, this);

        ++bcode_.ptr_;
        
        err = writeOperands(&output_, cmd_code, reg_code, num_int, num_flt, num_ptr, lab_num, line_cur, SECOND_WORD_PLACE, COMMENT_PLACE);
        DSM_ASSERTOK(err, err, this);

        bcode_.ptr_ = temp_ptr;

        writeCode(&output_, &bcode_, line_cur, COMMENT_PLACE, cmd_ptr, bcode_.ptr_ - cmd_ptr, COMMENT);

        // from now on, the length of the line means its binary code equivalent
        output_.lines_[line_cur].len = bcode_.ptr_ - cmd_ptr;

        ++line_cur;
    }

    qsort(labels_.data_, labels_.pos_, sizeof(labels_.data_[0]), labcmp);

    output_.num_ = line_cur;
    labels_.num_ = labels_.pos_;

    return DSM_OK;
}

//------------------------------------------------------------------------------

int Disassembler::Write (char* filename)
{
    DSM_ASSERTOK((this == nullptr),     DSM_NULL_INPUT_DISASSEMBLER_PTR, nullptr);
    DSM_ASSERTOK((filename == nullptr), DSM_NULL_INPUT_FILENAME,         nullptr);

    char newname[65] = "";
    filename = GetTrueFileName(filename);
    strcpy(newname, filename);
    strcat(newname, CODE_TYPE);

    FILE* fp = fopen(newname, "w");

    fprintf(fp, ";;;; %s Disassembler ;;;;\n\n", filename);
    fprintf(fp, "%s\n\n", "main:");

    labels_.pos_ = 0;
    bcode_.ptr_  = 0;
    
    for (int line = 0; line < output_.num_; ++line)
    {
        char flag = 0;
        while (labels_.data_[labels_.pos_].addr == bcode_.ptr_)
        {
            if (!flag) fprintf(fp, "\n");
            fprintf(fp, "%s%u:\n", LABEL, labels_.data_[labels_.pos_++].code);
            flag = 1;
        }
        if (flag) fprintf(fp, "\n");

        fprintf(fp, "%s\n", output_.lines_[line].str);
        bcode_.ptr_ += output_.lines_[line].len;
    }

    fclose(fp);

    return DSM_OK;
}

//------------------------------------------------------------------------------

int Disassembler::writeCMD (Text* text, char cmd_code, size_t line, size_t endpos)
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

    strcpy(text->lines_[line].str, "\t");
    strcpy(text->lines_[line].str + 1, cmd_word);

    for (int i = 1 + strlen(cmd_word); i < endpos; ++i)
    {
        text->lines_[line].str[i] = ' ';
    }

    return DSM_OK;
}

//------------------------------------------------------------------------------

int Disassembler::writeOperands (Text* text, char cmd_code, char reg_code, INT_TYPE num_int, FLT_TYPE num_flt, ptr_t num_ptr, size_t lab_num, size_t line, size_t startpos, size_t endpos)
{
    assert(text != nullptr);

    int flags = cmd_code & (NUM_FLAG | REG_FLAG | PTR_FLAG);

    if ( !((cmd_code & PTR_FLAG) && !(cmd_code & (NUM_FLAG | REG_FLAG))) )
        cmd_code = cmd_code & ~(NUM_FLAG | REG_FLAG | PTR_FLAG);

    size_t len = 0;

    if ( (flags & PTR_FLAG) && ((flags & NUM_FLAG) || (flags & REG_FLAG)) )
        strcpy(text->lines_[line].str + startpos + len++, "[");

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

        strcpy(text->lines_[line].str + startpos + len, reg_word);
        len += strlen(reg_word);
    }

    if ((flags & PTR_FLAG) && (flags & NUM_FLAG))
    {
        char num_word[13] = "";
        if (flags & REG_FLAG) sprintf(num_word, INT_PRINT_FORMAT,     num_int);
        else                  sprintf(num_word, POINTER_PRINT_FORMAT, num_ptr);

        if ((flags & REG_FLAG) && (num_int >= 0))
            strcpy(text->lines_[line].str + startpos + len++, "+");

        strcpy(text->lines_[line].str + startpos + len, num_word);
        len += strlen(num_word);
    }
    else if (flags & NUM_FLAG)
    {
        char num_word[25] = "";
        if (cmd_code == CMD_PUSHQ) sprintf(num_word, FLT_PRINT_FORMAT, num_flt);
        else                       sprintf(num_word, INT_PRINT_FORMAT, num_int);

        strcpy(text->lines_[line].str + startpos + len, num_word);
        len += strlen(num_word);
    }

    if (isJUMP(cmd_code))
    {
        char lab_word[13] = "";
        sprintf(lab_word, "%u", lab_num);
        strcpy(text->lines_[line].str + startpos + len, LABEL);
        len += strlen(LABEL);

        strcpy(text->lines_[line].str + startpos + len, lab_word);
        len += strlen(lab_word);
    }

    if ( (flags & PTR_FLAG) && ((flags & NUM_FLAG) || (flags & REG_FLAG)) )
        strcpy(text->lines_[line].str + startpos + len++, "]");

    for (int i = startpos + len; i < endpos; ++i)
    {
        text->lines_[line].str[i] = ' ';
    }

    return DSM_OK;
}

//------------------------------------------------------------------------------

void Disassembler::writeCode (Text* text, BinCode* bcode, size_t line, size_t pos, ptr_t ptr, size_t size, const char comment)
{
    assert(text  != nullptr);
    assert(bcode != nullptr);

    text->lines_[line].str[pos++] = comment;
    text->lines_[line].str[pos++] = ' ';

    char addr[9] = {};
    sprintf(addr, "%08X", ptr);
    strcpy(text->lines_[line].str + pos, addr);
    pos += 8;

    text->lines_[line].str[pos++] = ':';
    text->lines_[line].str[pos++] = ' ';

    for (char byte = 0; byte < size; ++byte)
    {
        sprintf(text->lines_[line].str + pos + byte*3, "%02X ", (unsigned char)bcode->data_[ptr + byte]);
    }
}

//------------------------------------------------------------------------------

void Disassembler::PrintCode (const char* logname, int err)
{
    assert(logname != nullptr);

    FILE* log = fopen(logname, "a");
    assert(log != nullptr);

    fprintf(log, " Address: %08X\n\n", bcode_.ptr_);
    printf (     " Address: %08X\n\n", bcode_.ptr_);

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

    size_t line      = bcode_.ptr_  - (bcode_.ptr_  % 0x10);
    size_t last_line = bcode_.size_ - (bcode_.size_ % 0x10);

    for (char i = -0x20; i <= 0x20; i += 0x10)
    {
        if ((line + i >= 0) && (line + i <= last_line))
        {
            fprintf(log, "%s%s%08X ", ((i == 0)? "=>" : "  "), "   ", line + i);
            printf (     "%s%s%08X ", ((i == 0)? "=>" : "  "), "   ", line + i);

            for (char byte = 0; byte < 0x10; ++byte)
            {
                if (line + i + byte == bcode_.size_) break;

                fprintf(log, "%02X  ", (unsigned char)bcode_.data_[line + i + byte]);
                printf (     "%02X  ", (unsigned char)bcode_.data_[line + i + byte]);
            }

            fprintf(log, "\n");
            printf (     "\n");
        }
    }

    for (int i = 0; i < 14 + 4*(bcode_.ptr_ % 0x10); ++i)
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

Labels::Labels () :
    num_ (DEFAULT_LABELS_NUM),
    pos_ (0)
{
    data_ = (Label*)calloc(num_ + 2, sizeof(Label));
    LABS_ASSERTOK((data_ == nullptr), DSM_NO_MEMORY);

    for (int i = pos_; i < num_; ++i)
    {
        data_[i].code = -1;
        data_[i].addr = -1;
    }

    state_ = DSM_OK;
}

//------------------------------------------------------------------------------

Labels::~Labels ()
{
    LABS_ASSERTOK((this == nullptr), DSM_NULL_INPUT_LABELS_PTR);

    if (state_ != DSM_LABELS_DESTRUCTED)
    {
        free(data_);

        num_ = 0;
        pos_ = 0;

        state_ = DSM_LABELS_DESTRUCTED;
    }
}

//------------------------------------------------------------------------------

int Labels::Expand ()
{
    LABS_ASSERTOK((this == nullptr), DSM_NULL_INPUT_LABELS_PTR);

    num_ *= 2;

    void* temp = calloc(num_ + 2, sizeof(Label));
    LABS_ASSERTOK((temp == nullptr), DSM_NO_MEMORY);

    void* oldtemp = data_;
    memcpy(temp, data_, num_ * sizeof(Label) / 2);
    free(oldtemp);

    data_ = (Label*)temp;

    for (int i = pos_; i < num_; ++i)
    {
        data_[i].code = -1;
        data_[i].addr = -1;
    }

    return DSM_OK;
}

//------------------------------------------------------------------------------

int labcmp (const void* p1, const void* p2)
{
    assert(p1 != nullptr);
    assert(p2 != nullptr);
    assert(p1 != p2);

    return ((Label*)p1)->addr - ((Label*)p2)->addr;
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
    fprintf(log, "%s\n", dsm_errstr[err + 1]);

    printf (     "ERROR: file %s  line %d  function %s\n",   file, line, function);
    printf (     "%s\n\n", dsm_errstr[err + 1]);

    fclose(log);
}

//------------------------------------------------------------------------------
