/*------------------------------------------------------------------------------
    * File:        CPU.h                                                       *
    * Description: Program for executing binary programs                       *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
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
    
    cpu_t cpu = {};
    CPUConstruct(&cpu, argv[1]);

    Execute(&cpu, argv[1]);

    CPUDestruct(&cpu);

    return 0;
}