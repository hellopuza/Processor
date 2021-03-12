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

int AsmConstruct(asm_t* p_asm, const char* filename)
{
    assert(filename != nullptr);

    ASM_ASSERTOK((p_asm == nullptr), ASM_NULL_INPUT_ASSEMBLER_PTR, 0, {}, 0);
    ASM_ASSERTOK(((p_asm->state != ASM_NOT_CONSTRUCTED) && (p_asm->state != ASM_DESTRUCTED)), ASM_CONSTRUCTED, 0, {}, 0);

    int err = 0;
    
    err = TextConstruct(&p_asm->input, filename);
    ASM_ASSERTOK((err), err, 0, {}, 0);

    err = BCodeConstruct(&p_asm->bcode, DEFAULT_BCODE_SIZE);
    ASM_ASSERTOK((err), err, 0, {}, 0);

    err = LabelsConstruct(&p_asm->defined_labels,   DEFAULT_LABEL_NUM);
    ASM_ASSERTOK((err), err, 0, {}, 0);

    err = LabelsConstruct(&p_asm->undefined_labels, DEFAULT_LABEL_NUM);
    ASM_ASSERTOK((err), err, 0, {}, 0);

    p_asm->state = ASM_CONSTRUCTED;

    return ASM_OK;
}

//------------------------------------------------------------------------------

int AsmDestruct(asm_t* p_asm)
{
    ASM_ASSERTOK((p_asm == nullptr), ASM_NULL_INPUT_ASSEMBLER_PTR, 0, {}, 0);
    ASM_ASSERTOK((p_asm->state != ASM_CONSTRUCTED), ASM_NOT_CONSTRUCTED, 0, {}, 0);
    ASM_ASSERTOK((p_asm->state == ASM_DESTRUCTED), ASM_DESTRUCTED, 0, {}, 0);

    TextDestruct (&p_asm->input);
    BCodeDestruct(&p_asm->bcode);

    LabelsDestruct(&p_asm->defined_labels);
    LabelsDestruct(&p_asm->undefined_labels);

    p_asm->state = ASM_DESTRUCTED;

    return ASM_OK;
}

//------------------------------------------------------------------------------

int Assemble(asm_t* p_asm)
{
    ASM_ASSERTOK((p_asm == nullptr), ASM_NULL_INPUT_ASSEMBLER_PTR, 0, {}, 0);
    ASM_ASSERTOK((p_asm->state != ASM_CONSTRUCTED), ASM_NOT_CONSTRUCTED, 0, {}, 0);
    
    for (int i = 0; i < p_asm->input.num; ++i)
    {
        if (p_asm->input.lines[i].len == 0) continue;

        char previous_line[80] = "";
        strcpy(previous_line, p_asm->input.lines[i].str);

        char* pos = DeleteComments(&p_asm->input.lines[i], COMMENT);
        if (pos == NULL) continue;

        int num = GetWordNum(p_asm->input.lines[i]);
        ASM_ASSERTOK(((num > MAX_WORDS_IN_LINE) || (num == 0)), ASM_TOO_MANY_WORDS_IN_LINE, 1, p_asm->input, i);

        char* word = strtok(p_asm->input.lines[i].str, DELIMETERS);

        char cmd = CMDIdentify(word);
        if (cmd == ASM_NOT_OK)
        {
            int state = LabelCheck(&p_asm->defined_labels, p_asm->input.lines[i], p_asm->bcode.ptr);
            if (state == ASM_OK) continue;

            ASM_ASSERTOK((state == ASM_NOT_OK),                ASM_UNIDENTIFIED_COMMAND,  1, p_asm->input, i);
            ASM_ASSERTOK((state == ASM_INCORRECT_LABEL_INPUT), ASM_INCORRECT_LABEL_INPUT, 1, p_asm->input, i);
            ASM_ASSERTOK((state == ASM_NO_MEMORY),             ASM_NO_MEMORY,             1, p_asm->input, i);
            ASM_ASSERTOK((state == ASM_LABEL_REDIFINITION),    ASM_LABEL_REDIFINITION,    1, p_asm->input, i);
        }

        word = strtok(NULL, DELIMETERS);

        char reg = 0;

        switch (cmd)
        {
        case CMD_PUSH:
        case CMD_PUSHQ:

            if (isdigit(word[0]) || (word[0] == '-')) // numbers
            {
                if (cmd == CMD_PUSH)
                    WriteCommandWithIntNumber(p_asm, cmd, word, i, ASM_WRONG_PUSH_OPERAND_NUMBER, 0x00); else
                if (cmd == CMD_PUSHQ)
                    WriteCommandWithFloatNumber(p_asm, cmd, word, i, ASM_WRONG_PUSHQ_OPERAND_NUMBER);
            }
            else if (word[0] == '[') // RAM
            {
                WriteCommandWithPointer(p_asm, cmd, word, i, ASM_WRONG_PUSH_OPERAND_POINTER);
            }
            else // registers
            {
                WriteCommandWithRegister(p_asm, cmd, word, i, ASM_WRONG_PUSH_OPERAND_REGISTER, 0x00);
            }
            break;

        case CMD_POP:
        case CMD_POPQ:

            if (word == NULL) // to space
            {
                WriteCommandSingle(p_asm, cmd, 0x00);
            }
            else if (word[0] == '[') // to RAM
            {
                WriteCommandWithPointer(p_asm, cmd, word, i, ASM_WRONG_POP_OPERAND_POINTER);
            }
            else // to register
            {
                WriteCommandWithRegister(p_asm, cmd, word, i, ASM_WRONG_POP_OPERAND_REGISTER, 0x00);
            }
            break;

        case CMD_IN:
        case CMD_INQ:
        case CMD_OUT:
        case CMD_OUTQ:

            if (word == NULL) // to stack
            {
                WriteCommandSingle(p_asm, cmd, 0x00);
            }
            else // to register
            {
                if ((cmd == CMD_IN) || (cmd == CMD_INQ))
                    WriteCommandWithRegister(p_asm, cmd, word, i, ASM_WRONG_IN_OPERAND_REGISTER, 0x00); else
                if ((cmd == CMD_OUT) || (cmd == CMD_OUTQ))
                    WriteCommandWithRegister(p_asm, cmd, word, i, ASM_WRONG_OUT_OPERAND_REGISTER, 0x00);
            }
            break;
        
        case CMD_SCREEN:

            WriteCommandWithRegister(p_asm, cmd, word, i, ASM_WRONG_SCREEN_OPERAND_REGISTER, 0x00);
            break;

        default:
            if (isJUMP(cmd))
            {
                ASM_ASSERTOK((word == NULL), ASM_LABEL_NEED, 1, p_asm->input, i);

                WriteCommandSingle(p_asm, cmd, 0x00);

                ASM_ASSERTOK((LabelDefining(p_asm, word, i) == ASM_NO_MEMORY), ASM_NO_MEMORY, 0, {}, 0);
            }
            else if (word == NULL)
            {
                WriteCommandSingle(p_asm, cmd, 0x00);
            }
            else
            {
                ASM_ASSERTOK((reg == ASM_NOT_OK), ASM_EXTRA_WORD, 1, p_asm->input, i);
            }
            break;
        }

        strcpy(p_asm->input.lines[i].str, previous_line);
    }

    int pos = LabelRedefine(p_asm);
    ASM_ASSERTOK((pos != -1), ASM_LABEL_DEFINITION_NOT_FOUND, 1, p_asm->input, p_asm->undefined_labels.labels[pos].line);
    
    return ASM_OK;
}

//------------------------------------------------------------------------------

int AsmWrite(asm_t* p_asm, const char* filename)
{
    ASM_ASSERTOK((p_asm == nullptr), ASM_NULL_INPUT_ASSEMBLER_PTR, 0, {}, 0);
    assert(filename != nullptr);

    FILE* fp = nullptr;
    fp = fopen(filename, "wb");

    fwrite(p_asm->bcode.data, 1, p_asm->bcode.ptr, fp);

    fclose(fp);

    return ASM_OK;
}

//------------------------------------------------------------------------------

char CMDIdentify(const char* word)
{
    assert(word != nullptr);

    for (int i = 0; i < CMD_NUM; ++i)
        if (! strcmp(cmd_names[i].word, word))
            return cmd_names[i].code;

    return ASM_NOT_OK;
}

//------------------------------------------------------------------------------

char REGIdentify(const char* word)
{
    assert(word != nullptr);

    for (int i = 0; i < REG_NUM; ++i)
        if (! strcmp(reg_names[i].word, word))
            return reg_names[i].code;

    return ASM_NOT_OK;
}

//------------------------------------------------------------------------------

char* DeleteComments(line_t* line, const char comment)
{
    assert(line != nullptr);

    char* pos = strchr(line->str, comment);
    if (pos == NULL)
        return line->str;

    else if (line->str[0] == comment)
        return NULL;

    else
    {
        *pos = '\0';
        line->len = strlen(line->str);
    }
    
    return pos;
}

//------------------------------------------------------------------------------

void WriteIntNumber (asm_t* p_asm, char* word, size_t line, int err)
{
    assert(p_asm != nullptr);
    assert(word  != nullptr);

    ASM_ASSERTOK((strchr(word, '.') != NULL), err, 1, p_asm->input, line);
    
    char* end = 0;
    NUM_INT_TYPE number = (NUM_INT_TYPE)strtod(word, &end);
    ASM_ASSERTOK((end[0] != '\0'), err, 1, p_asm->input, line);

    if (p_asm->bcode.ptr == p_asm->bcode.size - NUMBER_INT_SIZE)
    {
        ASM_ASSERTOK((BCodeExpand(&p_asm->bcode) == ASM_NO_MEMORY), ASM_NO_MEMORY, 0, {}, 0);
    }

    memcpy(p_asm->bcode.data + p_asm->bcode.ptr, &number, NUMBER_INT_SIZE);
    p_asm->bcode.ptr += NUMBER_INT_SIZE;
}

//------------------------------------------------------------------------------

void WriteCommandSingle(asm_t* p_asm, char cmd, char flag)
{
    assert(p_asm != nullptr);

    if (p_asm->bcode.ptr == p_asm->bcode.size - 1)
    {
        ASM_ASSERTOK((BCodeExpand(&p_asm->bcode) == ASM_NO_MEMORY), ASM_NO_MEMORY, 0, {}, 0);
    }

    p_asm->bcode.data[p_asm->bcode.ptr++] = cmd | flag;
}

//------------------------------------------------------------------------------

void WriteCommandWithIntNumber(asm_t* p_asm, char cmd, char* word, size_t line, int err, char flag)
{
    assert(p_asm != nullptr);
    assert(word  != nullptr);

    WriteCommandSingle(p_asm, cmd, NUM_FLAG | flag);

    WriteIntNumber(p_asm, word, line, err);
}

//------------------------------------------------------------------------------

void WriteCommandWithFloatNumber(asm_t* p_asm, char cmd, char* word, size_t line, int err)
{
    assert(p_asm != nullptr);
    assert(word  != nullptr);

    char* end = 0;
    NUM_FLT_TYPE number = (NUM_FLT_TYPE)strtod(word, &end);
    ASM_ASSERTOK((end[0] != '\0'), err, 1, p_asm->input, line);

    if (p_asm->bcode.ptr == p_asm->bcode.size - NUMBER_FLT_SIZE - 1)
    {
        ASM_ASSERTOK((BCodeExpand(&p_asm->bcode) == ASM_NO_MEMORY), ASM_NO_MEMORY, 0, {}, 0);
    }

    p_asm->bcode.data[p_asm->bcode.ptr++] = cmd | NUM_FLAG;
    memcpy(p_asm->bcode.data + p_asm->bcode.ptr, &number, NUMBER_FLT_SIZE);
    p_asm->bcode.ptr += NUMBER_FLT_SIZE;
}

//------------------------------------------------------------------------------

void WriteCommandWithRegister(asm_t* p_asm, char cmd, char* word, size_t line, int err, char flag)
{
    assert(p_asm != nullptr);
    assert(word  != nullptr);

    char reg = REGIdentify(word);
    ASM_ASSERTOK((reg == ASM_NOT_OK), err, 1, p_asm->input, line);

    if (p_asm->bcode.ptr == p_asm->bcode.size - 1)
    {
        ASM_ASSERTOK((BCodeExpand(&p_asm->bcode) == ASM_NO_MEMORY), ASM_NO_MEMORY, 0, {}, 0);
    }

    p_asm->bcode.data[p_asm->bcode.ptr++] = cmd | REG_FLAG | flag;
    p_asm->bcode.data[p_asm->bcode.ptr++] = reg;
}

//------------------------------------------------------------------------------

void WriteCommandWithPointer(asm_t* p_asm, char cmd, char* word, size_t line, int err)
{
    assert(p_asm != nullptr);
    assert(word  != nullptr);

    char* start = word;
    word = strchr(word, ']');
    ASM_ASSERTOK((word == NULL), err, 1, p_asm->input, line);

    *word = '\0';
    word = start + 1;

    char plus_symb  = (strchr(word, '+') != NULL);
    char minus_symb = (strchr(word, '-') != NULL);

    ASM_ASSERTOK((plus_symb && minus_symb), err, 1, p_asm->input, line);

    if ((! plus_symb) && (! minus_symb))
    {
        if (isdigit(word[0]))
        {
            WriteCommandWithIntNumber(p_asm, cmd, word, line, err, PTR_FLAG);
        }
        else
        {
            WriteCommandWithRegister(p_asm, cmd, word, line, err, PTR_FLAG);
        }
    }
    else
    {
        if (plus_symb) word = strtok(word, "+");
        else           word = strtok(word, "-");

        WriteCommandWithRegister(p_asm, cmd, word, line, err, PTR_FLAG | NUM_FLAG);

        word = strtok(NULL, "\0");

        if (minus_symb) *(--word) = '-';

        WriteIntNumber(p_asm, word, line, err);
    }
}

//------------------------------------------------------------------------------

int LabelsConstruct(labs_t* p_labs, size_t num)
{
    assert(p_labs != nullptr);
    assert(num);

    p_labs->labels = (label_t*)calloc(p_labs->num + 2, sizeof(label_t));
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

int LabelsDestruct(labs_t* p_labs)
{
    assert(p_labs != nullptr);

    free(p_labs->labels);

    p_labs->num = 0;
    p_labs->pos = 0;

    return ASM_OK;
}

//------------------------------------------------------------------------------

int LabelCheck (labs_t* p_labs, line_t line, size_t pos)
{
    assert(p_labs != nullptr);

    int colon = chrcnt(line.str, ':');
    if (colon > 1)
    {
        return ASM_INCORRECT_LABEL_INPUT;
    }

    if (chrcnt(line.str, ':') == 1)
    {
        char* name = strtok(line.str, DELIMETERS);
        char* end  = strtok(NULL, DELIMETERS);
        size_t namelen = strlen(name);

        if ( ( (name[namelen - 1] == ':') && (end == NULL)           ) ||
             ( (name[namelen - 1] != ':') && (strcmp(end, ":") == 0) )    ) // OK
        {
            if (name[namelen - 1] == ':')
            {
                name[namelen - 1] = '\0';
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
                if (strcmp(p_labs->labels[i].name, name) == 0)
                {
                    return ASM_LABEL_REDIFINITION;
                }
            }

            p_labs->labels[p_labs->pos].name  = name;
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

int LabelDefining (asm_t* p_asm, char* name, size_t num_scline)
{
    assert(p_asm != nullptr);
    assert(name  != nullptr);

    char f = 0;

    for (int i = 0; i < p_asm->defined_labels.pos; ++i)
    {
        if (strcmp(p_asm->defined_labels.labels[i].name, name) == 0)
        {
            memcpy(p_asm->bcode.data + p_asm->bcode.ptr, &p_asm->defined_labels.labels[i].ptr, POINTER_SIZE);
            p_asm->bcode.ptr += POINTER_SIZE;

            f = 1;
        }
    }

    if (f == 0)
    {
        if (p_asm->undefined_labels.pos == p_asm->undefined_labels.num - 1)
        {
            if (LabelsExpand(&p_asm->undefined_labels) == ASM_NO_MEMORY)
            {
                return ASM_NO_MEMORY;
            }
        }

        memcpy(&p_asm->undefined_labels.labels[p_asm->undefined_labels.pos].ptr, &p_asm->bcode.ptr, POINTER_SIZE);
        p_asm->undefined_labels.labels[p_asm->undefined_labels.pos].name   = name;
        p_asm->undefined_labels.labels[p_asm->undefined_labels.pos++].line = num_scline;

        size_t badptr = -1;

        if (p_asm->bcode.ptr == p_asm->bcode.size - POINTER_SIZE - 1)
        {
            ASM_ASSERTOK((BCodeExpand(&p_asm->bcode) == ASM_NO_MEMORY), ASM_NO_MEMORY, 0, {}, 0);
        }

        memcpy(p_asm->bcode.data + p_asm->bcode.ptr, &badptr, POINTER_SIZE);
        p_asm->bcode.ptr += POINTER_SIZE;
    }

    return ASM_OK;
}

//------------------------------------------------------------------------------

int LabelRedefine(asm_t* p_asm)
{
    assert(p_asm != nullptr);

    for (int i = 0; i < p_asm->undefined_labels.pos; ++i)
    {
        char f = 0;

        for (int j = 0; j < p_asm->defined_labels.pos; ++j)
        {
            if (strcmp(p_asm->undefined_labels.labels[i].name, p_asm->defined_labels.labels[j].name) == 0)
            {
                memcpy(p_asm->bcode.data + p_asm->undefined_labels.labels[i].ptr, &p_asm->defined_labels.labels[j].ptr, POINTER_SIZE);
                f = 1;
            }
        }
        if (f == 0)
        {
            return i;
        }
    }

    return -1;
}

//------------------------------------------------------------------------------

int LabelsExpand(labs_t* p_labs)
{
    assert(p_labs != nullptr);

    p_labs->num *= 2;

    void* temp = calloc(p_labs->num + 2, sizeof(label_t));
    if (temp == nullptr)
        return ASM_NO_MEMORY;

    void* oldtemp = p_labs->labels;
    memcpy(temp, p_labs->labels, p_labs->num * sizeof(label_t) / 2);
    free(oldtemp);

    p_labs->labels = (label_t*)temp;

    for (int i = p_labs->pos; i < p_labs->num; ++i)
    {
        p_labs->labels[i].ptr = -1;
    }

    return ASM_OK;
}

//------------------------------------------------------------------------------

void AsmPrintCode(text_t text, size_t line, const char* logname, int err)
{
    assert(logname != nullptr);

    FILE* log = fopen(logname, "a");
    assert(log != nullptr);

    fprintf(log, "////////////////--CODE--////////////////" "\n");
    printf (     "////////////////--CODE--////////////////" "\n");

    size_t true_line = line + 1;
    
    for (int i = -2; i <= 2; ++i)
    {
        if ((true_line + i > 0) && (true_line + i <= text.num))
        {
            if ((text.lines[true_line + i - 1].len - strlen(text.lines[true_line + i - 1].str) == 1) && (err == ASM_LABEL_REDIFINITION))
            {
                text.lines[true_line + i - 1].str[strlen(text.lines[true_line + i - 1].str)] = ':';
            }
            else if (text.lines[true_line + i - 1].len - strlen(text.lines[true_line + i - 1].str) > 0)
            {
                text.lines[true_line + i - 1].str[strlen(text.lines[true_line + i - 1].str)] = ' ';
            }
            

            fprintf(log, "%s%5d: %s\n", ((i == 0)? "=>" : "  "), true_line + i, text.lines[true_line + i - 1].str);
            printf (     "%s%5d: %s\n", ((i == 0)? "=>" : "  "), true_line + i, text.lines[true_line + i - 1].str);
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
