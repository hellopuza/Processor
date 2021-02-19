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
    /*
    if (argc != 3)
    {
        printf("wrong input parameters");
        return 0;
    }
    */
    // argv[1] --- prog.code
    // argv[2] --- prog.asm
    
    asm_t assembler = {};
    AsmConstruct(&assembler, "prog.asm" /*argv[2]*/ );

    /*
    printf("main.cpp\n");
    printf("p_asm %p\n", &assembler);
    printf("asm.p_input %p\n", assembler.p_input);
    printf("asm.p_input->num %d\n\n", assembler.p_input.num);
    */

    Assemble(&assembler);
    
    AsmWrite(&assembler, "prog.code" /*argv[1]*/ );
    
    AsmDestruct(&assembler);

    return 0;
}