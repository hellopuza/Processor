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

    return OK;
}

//------------------------------------------------------------------------------

int Execute(cpu_t* p_cpu)
{
    assert(p_cpu != nullptr);
    assert(p_cpu->bcode.size != 0);

    p_cpu->bcode.ptr = 0;

    unsigned char cmd = 0;
    char reg = 0;

    TYPE num  = TEMPLATE(TYPE, POISON);
    TYPE num1 = TEMPLATE(TYPE, POISON);
    TYPE num2 = TEMPLATE(TYPE, POISON);

    while (p_cpu->bcode.ptr < p_cpu->bcode.size)
    {
        cmd = p_cpu->bcode.data[p_cpu->bcode.ptr++];

        switch (cmd)
        {
        case CMD_END:

            return PROCESS_END;
            break;

        case CMD_PUSH | NUM_FLAG:

            if (p_cpu->bcode.size - p_cpu->bcode.ptr < NUMBER_SIZE)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            num = *(TYPE*)(p_cpu->bcode.data + p_cpu->bcode.ptr);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num);
            p_cpu->bcode.ptr += NUMBER_SIZE;
            break;

        case CMD_PUSH | REG_FLAG:

            if (p_cpu->bcode.size - p_cpu->bcode.ptr < 1)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            if (reg > REG_NUM)
            {
                printf("#efee %d eefe#", __LINE__);
            }
            if (TEMPLATE(isPOISON, TYPE) (p_cpu->registers[reg - 1]))
            {
                // register is empty
                printf("#efee %d eefe#", __LINE__);
            }

            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, p_cpu->registers[reg - 1]);
            break;

        case CMD_POP:

            if (p_cpu->bcode.size - p_cpu->bcode.ptr < 1)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            if (reg > REG_NUM)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            p_cpu->registers[reg - 1] = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            break;

        case CMD_ADD:

            num1 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            num2 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num2 + num1);
            break;

        case CMD_SUB:

            num1 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            num2 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num2 - num1);
            break;

        case CMD_MUL:

            num1 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            num2 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num2 * num1);
            break;

        case CMD_DIV:

            num1 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            num2 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            if (fabs(num1) < NIL)
                return DIVISION_BY_ZERO;
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num2 / num1);
            break;

        case CMD_NEG:

            num = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, -num);
            break;

        case CMD_SIN:

            num = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, (TYPE)sin(num));
            break;

        case CMD_COS:

            num = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, (TYPE)cos(num));
            break;

        case CMD_SQRT:

            num = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            if (num < 0)
                return ROOT_OF_A_NEG_NUMBER;
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, (TYPE)sqrt(num));
            break;

        case CMD_IN:

            printf("IN: ");
            if (scanf(TEMPLATE(TYPE, PRINT_FORMAT), &num) != 1)
                return INCORRECT_INPUT;
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num);
            break;

        case CMD_IN | REG_FLAG:

            if (p_cpu->bcode.size - p_cpu->bcode.ptr < 1)
            {
                printf("#efee %d eefe#", __LINE__);
            }
            printf("IN: ");
            if (scanf(TEMPLATE(TYPE, PRINT_FORMAT), &num) != 1)
                return INCORRECT_INPUT;

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            if (reg > REG_NUM)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            p_cpu->registers[reg - 1] = num;
            break;

        case CMD_OUT:

            num = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num);
            printf("OUT: " TEMPLATE(TYPE, PRINT_FORMAT) "\n", num);
            break;

        case CMD_JMP:

            if (p_cpu->bcode.size - p_cpu->bcode.ptr < POINTER_SIZE)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            memcpy(&p_cpu->bcode.ptr, p_cpu->bcode.data + p_cpu->bcode.ptr, POINTER_SIZE);
            break;

        case CMD_JE:

            if (p_cpu->bcode.size - p_cpu->bcode.ptr < POINTER_SIZE)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            num1 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            num2 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num2);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num1);
            
            if (num2 == num1)
                memcpy(&p_cpu->bcode.ptr, p_cpu->bcode.data + p_cpu->bcode.ptr, POINTER_SIZE);
            else
                p_cpu->bcode.ptr += POINTER_SIZE;
            break;

        case CMD_JNE:

            if (p_cpu->bcode.size - p_cpu->bcode.ptr < POINTER_SIZE)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            num1 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            num2 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num2);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num1);

            if (num2 != num1)
                memcpy(&p_cpu->bcode.ptr, p_cpu->bcode.data + p_cpu->bcode.ptr, POINTER_SIZE);
            else
                p_cpu->bcode.ptr += POINTER_SIZE;
            break;

        case CMD_JA:

            if (p_cpu->bcode.size - p_cpu->bcode.ptr < POINTER_SIZE)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            num1 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            num2 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num2);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num1);

            if (num2 > num1)
                memcpy(&p_cpu->bcode.ptr, p_cpu->bcode.data + p_cpu->bcode.ptr, POINTER_SIZE);
            else
                p_cpu->bcode.ptr += POINTER_SIZE;
            break;

        case CMD_JAE:

            if (p_cpu->bcode.size - p_cpu->bcode.ptr < POINTER_SIZE)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            num1 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            num2 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num2);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num1);

            if (num2 >= num1)
                memcpy(&p_cpu->bcode.ptr, p_cpu->bcode.data + p_cpu->bcode.ptr, POINTER_SIZE);
            else
                p_cpu->bcode.ptr += POINTER_SIZE;
            break;

        case CMD_JB:

            if (p_cpu->bcode.size - p_cpu->bcode.ptr < POINTER_SIZE)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            num1 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            num2 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num2);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num1);

            if (num2 < num1)
                memcpy(&p_cpu->bcode.ptr, p_cpu->bcode.data + p_cpu->bcode.ptr, POINTER_SIZE);
            else
                p_cpu->bcode.ptr += POINTER_SIZE;
            break;

        case CMD_JBE:

            if (p_cpu->bcode.size - p_cpu->bcode.ptr < POINTER_SIZE)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            num1 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            num2 = TEMPLATE(StackPop, TYPE) (&p_cpu->stkCPU);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num2);
            TEMPLATE(StackPush, TYPE) (&p_cpu->stkCPU, num1);

            if (num2 <= num1)
                memcpy(&p_cpu->bcode.ptr, p_cpu->bcode.data + p_cpu->bcode.ptr, POINTER_SIZE);
            else
                p_cpu->bcode.ptr += POINTER_SIZE;
            break;

        default:
            printf("#efee %d eefe#", __LINE__);

            return NOT_OK;
        }
    }

    return OK;
}

//------------------------------------------------------------------------------

int CPUDestruct(cpu_t* p_cpu)
{
    assert(p_cpu != nullptr);

    BCodeDestruct(&p_cpu->bcode);
    TEMPLATE(StackDestruct, TYPE) (&p_cpu->stkCPU);

    for (int i = 0; i < REG_NUM; ++i)
    {
        p_cpu->registers[i] = TEMPLATE(TYPE, POISON);
    }

    return OK;
}

//------------------------------------------------------------------------------


