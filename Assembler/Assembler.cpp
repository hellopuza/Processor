/*------------------------------------------------------------------------------
    * File:        Assembler.cpp                                               *
    * Description: Functions to parser programs on the assembler language.     *
    * Created:     7 feb 2021                                                  *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

#include "Assembler.h"

//------------------------------------------------------------------------------

Assembler::Assembler (const char* filename) :
    input_ (filename),
    bcode_ (DEFAULT_BCODE_SIZE),
    state_ (ASM_OK)
{ }

//------------------------------------------------------------------------------

Assembler::~Assembler ()
{
    ASM_ASSERTOK((this == nullptr),          ASM_NULL_INPUT_ASSEMBLER_PTR, -1);
    ASM_ASSERTOK((state_ == ASM_DESTRUCTED), ASM_DESTRUCTED,               -1);

    state_ = ASM_DESTRUCTED;
}

//------------------------------------------------------------------------------

int Assembler::Assemble ()
{
    ASM_ASSERTOK((this == nullptr), ASM_NULL_INPUT_ASSEMBLER_PTR, -1);
    
    for (int line_cur = 0; line_cur < input_.num_; ++line_cur)
    {
        if (input_.lines_[line_cur].len == 0) continue;

        char previous_line[128] = "";
        strcpy(previous_line, input_.lines_[line_cur].str);
        prev_line_ = previous_line;

        char* comment_ptr = DeleteComments(&input_.lines_[line_cur], COMMENT);
        if (comment_ptr == NULL) continue;

        size_t words_num = GetWordsNum(input_.lines_[line_cur]);
        ASM_ASSERTOK(((words_num > MAX_WORDS_IN_LINE) || (words_num == 0)), ASM_TOO_MANY_WORDS_IN_LINE, line_cur);

        char* command_word = strtok(input_.lines_[line_cur].str, DELIMETERS);

        char cmd_code = CMDIdentify(command_word);
        if (cmd_code == ASM_NOT_OK)
        {
            int state = def_labs_.Check(input_.lines_[line_cur], bcode_.ptr_);
            if (state == ASM_OK)
            {
                strcpy(input_.lines_[line_cur].str, previous_line);
                continue;
            }

            ASM_ASSERTOK((state == ASM_NOT_OK), ASM_UNIDENTIFIED_COMMAND, line_cur);
            ASM_ASSERTOK(state, state, line_cur);
        }

        char* operand_word = strtok(NULL, DELIMETERS);

        char reg = 0;

        switch (cmd_code)
        {
        case CMD_PUSH:
        case CMD_PUSHQ:

            ASM_ASSERTOK((operand_word == NULL), ASM_WRONG_PUSH_OPERAND_NULL, line_cur);
            
            if (isdigit(operand_word[0]) || (operand_word[0] == '-')) // numbers
            {
                if (cmd_code == CMD_PUSH)
                    WriteCommandWithIntNumber  (cmd_code, operand_word, line_cur, ASM_WRONG_PUSH_OPERAND_NUMBER, 0x00); else
                if (cmd_code == CMD_PUSHQ)
                    WriteCommandWithFloatNumber(cmd_code, operand_word, line_cur, ASM_WRONG_PUSHQ_OPERAND_NUMBER);
            }
            else if (operand_word[0] == '[') // RAM
            {
                WriteCommandWithPointer (cmd_code, operand_word, line_cur, ASM_WRONG_PUSH_OPERAND_POINTER);
            }
            else // registers
            {
                WriteCommandWithRegister(cmd_code, operand_word, line_cur, ASM_WRONG_PUSH_OPERAND_REGISTER, 0x00);
            }
            break;

        case CMD_POP:
        case CMD_POPQ:

            if (operand_word == NULL) // to space
            {
                WriteCommandSingle(cmd_code, 0x00);
            }
            else if (operand_word[0] == '[') // to RAM
            {
                WriteCommandWithPointer (cmd_code, operand_word, line_cur, ASM_WRONG_POP_OPERAND_POINTER);
            }
            else // to register
            {
                WriteCommandWithRegister(cmd_code, operand_word, line_cur, ASM_WRONG_POP_OPERAND_REGISTER, 0x00);
            }
            break;

        case CMD_IN:
        case CMD_INQ:
        case CMD_OUT:
        case CMD_OUTQ:

            if (operand_word == NULL) // stack
            {
                WriteCommandSingle(cmd_code, 0x00);
            }
            else // register
            {
                if ((cmd_code == CMD_IN)  || (cmd_code == CMD_INQ))
                    WriteCommandWithRegister(cmd_code, operand_word, line_cur, ASM_WRONG_IN_OPERAND_REGISTER, 0x00); else
                if ((cmd_code == CMD_OUT) || (cmd_code == CMD_OUTQ))
                    WriteCommandWithRegister(cmd_code, operand_word, line_cur, ASM_WRONG_OUT_OPERAND_REGISTER, 0x00);
            }
            break;
        
        case CMD_SCREEN:

            WriteCommandSingle(cmd_code, 0x00);
            WriteRegister(operand_word, line_cur, ASM_WRONG_SCREEN_OPERAND_REGISTER);
            break;

        default:
            if (isJUMP(cmd_code))
            {
                ASM_ASSERTOK((operand_word == NULL), ASM_LABEL_NEED, line_cur);

                WriteCommandSingle(cmd_code, 0x00);

                ASM_ASSERTOK((LabelDefining(operand_word, line_cur) == ASM_NO_MEMORY), ASM_NO_MEMORY, -1);
            }
            else if (operand_word == NULL)
            {
                WriteCommandSingle(cmd_code, 0x00);
            }
            else
            {
                ASM_ASSERTOK((reg == ASM_NOT_OK), ASM_EXTRA_WORD, line_cur);
            }
            break;
        }

        strcpy(input_.lines_[line_cur].str, previous_line);
    }

    int pos = LabelRedefine();
    ASM_ASSERTOK((pos != -1), ASM_LABEL_DEFINITION_NOT_FOUND, undef_labs_.labels_[pos].line);
    
    return ASM_OK;
}

//------------------------------------------------------------------------------

int Assembler::Write (char* filename)
{
    ASM_ASSERTOK((this == nullptr),     ASM_NULL_INPUT_ASSEMBLER_PTR, -1);
    ASM_ASSERTOK((filename == nullptr), ASM_NULL_INPUT_FILENAME,      -1);

    char newname[65] = "";
    filename = GetTrueFileName(filename);
    strcpy(newname, filename);
    strcat(newname, CODE_TYPE);

    FILE* fp = nullptr;
    fp = fopen(newname, "wb");

    fwrite(bcode_.data_, 1, bcode_.ptr_, fp);

    fclose(fp);

    return ASM_OK;
}

//------------------------------------------------------------------------------

char Assembler::CMDIdentify (const char* word)
{
    assert(word != nullptr);

    struct command cmd_key = { 0, word };

    struct command* p_cmd_struct = (struct command*)bsearch(&cmd_key, cmd_names, CMD_NUM, sizeof(cmd_names[0]), CompareCMD_Names);

    if (p_cmd_struct != nullptr) return p_cmd_struct->code;

    return ASM_NOT_OK;
}

//------------------------------------------------------------------------------

char Assembler::REGIdentify (const char* word)
{
    assert(word != nullptr);

    struct reg reg_key = { 0, word };

    struct reg* p_reg_struct = (struct reg*)bsearch(&reg_key, reg_names, REG_NUM, sizeof(reg_names[0]), CompareREG_Names);

    if (p_reg_struct != nullptr) return p_reg_struct->code;

    return ASM_NOT_OK;
}

//------------------------------------------------------------------------------

char* Assembler::DeleteComments (Line* line, const char comment)
{
    assert(line != nullptr);

    char* comment_ptr = strchr(line->str, comment);
    if (comment_ptr == NULL)
        return line->str;

    else if (line->str[0] == comment)
        return NULL;

    else
    {
        *comment_ptr = '\0';
        line->len = strlen(line->str);
    }
    
    return comment_ptr;
}

//------------------------------------------------------------------------------

void Assembler::WriteIntNumber (char* op_word, size_t line, int err)
{
    assert(op_word != nullptr);

    ASM_ASSERTOK((strchr(op_word, '.') != NULL), err, line);
    
    char* end_word = 0;
    INT_TYPE number = (INT_TYPE)strtod(op_word, &end_word);
    ASM_ASSERTOK((end_word[0] != '\0'), err, line);

    if (bcode_.ptr_ == bcode_.size_ - NUMBER_INT_SIZE)
    {
        ASM_ASSERTOK((bcode_.Expand() == ASM_NO_MEMORY), ASM_NO_MEMORY, -1);
    }

    memcpy(bcode_.data_ + bcode_.ptr_, &number, NUMBER_INT_SIZE);
    bcode_.ptr_ += NUMBER_INT_SIZE;
}

//------------------------------------------------------------------------------

void Assembler::WriteCommandSingle (char cmd_code, char flag)
{
    if (bcode_.ptr_ == bcode_.size_ - 1)
    {
        ASM_ASSERTOK((bcode_.Expand() == ASM_NO_MEMORY), ASM_NO_MEMORY, -1);
    }

    bcode_.data_[bcode_.ptr_++] = cmd_code | flag;
}

//------------------------------------------------------------------------------

void Assembler::WriteCommandWithIntNumber (char cmd_code, char* op_word, size_t line, int err, char flag)
{
    assert(op_word != nullptr);

    WriteCommandSingle(cmd_code, NUM_FLAG | flag);

    WriteIntNumber(op_word, line, err);
}

//------------------------------------------------------------------------------

void Assembler::WriteCommandWithFloatNumber (char cmd_code, char* op_word, size_t line, int err)
{
    assert(op_word != nullptr);

    char* end_word = 0;
    FLT_TYPE number = (FLT_TYPE)strtod(op_word, &end_word);
    ASM_ASSERTOK((end_word[0] != '\0'), err, line);

    if (bcode_.ptr_ == bcode_.size_ - NUMBER_FLT_SIZE - 1)
    {
        ASM_ASSERTOK((bcode_.Expand() == ASM_NO_MEMORY), ASM_NO_MEMORY, -1);
    }

    bcode_.data_[bcode_.ptr_++] = cmd_code | NUM_FLAG;
    memcpy(bcode_.data_ + bcode_.ptr_, &number, NUMBER_FLT_SIZE);
    bcode_.ptr_ += NUMBER_FLT_SIZE;
}

//------------------------------------------------------------------------------

void Assembler::WriteCommandWithRegister (char cmd_code, char* op_word, size_t line, int err, char flag)
{
    assert(op_word != nullptr);

    WriteCommandSingle(cmd_code, REG_FLAG | flag);

    WriteRegister(op_word, line, err);
}

//------------------------------------------------------------------------------

void Assembler::WriteRegister (char* op_word, size_t line, int err)
{
    assert(op_word != nullptr);

    char reg_code = REGIdentify(op_word);
    ASM_ASSERTOK((reg_code == ASM_NOT_OK), err, line);

    if (bcode_.ptr_ == bcode_.size_ - 1)
    {
        ASM_ASSERTOK((bcode_.Expand() == ASM_NO_MEMORY), ASM_NO_MEMORY, -1);
    }

    bcode_.data_[bcode_.ptr_++] = reg_code;
}

//------------------------------------------------------------------------------

void Assembler::WriteCommandWithPointer (char cmd_code, char* op_word, size_t line, int err)
{
    assert(op_word != nullptr);

    char* start_word_ptr = op_word;
    op_word = strchr(op_word, ']');
    ASM_ASSERTOK((op_word == NULL), err, line);

    *op_word = '\0';
    op_word = start_word_ptr + 1;

    char is_plus_symb  = (strchr(op_word, '+') != NULL);
    char is_minus_symb = (strchr(op_word, '-') != NULL);

    ASM_ASSERTOK((is_plus_symb && is_minus_symb), err, line);

    if ((! is_plus_symb) && (! is_minus_symb))
    {
        if (isdigit(op_word[0]))
        {
            WriteCommandWithIntNumber(cmd_code, op_word, line, err, PTR_FLAG);
        }
        else
        {
            WriteCommandWithRegister(cmd_code, op_word, line, err, PTR_FLAG);
        }
    }
    else
    {
        if (is_plus_symb) op_word = strtok(op_word, "+");
        else              op_word = strtok(op_word, "-");

        WriteCommandWithRegister(cmd_code, op_word, line, err, PTR_FLAG | NUM_FLAG);

        op_word = strtok(NULL, "\0");

        if (is_minus_symb) *(--op_word) = '-';

        WriteIntNumber(op_word, line, err);
    }
}

//------------------------------------------------------------------------------

int Assembler::LabelDefining (char* lab_name, size_t line)
{
    assert(lab_name != nullptr);

    char lab_is_def = 0;

    for (int i = 0; i < def_labs_.pos_; ++i)
    {
        if (strcmp(def_labs_.labels_[i].name, lab_name) == 0)
        {
            memcpy(bcode_.data_ + bcode_.ptr_, &def_labs_.labels_[i].ptr, POINTER_SIZE);
            bcode_.ptr_ += POINTER_SIZE;

            lab_is_def = 1;
        }
    }

    if (lab_is_def == 0)
    {
        if (undef_labs_.pos_ == undef_labs_.num_ - 1)
        {
            if (undef_labs_.Expand() == ASM_NO_MEMORY)
            {
                return ASM_NO_MEMORY;
            }
        }

        memcpy(&undef_labs_.labels_[undef_labs_.pos_].ptr, &bcode_.ptr_, POINTER_SIZE);
        strcpy(undef_labs_.labels_[undef_labs_.pos_].name, lab_name);
        undef_labs_.labels_[undef_labs_.pos_++].line = line;

        size_t badptr = -1;

        if (bcode_.ptr_ == bcode_.size_ - POINTER_SIZE - 1)
        {
            ASM_ASSERTOK((bcode_.Expand() == ASM_NO_MEMORY), ASM_NO_MEMORY, -1);
        }

        memcpy(bcode_.data_ + bcode_.ptr_, &badptr, POINTER_SIZE);
        bcode_.ptr_ += POINTER_SIZE;
    }

    return ASM_OK;
}

//------------------------------------------------------------------------------

int Assembler::LabelRedefine ()
{
    for (int undef_lab_item = 0; undef_lab_item < undef_labs_.pos_; ++undef_lab_item)
    {
        char lab_is_here = 0;

        for (int def_lab_item = 0; def_lab_item < def_labs_.pos_; ++def_lab_item)
        {
            if (strcmp(undef_labs_.labels_[undef_lab_item].name, def_labs_.labels_[def_lab_item].name) == 0)
            {
                memcpy(bcode_.data_ + undef_labs_.labels_[undef_lab_item].ptr, &def_labs_.labels_[def_lab_item].ptr, POINTER_SIZE);
                lab_is_here = 1;
            }
        }
        if (lab_is_here == 0)
        {
            return undef_lab_item;
        }
    }

    return -1;
}

//------------------------------------------------------------------------------

void Assembler::PrintCode (size_t line, const char* logname)
{
    assert(logname != nullptr);

    FILE* log = fopen(logname, "a");
    assert(log != nullptr);

    fprintf(log, "////////////////--CODE--////////////////" "\n");
    printf (     "////////////////--CODE--////////////////" "\n");

    strcpy(input_.lines_[line].str, prev_line_);

    size_t true_line = line + 1;
    
    for (int i = -2; i <= 2; ++i)
    {
        if ((true_line + i > 0) && (true_line + i <= input_.num_))
        {
            fprintf(log, "%s%5lu: %s\n", ((i == 0)? "=>" : "  "), true_line + i, input_.lines_[true_line + i - 1].str);
            printf (     "%s%5lu: %s\n", ((i == 0)? "=>" : "  "), true_line + i, input_.lines_[true_line + i - 1].str);
        }
    }

    fprintf(log, "////////////////////////////////////////" "\n\n");
    printf (     "////////////////////////////////////////" "\n\n");

    fclose(log);
}

//------------------------------------------------------------------------------

Labels::Labels () :
    num_ (DEFAULT_LABELS_NUM),
    pos_ (0)
{
    labels_ = (Label*)calloc(num_ + 2, sizeof(Label));
    LABS_ASSERTOK((labels_ == nullptr), ASM_NO_MEMORY);

    for (int i = pos_; i < num_; ++i)
    {
        labels_[i].ptr = -1;
    }

    state_ = ASM_OK;
}

//------------------------------------------------------------------------------

Labels::~Labels ()
{
    LABS_ASSERTOK((this == nullptr), ASM_NULL_INPUT_LABELS_PTR);

    if (state_ != ASM_LABELS_DESTRUCTED)
    {
        free(labels_);

        num_ = 0;
        pos_ = 0;

        state_ = ASM_LABELS_DESTRUCTED;
    }
}

//------------------------------------------------------------------------------

int Labels::Check (Line line, size_t pos)
{
    LABS_ASSERTOK((this == nullptr), ASM_NULL_INPUT_LABELS_PTR);

    int colons_num = chrcnt(line.str, ':');
    if (colons_num > 1)
    {
        return ASM_INCORRECT_LABEL_INPUT;
    }

    if (colons_num == 1)
    {
        char* lab_name = strtok(line.str, DELIMETERS);
        char* end_word = strtok(NULL, DELIMETERS);
        size_t namelen = strlen(lab_name);

        if ( ( (lab_name[namelen - 1] == ':') && (end_word == NULL)           ) ||
             ( (lab_name[namelen - 1] != ':') && (strcmp(end_word, ":") == 0) )   ) // OK
        {
            if (lab_name[namelen - 1] == ':')
            {
                lab_name[namelen - 1] = '\0';
            }
            if (pos_ == num_ - 1)
            {
                LABS_ASSERTOK((Expand() == ASM_NO_MEMORY), ASM_NO_MEMORY);
            }

            // check if there are two identical labels in different places
            for (int i = 0; i < pos_; ++i)
            {
                if (strcmp(labels_[i].name, lab_name) == 0)
                {
                    return ASM_LABEL_REDIFINITION;
                }
            }

            strcpy(labels_[pos_].name, lab_name);
            labels_[pos_++].ptr = pos;

            return ASM_OK;
        }
        else // NOT OK
        {
            return ASM_INCORRECT_LABEL_INPUT;
        }
    }

    return ASM_NOT_OK;
}

//------------------------------------------------------------------------------

int Labels::Expand ()
{
    LABS_ASSERTOK((this == nullptr), ASM_NULL_INPUT_LABELS_PTR);

    num_ *= 2;

    void* temp = calloc(num_ + 2, sizeof(Label));
    if (temp == nullptr)
        return ASM_NO_MEMORY;

    void* oldtemp = labels_;
    memcpy(temp, labels_, num_ * sizeof(Label) / 2);
    free(oldtemp);

    labels_ = (Label*)temp;

    for (int i = pos_; i < num_; ++i)
    {
        labels_[i].ptr = -1;
    }

    return ASM_OK;
}

//------------------------------------------------------------------------------

void AsmPrintError (const char* logname, const char* file, int line, const char* function, int err)
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
