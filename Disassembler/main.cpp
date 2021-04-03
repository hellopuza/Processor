/*------------------------------------------------------------------------------
    * File:        main.cpp                                                    *
    * Description: Program for disassembling bibary programs.                  *
    * Created:     7 feb 2021                                                  *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
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
    
    Disassembler disasm(argv[1]);
    //DsmConstruct(&disasm, argv[1]);

    disasm.Disassemble();

    disasm.Write(argv[1]);
    
    //DsmDestruct(&disasm);

    return 0;
}