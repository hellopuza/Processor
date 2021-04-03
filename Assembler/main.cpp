/*------------------------------------------------------------------------------
    * File:        main.cpp                                                    *
    * Description: Program for parsing programs on the assembler language.     *
    * Created:     7 feb 2021                                                  *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
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
    
    Assembler assembler(argv[1]);

    printf("1");
    assembler.Assemble();
    printf("!!!!");

    assembler.Write(argv[1]);

    return 0;
}