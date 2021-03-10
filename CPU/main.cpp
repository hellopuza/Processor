/*------------------------------------------------------------------------------
    * File:        main.cpp                                                    *
    * Description: ***********************************                         *
    * Created:     7 feb 2020                                                  *
    * Copyright:   (C) 2020 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
*///----------------------------------------------------------------------------

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