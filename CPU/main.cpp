/*------------------------------------------------------------------------------
    * File:        main.cpp                                                    *
    * Description: Program for executing binary programs                       *
    * Created:     7 feb 2021                                                  *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

#include "CPU.h"

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("wrong input parameters");
        return 0;
    }
    
    CPU cpu(argv[1]);
    //CPUConstruct(&cpu, argv[1]);

    cpu.Execute(&cpu, argv[1]);

    //CPUDestruct(&cpu);

    return 0;
}