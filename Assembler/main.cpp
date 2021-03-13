/*------------------------------------------------------------------------------
    * File:        Assembler.h                                                 *
    * Description: Program for parsing programs on the assembler language.     *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#include "Assembler.h"

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("wrong input parameters");
        return 0;
    }
    
    asm_t assembler = {};
    AsmConstruct(&assembler, argv[1]);

    Assemble(&assembler);

    AsmWrite(&assembler, argv[1]);
    
    AsmDestruct(&assembler);

    return 0;
}