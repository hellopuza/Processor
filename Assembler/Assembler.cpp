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

    TextConstruct(&p_asm->input, filename);
    BCodeConstruct(&p_asm->bcode, DEFAULT_BCODE_SIZE);
    //checking

    return 0;
}

//------------------------------------------------------------------------------

int AsmDestruct(asm_t* p_asm)
{
    assert(p_asm);

    TextDestruct(&p_asm->input);
    BCodeDestruct(&p_asm->bcode);

    return 0;
}

//------------------------------------------------------------------------------

int Assemble(asm_t* p_asm)
{
    assert(p_asm != nullptr);
    
    for (int i = 0; i < p_asm->input.num; ++i)
    {
        int num = GetWordNum(p_asm->input.lines[i]);

        if (num > MAX_WORDS_IN_LINE)
        {
            printf("#efee %d eefe#", __LINE__);
        }

        char* word = strtok(p_asm->input.lines[i].str, delimeters);

        char code = CMDIdentify(word);
        if (code == NOT_OK)
        {
            printf("#efee %d eefe#", __LINE__);
        }
        
        p_asm->bcode.data[p_asm->bcode.pos++] = code;
        word = strtok(NULL, delimeters);

        if (code & NUM_BIT) // PUSH
        {
            if (isdigit(word[0]) || (word[0] == '-')) // numbers
            {
                char* end = 0;
                TYPE number = (TYPE)strtod(word, &end);
                if (end[0] != '\0')
                {
                    printf("#efee %d eefe#", __LINE__);
                }
                //resize p_bcode.data

                memcpy(p_asm->bcode.data + p_asm->bcode.pos, &number, NUMBER_SIZE);
                p_asm->bcode.pos += NUMBER_SIZE;
            }
            else //registers
            {
                p_asm->bcode.data[p_asm->bcode.pos - 1] = code | REG_BIT;
                code = REGIdentify(word);
                if (code == NOT_OK)
                {
                    printf("#efee %d eefe#", __LINE__);
                }

                p_asm->bcode.data[p_asm->bcode.pos++] = code;
            }
        }
        else if (code & REG_BIT) // POP
        {
            code = REGIdentify(word);
            if (code == NOT_OK)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            p_asm->bcode.data[p_asm->bcode.pos++] = code;
        }
        else if (word != NULL)
        {
            printf("#efee %d eefe#", __LINE__);
        }
    }
    
    return 0;
}

//------------------------------------------------------------------------------

int AsmWrite(asm_t* p_asm, const char* filename)
{
    assert(p_asm != nullptr);
    assert(filename != nullptr);

    FILE* fp = nullptr;
    fp = fopen(filename, "wb");

    fwrite(p_asm->bcode.data, 1, p_asm->bcode.pos, fp);

    fclose(fp);

    return 0;
}

//------------------------------------------------------------------------------

char CMDIdentify(const char* word)
{
    for (int i = 0; i < CMD_NUM; ++i)
        if (! strcmp(cmd_names[i].word, word))
            return cmd_names[i].code;

    return NOT_OK;
}

//------------------------------------------------------------------------------

char REGIdentify(const char* word)
{
    for (int i = 0; i < REG_NUM; ++i)
        if (! strcmp(reg_names[i].word, word))
            return reg_names[i].code;

    return NOT_OK;
}

//------------------------------------------------------------------------------
