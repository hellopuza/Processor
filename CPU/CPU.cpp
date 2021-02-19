/*------------------------------------------------------------------------------
    * File:        CPU.cpp                                                     *
    * Description: ***********************************                         *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
*///----------------------------------------------------------------------------

#include "CPU.h"

//------------------------------------------------------------------------------

int CPUConstruct(cpu_t* p_cpu, const char* filename)
{
    assert(p_cpu != nullptr);
    assert(filename != nullptr);

    fillinBCodeStruct(&p_cpu->bcode, filename);
    TEMPLATE(_StackConstruct, TYPE) (&p_cpu->stkCPU, DEFAULT_STACK_CAPACITY, (char*)"stkCPU");

    for (int i = 0; i < REG_NUM; ++i)
    {
        p_cpu->registers[i] = TEMPLATE(TYPE, POISON);
    }
    
    //checking

    return 0;
}

//------------------------------------------------------------------------------

int Execute(cpu_t* p_cpu)
{
    assert(p_cpu != nullptr);
    
    p_cpu->bcode.pos = 0;

    while (p_cpu->bcode.pos < p_cpu->bcode.size)
    {
        char cmd_code = p_cpu->bcode.data[p_cpu->bcode.pos++];

        if (cmd_code & NUM_BIT) // PUSH
        {
            if (cmd_code & REG_BIT) // registers
            {
                if (p_cpu->bcode.size - p_cpu->bcode.pos < 1)
                {
                    printf("#efee %d eefe#", __LINE__);
                }

                char reg_code = p_cpu->bcode.data[p_cpu->bcode.pos++];

                if (reg_code > REG_NUM)
                {
                    printf("#efee %d eefe#", __LINE__);
                }
                if (TEMPLATE(isPOISON, TYPE) (p_cpu->registers[reg_code - 1]))
                {
                    // register is empty
                    printf("#efee %d eefe#", __LINE__);
                }

                TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, p_cpu->registers[reg_code - 1]);
            }
            else // numbers
            {
                if (p_cpu->bcode.size - p_cpu->bcode.pos < NUMBER_SIZE)
                {
                    printf("#efee %d eefe#", __LINE__);
                }

                TYPE number = *(TYPE *)(p_cpu->bcode.data + p_cpu->bcode.pos);

                TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, number);

                p_cpu->bcode.pos += NUMBER_SIZE;
            }
        }
        else if (cmd_code & REG_BIT) // POP
        {
            if (p_cpu->bcode.size - p_cpu->bcode.pos < 1)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            char reg_code = p_cpu->bcode.data[p_cpu->bcode.pos++];

            if (reg_code > REG_NUM)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            p_cpu->registers[reg_code - 1] = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
        }
        else
        {
            if (cmd_code >= CMD_NUM)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            int state = EXECUTE_CMD(&p_cpu->stkCPU, cmd_code);
            if (state == PROCESS_END)
            {
                // end of the program
                return 0;
            }
        }
    }

    return 0;
}

//------------------------------------------------------------------------------

int CPUDestruct(cpu_t* p_cpu)
{
    assert(p_cpu != nullptr);

    BCodeDestruct(&p_cpu->bcode);
    TEMPLATE(StackDestruct, TYPE) (&p_cpu->stkCPU);

    return 0;
}

//------------------------------------------------------------------------------


