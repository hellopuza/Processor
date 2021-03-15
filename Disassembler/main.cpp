/*------------------------------------------------------------------------------
    * File:        main.cpp                                                    *
    * Description: Program for disassembling bibary programs.                  *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#include "Disassembler.h"

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("wrong input parameters");
        return 0;
    }
    
    dsm_t disasm = {};
    DsmConstruct(&disasm, argv[1]);

    Disassemble(&disasm);

    DsmWrite(&disasm, argv[1]);
    
    DsmDestruct(&disasm);

    return 0;
}