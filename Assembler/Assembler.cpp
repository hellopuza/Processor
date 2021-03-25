/*------------------------------------------------------------------------------
    * File:        Assembler.cpp                                               *
    * Description: Functions to parser programs on the assembler language.     *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#include "Assembler.h"

//------------------------------------------------------------------------------

int AsmConstruct(Assembler* p_asm, const char* filename)
{
    assert(filename != nullptr);

    ASM_ASSERTOK((p_asm == nullptr), ASM_NULL_INPUT_ASSEMBLER_PTR, 0, p_asm, 0);
    ASM_ASSERTOK(((p_asm->state != ASM_NOT_CONSTRUCTED) && (p_asm->state != ASM_DESTRUCTED)), ASM_CONSTRUCTED, 0, p_asm, 0);

    int err = 0;
    
    err = TextConstruct(&p_asm->input, filename);
    ASM_ASSERTOK(err, err, 0, p_asm, 0);

    err = BinCodeConstruct(&p_asm->bcode, DEFAULT_BCODE_SIZE);
    ASM_ASSERTOK(err, err, 0, p_asm, 0);

    err = LabelsConstruct(&p_asm->defined_labels,   DEFAULT_LABEL_NUM);
    ASM_ASSERTOK(err, err, 0, p_asm, 0);

    err = LabelsConstruct(&p_asm->undefined_labels, DEFAULT_LABEL_NUM);
    ASM_ASSERTOK(err, err, 0, p_asm, 0);

    p_asm->state = ASM_CONSTRUCTED;

    return ASM_OK;
}

//------------------------------------------------------------------------------

int AsmDestruct(Assembler* p_asm)
{
    ASM_ASSERTOK((p_asm == nullptr),                ASM_NULL_INPUT_ASSEMBLER_PTR, 0, p_asm, 0);
    ASM_ASSERTOK((p_asm->state == ASM_DESTRUCTED),  ASM_DESTRUCTED,               0, p_asm, 0);
    ASM_ASSERTOK((p_asm->state != ASM_CONSTRUCTED), ASM_NOT_CONSTRUCTED,          0, p_asm, 0);

    TextDestruct   (&p_asm->input);
    BinCodeDestruct(&p_asm->bcode);

    LabelsDestruct(&p_asm->defined_labels);
    LabelsDestruct(&p_asm->undefined_labels);

    p_asm->state = ASM_DESTRUCTED;

    return ASM_OK;
}

//------------------------------------------------------------------------------

int Assemble(Assembler* p_asm)
{
    ASM_ASSERTOK((p_asm == nullptr),                ASM_NULL_INPUT_ASSEMBLER_PTR, 0, p_asm, 0);
    ASM_ASSERTOK((p_asm->state != ASM_CONSTRUCTED), ASM_NOT_CONSTRUCTED,          0, p_asm, 0);
    
    for (int line_cur = 0; line_cur < p_asm->input.num; ++line_cur)
    {
        if (p_asm->input.lines[line_cur].len == 0) continue;

        char previous_line[128] = "";
        strcpy(previous_line, p_asm->input.lines[line_cur].str);
        p_asm->prev_line = previous_line;

        char* comment_ptr = DeleteComments(&p_asm->input.lines[line_cur], COMMENT);
        if (comment_ptr == NULL) continue;

        size_t words_num = GetWordsNum(p_asm->input.lines[line_cur]);
        ASM_ASSERTOK(((words_num > MAX_WORDS_IN_LINE) || (words_num == 0)), ASM_TOO_MANY_WORDS_IN_LINE, 1, p_asm, line_cur);

        char* command_word = strtok(p_asm->input.lines[line_cur].str, DELIMETERS);

        char cmd_code = CMDIdentify(command_word);
        if (cmd_code == ASM_NOT_OK)
        {
            int state = LabelCheck(&p_asm->defined_labels, p_asm->input.lines[line_cur], p_asm->bcode.ptr);
            if (state == ASM_OK)
            {
                strcpy(p_asm->input.lines[line_cur].str, previous_line);
                continue;
            }

            ASM_ASSERTOK((state == ASM_NOT_OK), ASM_UNIDENTIFIED_COMMAND, 1, p_asm, line_cur);
            ASM_ASSERTOK(state, state, 1, p_asm, line_cur);
        }

        char* operand_word = strtok(NULL, DELIMETERS);

        char reg = 0;

        switch (cmd_code)
        {
        case CMD_PUSH:
        case CMD_PUSHQ:

            ASM_ASSERTOK((operand_word == NULL), ASM_WRONG_PUSH_OPERAND_NULL, 1, p_asm, line_cur);
            
            if (isdigit(operand_word[0]) || (operand_word[0] == '-')) // numbers
            {
                if (cmd_code == CMD_PUSH)
                    WriteCommandWithIntNumber  (p_asm, cmd_code, operand_word, line_cur, ASM_WRONG_PUSH_OPERAND_NUMBER, 0x00); else
                if (cmd_code == CMD_PUSHQ)
                    WriteCommandWithFloatNumber(p_asm, cmd_code, operand_word, line_cur, ASM_WRONG_PUSHQ_OPERAND_NUMBER);
            }
            else if (operand_word[0] == '[') // RAM
            {
                WriteCommandWithPointer (p_asm, cmd_code, operand_word, line_cur, ASM_WRONG_PUSH_OPERAND_POINTER);
            }
            else // registers
            {
                WriteCommandWithRegister(p_asm, cmd_code, operand_word, line_cur, ASM_WRONG_PUSH_OPERAND_REGISTER, 0x00);
            }
            break;

        case CMD_POP:
        case CMD_POPQ:

            if (operand_word == NULL) // to space
            {
                WriteCommandSingle(p_asm, cmd_code, 0x00);
            }
            else if (operand_word[0] == '[') // to RAM
            {
                WriteCommandWithPointer (p_asm, cmd_code, operand_word, line_cur, ASM_WRONG_POP_OPERAND_POINTER);
            }
            else // to register
            {
                WriteCommandWithRegister(p_asm, cmd_code, operand_word, line_cur, ASM_WRONG_POP_OPERAND_REGISTER, 0x00);
            }
            break;

        case CMD_IN:
        case CMD_INQ:
        case CMD_OUT:
        case CMD_OUTQ:

            if (operand_word == NULL) // stack
            {
                WriteCommandSingle(p_asm, cmd_code, 0x00);
            }
            else // register
            {
                if ((cmd_code == CMD_IN)  || (cmd_code == CMD_INQ))
                    WriteCommandWithRegister(p_asm, cmd_code, operand_word, line_cur, ASM_WRONG_IN_OPERAND_REGISTER, 0x00); else
                if ((cmd_code == CMD_OUT) || (cmd_code == CMD_OUTQ))
                    WriteCommandWithRegister(p_asm, cmd_code, operand_word, line_cur, ASM_WRONG_OUT_OPERAND_REGISTER, 0x00);
            }
            break;
        
        case CMD_SCREEN:

            WriteCommandSingle(p_asm, cmd_code, 0x00);
            WriteRegister(p_asm, operand_word, line_cur, ASM_WRONG_SCREEN_OPERAND_REGISTER);
            break;

        default:
            if (isJUMP(cmd_code))
            {
                ASM_ASSERTOK((operand_word == NULL), ASM_LABEL_NEED, 1, p_asm, line_cur);

                WriteCommandSingle(p_asm, cmd_code, 0x00);

                ASM_ASSERTOK((LabelDefining(p_asm, operand_word, line_cur) == ASM_NO_MEMORY), ASM_NO_MEMORY, 0, {}, 0);
            }
            else if (operand_word == NULL)
            {
                WriteCommandSingle(p_asm, cmd_code, 0x00);
            }
            else
            {
                ASM_ASSERTOK((reg == ASM_NOT_OK), ASM_EXTRA_WORD, 1, p_asm, line_cur);
            }
            break;
        }

        strcpy(p_asm->input.lines[line_cur].str, previous_line);
    }

    int pos = LabelRedefine(p_asm);
    ASM_ASSERTOK((pos != -1), ASM_LABEL_DEFINITION_NOT_FOUND, 1, p_asm, p_asm->undefined_labels.labels[pos].line);
    
    return ASM_OK;
}

//------------------------------------------------------------------------------

int AsmWrite(Assembler* p_asm, char* filename)
{
    ASM_ASSERTOK((p_asm == nullptr),                ASM_NULL_INPUT_ASSEMBLER_PTR, 0, p_asm, 0);
    ASM_ASSERTOK((p_asm->state != ASM_CONSTRUCTED), ASM_NOT_CONSTRUCTED,          0, p_asm, 0);

    assert(filename != nullptr);

    char newname[65] = "";
    filename = GetTrueFileName(filename);
    strcpy(newname, filename);
    strcat(newname, CODE_TYPE);

    FILE* fp = nullptr;
    fp = fopen(newname, "wb");

    fwrite(p_asm->bcode.data, 1, p_asm->bcode.ptr, fp);

    fclose(fp);

    return ASM_OK;
}

//------------------------------------------------------------------------------

char CMDIdentify(const char* word)
{
    assert(word != nullptr);

    struct command cmd_key = { 0, word };

    struct command* p_cmd_struct = (struct command*)bsearch(&cmd_key, cmd_names, CMD_NUM, sizeof(cmd_names[0]), CompareCMD_Names);

    if (p_cmd_struct != nullptr) return p_cmd_struct->code;

    return ASM_NOT_OK;
}

//------------------------------------------------------------------------------

char REGIdentify(const char* word)
{
    assert(word != nullptr);

    struct reg reg_key = { 0, word };

    struct reg* p_reg_struct = (struct reg*)bsearch(&reg_key, reg_names, REG_NUM, sizeof(reg_names[0]), CompareREG_Names);

    if (p_reg_struct != nullptr) return p_reg_struct->code;

    return ASM_NOT_OK;
}

//------------------------------------------------------------------------------

char* DeleteComments(Line* line, const char comment)
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

void WriteIntNumber(Assembler* p_asm, char* op_word, size_t line, int err)
{
    assert(p_asm   != nullptr);
    assert(op_word != nullptr);

    ASM_ASSERTOK((strchr(op_word, '.') != NULL), err, 1, p_asm, line);
    
    char* end_word = 0;
    NUM_INT_TYPE number = (NUM_INT_TYPE)strtod(op_word, &end_word);
    ASM_ASSERTOK((end_word[0] != '\0'), err, 1, p_asm, line);

    if (p_asm->bcode.ptr == p_asm->bcode.size - NUMBER_INT_SIZE)
    {
        ASM_ASSERTOK((BinCodeExpand(&p_asm->bcode) == ASM_NO_MEMORY), ASM_NO_MEMORY, 0, p_asm, 0);
    }

    memcpy(p_asm->bcode.data + p_asm->bcode.ptr, &number, NUMBER_INT_SIZE);
    p_asm->bcode.ptr += NUMBER_INT_SIZE;
}

//------------------------------------------------------------------------------

void WriteCommandSingle(Assembler* p_asm, char cmd_code, char flag)
{
    assert(p_asm != nullptr);

    if (p_asm->bcode.ptr == p_asm->bcode.size - 1)
    {
        ASM_ASSERTOK((BinCodeExpand(&p_asm->bcode) == ASM_NO_MEMORY), ASM_NO_MEMORY, 0, p_asm, 0);
    }

    p_asm->bcode.data[p_asm->bcode.ptr++] = cmd_code | flag;
}

//------------------------------------------------------------------------------

void WriteCommandWithIntNumber(Assembler* p_asm, char cmd_code, char* op_word, size_t line, int err, char flag)
{
    assert(p_asm   != nullptr);
    assert(op_word != nullptr);

    WriteCommandSingle(p_asm, cmd_code, NUM_FLAG | flag);

    WriteIntNumber(p_asm, op_word, line, err);
}

//------------------------------------------------------------------------------

void WriteCommandWithFloatNumber(Assembler* p_asm, char cmd_code, char* op_word, size_t line, int err)
{
    assert(p_asm   != nullptr);
    assert(op_word != nullptr);

    char* end_word = 0;
    NUM_FLT_TYPE number = (NUM_FLT_TYPE)strtod(op_word, &end_word);
    ASM_ASSERTOK((end_word[0] != '\0'), err, 1, p_asm, line);

    if (p_asm->bcode.ptr == p_asm->bcode.size - NUMBER_FLT_SIZE - 1)
    {
        ASM_ASSERTOK((BinCodeExpand(&p_asm->bcode) == ASM_NO_MEMORY), ASM_NO_MEMORY, 0, p_asm, 0);
    }

    p_asm->bcode.data[p_asm->bcode.ptr++] = cmd_code | NUM_FLAG;
    memcpy(p_asm->bcode.data + p_asm->bcode.ptr, &number, NUMBER_FLT_SIZE);
    p_asm->bcode.ptr += NUMBER_FLT_SIZE;
}

//------------------------------------------------------------------------------

void WriteCommandWithRegister(Assembler* p_asm, char cmd_code, char* op_word, size_t line, int err, char flag)
{
    assert(p_asm   != nullptr);
    assert(op_word != nullptr);

    WriteCommandSingle(p_asm, cmd_code, REG_FLAG | flag);

    WriteRegister(p_asm, op_word, line, err);
}

//------------------------------------------------------------------------------

void WriteRegister(Assembler* p_asm, char* op_word, size_t line, int err)
{
    assert(p_asm   != nullptr);
    assert(op_word != nullptr);

    char reg_code = REGIdentify(op_word);
    ASM_ASSERTOK((reg_code == ASM_NOT_OK), err, 1, p_asm, line);

    if (p_asm->bcode.ptr == p_asm->bcode.size - 1)
    {
        ASM_ASSERTOK((BinCodeExpand(&p_asm->bcode) == ASM_NO_MEMORY), ASM_NO_MEMORY, 0, p_asm, 0);
    }

    p_asm->bcode.data[p_asm->bcode.ptr++] = reg_code;
}

//------------------------------------------------------------------------------

void WriteCommandWithPointer(Assembler* p_asm, char cmd_code, char* op_word, size_t line, int err)
{
    assert(p_asm   != nullptr);
    assert(op_word != nullptr);

    char* start_word_ptr = op_word;
    op_word = strchr(op_word, ']');
    ASM_ASSERTOK((op_word == NULL), err, 1, p_asm, line);

    *op_word = '\0';
    op_word = start_word_ptr + 1;

    char is_plus_symb  = (strchr(op_word, '+') != NULL);
    char is_minus_symb = (strchr(op_word, '-') != NULL);

    ASM_ASSERTOK((is_plus_symb && is_minus_symb), err, 1, p_asm, line);

    if ((! is_plus_symb) && (! is_minus_symb))
    {
        if (isdigit(op_word[0]))
        {
            WriteCommandWithIntNumber(p_asm, cmd_code, op_word, line, err, PTR_FLAG);
        }
        else
        {
            WriteCommandWithRegister(p_asm, cmd_code, op_word, line, err, PTR_FLAG);
        }
    }
    else
    {
        if (is_plus_symb) op_word = strtok(op_word, "+");
        else              op_word = strtok(op_word, "-");

        WriteCommandWithRegister(p_asm, cmd_code, op_word, line, err, PTR_FLAG | NUM_FLAG);

        op_word = strtok(NULL, "\0");

        if (is_minus_symb) *(--op_word) = '-';

        WriteIntNumber(p_asm, op_word, line, err);
    }
}

//------------------------------------------------------------------------------

int LabelsConstruct(Labels* p_labs, size_t num)
{
    assert(p_labs != nullptr);
    assert(num);

    p_labs->labels = (Label*)calloc(p_labs->num + 2, sizeof(Label));
    if (p_labs->labels == nullptr)
        return ASM_NO_MEMORY;

    p_labs->num = num;

    for (int i = p_labs->pos; i < p_labs->num; ++i)
    {
        p_labs->labels[i].ptr = -1;
    }

    return ASM_OK;
}

//------------------------------------------------------------------------------

int LabelsDestruct(Labels* p_labs)
{
    assert(p_labs != nullptr);

    free(p_labs->labels);

    p_labs->num = 0;
    p_labs->pos = 0;

    return ASM_OK;
}

//------------------------------------------------------------------------------

int LabelCheck(Labels* p_labs, Line line, size_t pos)
{
    assert(p_labs != nullptr);

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
            if (p_labs->pos == p_labs->num - 1)
            {
                if (LabelsExpand(p_labs) == ASM_NO_MEMORY)
                {
                    return ASM_NO_MEMORY;
                }
            }

            // check if there are two identical labels in different places
            for (int i = 0; i < p_labs->pos; ++i)
            {
                if (strcmp(p_labs->labels[i].name, lab_name) == 0)
                {
                    return ASM_LABEL_REDIFINITION;
                }
            }

            strcpy(p_labs->labels[p_labs->pos].name, lab_name);
            p_labs->labels[p_labs->pos++].ptr = pos;

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

int LabelDefining(Assembler* p_asm, char* lab_name, size_t line)
{
    assert(p_asm    != nullptr);
    assert(lab_name != nullptr);

    char lab_is_def = 0;

    for (int i = 0; i < p_asm->defined_labels.pos; ++i)
    {
        if (strcmp(p_asm->defined_labels.labels[i].name, lab_name) == 0)
        {
            memcpy(p_asm->bcode.data + p_asm->bcode.ptr, &p_asm->defined_labels.labels[i].ptr, POINTER_SIZE);
            p_asm->bcode.ptr += POINTER_SIZE;

            lab_is_def = 1;
        }
    }

    if (lab_is_def == 0)
    {
        if (p_asm->undefined_labels.pos == p_asm->undefined_labels.num - 1)
        {
            if (LabelsExpand(&p_asm->undefined_labels) == ASM_NO_MEMORY)
            {
                return ASM_NO_MEMORY;
            }
        }

        memcpy(&p_asm->undefined_labels.labels[p_asm->undefined_labels.pos].ptr, &p_asm->bcode.ptr, POINTER_SIZE);
        strcpy(p_asm->undefined_labels.labels[p_asm->undefined_labels.pos].name, lab_name);
        p_asm->undefined_labels.labels[p_asm->undefined_labels.pos++].line = line;

        size_t badptr = -1;

        if (p_asm->bcode.ptr == p_asm->bcode.size - POINTER_SIZE - 1)
        {
            ASM_ASSERTOK((BinCodeExpand(&p_asm->bcode) == ASM_NO_MEMORY), ASM_NO_MEMORY, 0, p_asm, 0);
        }

        memcpy(p_asm->bcode.data + p_asm->bcode.ptr, &badptr, POINTER_SIZE);
        p_asm->bcode.ptr += POINTER_SIZE;
    }

    return ASM_OK;
}

//------------------------------------------------------------------------------

int LabelRedefine(Assembler* p_asm)
{
    assert(p_asm != nullptr);

    for (int undef_lab_item = 0; undef_lab_item < p_asm->undefined_labels.pos; ++undef_lab_item)
    {
        char lab_is_here = 0;

        for (int def_lab_item = 0; def_lab_item < p_asm->defined_labels.pos; ++def_lab_item)
        {
            if (strcmp(p_asm->undefined_labels.labels[undef_lab_item].name, p_asm->defined_labels.labels[def_lab_item].name) == 0)
            {
                memcpy(p_asm->bcode.data + p_asm->undefined_labels.labels[undef_lab_item].ptr, &p_asm->defined_labels.labels[def_lab_item].ptr, POINTER_SIZE);
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

int LabelsExpand(Labels* p_labs)
{
    assert(p_labs != nullptr);

    p_labs->num *= 2;

    void* temp = calloc(p_labs->num + 2, sizeof(Label));
    if (temp == nullptr)
        return ASM_NO_MEMORY;

    void* oldtemp = p_labs->labels;
    memcpy(temp, p_labs->labels, p_labs->num * sizeof(Label) / 2);
    free(oldtemp);

    p_labs->labels = (Label*)temp;

    for (int i = p_labs->pos; i < p_labs->num; ++i)
    {
        p_labs->labels[i].ptr = -1;
    }

    return ASM_OK;
}

//------------------------------------------------------------------------------

void AsmPrintCode(Assembler* p_asm, size_t line, const char* logname, int err)
{
    assert(logname != nullptr);

    FILE* log = fopen(logname, "a");
    assert(log != nullptr);

    fprintf(log, "////////////////--CODE--////////////////" "\n");
    printf (     "////////////////--CODE--////////////////" "\n");

    strcpy(p_asm->input.lines[line].str, p_asm->prev_line);

    size_t true_line = line + 1;
    
    for (int i = -2; i <= 2; ++i)
    {
        if ((true_line + i > 0) && (true_line + i <= p_asm->input.num))
        {
            fprintf(log, "%s%5d: %s\n", ((i == 0)? "=>" : "  "), true_line + i, p_asm->input.lines[true_line + i - 1].str);
            printf (     "%s%5d: %s\n", ((i == 0)? "=>" : "  "), true_line + i, p_asm->input.lines[true_line + i - 1].str);
        }
    }

    fprintf(log, "////////////////////////////////////////" "\n\n");
    printf (     "////////////////////////////////////////" "\n\n");

    fclose(log);
}

//------------------------------------------------------------------------------

void AsmPrintError(const char* logname, const char* file, int line, const char* function, int err)
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
