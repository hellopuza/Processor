/*------------------------------------------------------------------------------
    * File:        Assembler.cpp                                               *
    * Description: ***********************************                         *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
*///----------------------------------------------------------------------------

#include "Assembler.h"

//------------------------------------------------------------------------------

int AsmConstruct(asm_t* p_asm, const char* filename)
{
    assert(filename != nullptr);

    TextConstruct (&p_asm->input, filename);
    BCodeConstruct(&p_asm->bcode, DEFAULT_BCODE_SIZE);

    LabelsConstruct(&p_asm->defined_labels,   DEFAULT_LABEL_NUM);
    LabelsConstruct(&p_asm->undefined_labels, DEFAULT_LABEL_NUM);

    //checking

    return OK;
}

//------------------------------------------------------------------------------

int AsmDestruct(asm_t* p_asm)
{
    assert(p_asm != nullptr);

    TextDestruct (&p_asm->input);
    BCodeDestruct(&p_asm->bcode);

    LabelsDestruct(&p_asm->defined_labels);
    LabelsDestruct(&p_asm->undefined_labels);

    return OK;
}

//------------------------------------------------------------------------------

int Assemble(asm_t* p_asm)
{
    assert(p_asm != nullptr);
    
    for (int i = 0; i < p_asm->input.num; ++i)
    {
        int num = GetWordNum(p_asm->input.lines[i]);
        if ((num > MAX_WORDS_IN_LINE) || (num == 0))
        {
            printf("#efee %d eefe#", __LINE__);
        }

        char* word = strtok(p_asm->input.lines[i].str, delimeters);

        char cmd = CMDIdentify(word);
        if (cmd == NOT_OK)
        {
            int state = LabelCheck(&p_asm->defined_labels, p_asm->input.lines[i], p_asm->bcode.ptr);
            if (state == OK)
            {
                continue;
            }
            else if (state == INCORRECT_INPUT)
            {
                printf("#efee %d eefe#", __LINE__);
            }
            else if (state == NOT_OK)
            {
                printf("#efee %d eefe#", __LINE__);
            }
        }

        word = strtok(NULL, delimeters);

        char reg = 0;

        switch (cmd)
        {
        case CMD_PUSH:

            if (isdigit(word[0]) || (word[0] == '-')) // numbers
            {
                char* end = 0;
                NUM_TYPE number = (NUM_TYPE)strtod(word, &end);
                if (end[0] != '\0')
                {
                    printf("#efee %d eefe#", __LINE__);
                }
                //resize p_bcode.data
                p_asm->bcode.data[p_asm->bcode.ptr++] = cmd | NUM_FLAG;
                memcpy(p_asm->bcode.data + p_asm->bcode.ptr, &number, NUMBER_SIZE);
                p_asm->bcode.ptr += NUMBER_SIZE;
            }
            else // registers
            {
                reg = REGIdentify(word);
                if (reg == NOT_OK)
                {
                    printf("#efee %d eefe#", __LINE__);
                }
                //resize p_bcode.data
                p_asm->bcode.data[p_asm->bcode.ptr++] = cmd | REG_FLAG;
                p_asm->bcode.data[p_asm->bcode.ptr++] = reg;
            }
            break;

        case CMD_POP:

            if (word == NULL) // to space
            {
                //resize p_bcode.data
                p_asm->bcode.data[p_asm->bcode.ptr++] = cmd;
            }
            else // to register
            {
                reg = REGIdentify(word);
                if (reg == NOT_OK)
                {
                    printf("#efee %d eefe#", __LINE__);
                }
                //resize p_bcode.data
                p_asm->bcode.data[p_asm->bcode.ptr++] = cmd | REG_FLAG;
                p_asm->bcode.data[p_asm->bcode.ptr++] = reg;
            }
            break;

        case CMD_IN:

            if (word == NULL) // to stack
            {
                //resize p_bcode.data
                p_asm->bcode.data[p_asm->bcode.ptr++] = cmd;
            }
            else // to register
            {
                reg = REGIdentify(word);
                if (reg == NOT_OK)
                {
                    printf("#efee %d eefe#", __LINE__);
                }
                //resize p_bcode.data
                p_asm->bcode.data[p_asm->bcode.ptr++] = cmd | REG_FLAG;
                p_asm->bcode.data[p_asm->bcode.ptr++] = reg;
            }
            break;

        default:
            if (isJUMP(cmd))
            {
                if (word == NULL)
                {
                    printf("#efee %d eefe#", __LINE__);
                }

                //resize p_bcode.data
                p_asm->bcode.data[p_asm->bcode.ptr++] = cmd;
                LabelFind(p_asm, word);
            }
            else if (word == NULL)
            {
                //resize p_bcode.data
                p_asm->bcode.data[p_asm->bcode.ptr++] = cmd;
            }
            else
            {
                printf("#efee %d eefe#", __LINE__);
            }
            break;
        }
    }
    LabelReturn(p_asm);
    
    return OK;
}

//------------------------------------------------------------------------------

int AsmWrite(asm_t* p_asm, const char* filename)
{
    assert(p_asm != nullptr);
    assert(filename != nullptr);

    FILE* fp = nullptr;
    fp = fopen(filename, "wb");

    fwrite(p_asm->bcode.data, 1, p_asm->bcode.ptr, fp);

    fclose(fp);

    return OK;
}

//------------------------------------------------------------------------------

char CMDIdentify(const char* word)
{
    assert(word != nullptr);

    for (int i = 0; i < CMD_NUM; ++i)
        if (! strcmp(cmd_names[i].word, word))
            return cmd_names[i].code;

    return NOT_OK;
}

//------------------------------------------------------------------------------

char REGIdentify(const char* word)
{
    assert(word != nullptr);

    for (int i = 0; i < REG_NUM; ++i)
        if (! strcmp(reg_names[i].word, word))
            return reg_names[i].code;

    return NOT_OK;
}

//------------------------------------------------------------------------------

int LabelsConstruct(labs_t* p_labs, size_t size)
{
    assert(p_labs != nullptr);
    assert(size);

    p_labs->labels = (label_t*)calloc(p_labs->num, sizeof(label_t));
    if (p_labs->labels == nullptr)
        return NO_MEMORY;

    for (int i = p_labs->pos; i < p_labs->num; ++i)
    {
        p_labs->labels[i].ptr = -1;
    }

    return OK;
}

//------------------------------------------------------------------------------

int LabelsDestruct(labs_t* p_labs)
{
    assert(p_labs != nullptr);

    free(p_labs->labels);

    p_labs->num = 0;
    p_labs->pos = 0;

    return OK;
}

//------------------------------------------------------------------------------

int LabelCheck(labs_t* p_labs, line_t line, size_t pos)
{
    assert(p_labs != nullptr);

    int colon = chrcnt(line.str, ':');
    if (colon > 1)
    {
        return INCORRECT_INPUT;
    }

    if (chrcnt(line.str, ':') == 1)
    {
        char* name = strtok(line.str, delimeters);
        char* end  = strtok(NULL, delimeters);
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
                if (LabelsExpand(p_labs) == NO_MEMORY)
                {
                    printf(errstr[NO_MEMORY + 1]);

                    return NO_MEMORY;
                }
            }

            // check if there are two identical labels in different places
            p_labs->labels[p_labs->pos].name  = name;
            p_labs->labels[p_labs->pos++].ptr = pos;

            return OK;
        }
        else // NOT OK
        {
            printf("#efee %d eefe#", __LINE__);
            return INCORRECT_INPUT;
        }
    }

    return NOT_OK;
}

//------------------------------------------------------------------------------

int LabelFind(asm_t* p_asm, char* name)
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
            if (LabelsExpand(&p_asm->undefined_labels) == NO_MEMORY)
            {
                printf(errstr[NO_MEMORY + 1]);

                return NO_MEMORY;
            }
        }

        memcpy(&p_asm->undefined_labels.labels[p_asm->undefined_labels.pos].ptr, &p_asm->bcode.ptr, POINTER_SIZE);
        p_asm->undefined_labels.labels[p_asm->undefined_labels.pos++].name = name;

        size_t badptr = -1;
        memcpy(p_asm->bcode.data + p_asm->bcode.ptr, &badptr, POINTER_SIZE);
        p_asm->bcode.ptr += POINTER_SIZE;
    }

    return OK;
}

//------------------------------------------------------------------------------

int LabelReturn(asm_t* p_asm)
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
            printf("#efee %d eefe#", __LINE__);
        }
    }

    return OK;
}

//------------------------------------------------------------------------------

int LabelsExpand(labs_t* p_labs)
{
    assert(p_labs != nullptr);

    p_labs->num *= 2;

    void* temp = calloc(p_labs->num, sizeof(label_t));
    if (temp == nullptr)
        return NO_MEMORY;

    void* oldtemp = p_labs->labels;
    memcpy(temp, p_labs->labels, p_labs->num * sizeof(label_t));
    free(oldtemp);

    for (int i = p_labs->pos; i < p_labs->num; ++i)
    {
        p_labs->labels[i].ptr = -1;
    }

    return OK;
}

//------------------------------------------------------------------------------
