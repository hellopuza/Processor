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
    /*
    if (argc != 2)
    {
        printf("wrong input parameters");
        return 0;
    }
    */
    // argv[1] --- prog.code

    cpu_t cpu = {};
    CPUConstruct(&cpu, "prog.code" /*argv[1]*/);

    Execute(&cpu);

    CPUDestruct(&cpu);

    return 0;
}