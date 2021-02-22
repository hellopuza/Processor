/*------------------------------------------------------------------------------
    * File:        main.cpp                                                    *
    * Description: ***********************************                         *
    * Created:     7 feb 2020                                                  *
    * Copyright:   (C) 2020 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
*///----------------------------------------------------------------------------

#include "Assembler.h"

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("wrong input parameters");
        return 0;
    }
    
    asm_t assembler = {};
    AsmConstruct(&assembler, argv[2] /*"prog.asm"*/);

    Assemble(&assembler);

    AsmWrite(&assembler, argv[1] /*"prog.code"*/ );
    
    AsmDestruct(&assembler);

    return 0;
}