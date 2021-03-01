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
    TEMPLATE(_StackConstruct, NUM_TYPE) (&p_cpu->stkCPU_NUM, DEFAULT_STACK_CAPACITY, (char*)"stkCPU_NUM");
    TEMPLATE(_StackConstruct, PTR_TYPE) (&p_cpu->stkCPU_PTR, DEFAULT_STACK_CAPACITY, (char*)"stkCPU_PTR");

    for (int i = 0; i < REG_NUM; ++i)
    {
        p_cpu->registers[i] = TEMPLATE(NUM_TYPE, POISON);
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

    PTR_TYPE ptr  = TEMPLATE(PTR_TYPE, POISON);

    NUM_TYPE num  = TEMPLATE(NUM_TYPE, POISON);
    NUM_TYPE num1 = TEMPLATE(NUM_TYPE, POISON);
    NUM_TYPE num2 = TEMPLATE(NUM_TYPE, POISON);

    while (p_cpu->bcode.ptr < p_cpu->bcode.size)
    {
        cmd = p_cpu->bcode.data[p_cpu->bcode.ptr++];

        switch (cmd)
        {
        case CMD_HLT:

            return PROCESS_HALT;
            break;

        case CMD_PUSH | NUM_FLAG:

            if (p_cpu->bcode.size - p_cpu->bcode.ptr < NUMBER_SIZE)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            num = *(NUM_TYPE*)(p_cpu->bcode.data + p_cpu->bcode.ptr);
            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, num);
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
            if (TEMPLATE(isPOISON, NUM_TYPE) (p_cpu->registers[reg - 1]))
            {
                // register is empty
                printf("#efee %d eefe#", __LINE__);
            }

            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, p_cpu->registers[reg - 1]);
            break;

        case CMD_POP:

            TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            break;

        case CMD_POP | REG_FLAG:

            if (p_cpu->bcode.size - p_cpu->bcode.ptr < 1)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            if (reg > REG_NUM)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            p_cpu->registers[reg - 1] = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            break;

        case CMD_ADD:

            num1 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            num2 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, num2 + num1);
            break;

        case CMD_SUB:

            num1 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            num2 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, num2 - num1);
            break;

        case CMD_MUL:

            num1 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            num2 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, num2 * num1);
            break;

        case CMD_DIV:

            num1 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            num2 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            if (fabs(num1) < NIL)
                return DIVISION_BY_ZERO;
            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, num2 / num1);
            break;

        case CMD_NEG:

            num = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, -num);
            break;

        case CMD_SIN:

            num = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, (NUM_TYPE)sin(num));
            break;

        case CMD_COS:

            num = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, (NUM_TYPE)cos(num));
            break;

        case CMD_SQRT:

            num = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            if (num < 0)
                return ROOT_OF_A_NEG_NUMBER;
            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, (NUM_TYPE)sqrt(num));
            break;

        case CMD_IN:

            printf("IN: ");
            if (scanf(TEMPLATE(NUM_TYPE, PRINT_FORMAT), &num) != 1)
                return INCORRECT_INPUT;
            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, num);
            break;

        case CMD_IN | REG_FLAG:

            if (p_cpu->bcode.size - p_cpu->bcode.ptr < 1)
            {
                printf("#efee %d eefe#", __LINE__);
            }
            printf("IN: ");
            if (scanf(TEMPLATE(NUM_TYPE, PRINT_FORMAT), &num) != 1)
                return INCORRECT_INPUT;

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            if (reg > REG_NUM)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            p_cpu->registers[reg - 1] = num;
            break;

        case CMD_OUT:

            num = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, num);

            printf("OUT: " TEMPLATE(NUM_TYPE, PRINT_FORMAT) "\n", num);
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

            num1 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            num2 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            
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

            num1 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            num2 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);

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

            num1 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            num2 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);

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

            num1 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            num2 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);

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

            num1 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            num2 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);

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

            num1 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
            num2 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);

            if (num2 <= num1)
                memcpy(&p_cpu->bcode.ptr, p_cpu->bcode.data + p_cpu->bcode.ptr, POINTER_SIZE);
            else
                p_cpu->bcode.ptr += POINTER_SIZE;
            break;

        case CMD_CALL:

            if (p_cpu->bcode.size - p_cpu->bcode.ptr < POINTER_SIZE)
            {
                printf("#efee %d eefe#", __LINE__);
            }

            TEMPLATE(StackPush, PTR_TYPE) (&p_cpu->stkCPU_PTR, (PTR_TYPE)(p_cpu->bcode.ptr + POINTER_SIZE));
            memcpy(&p_cpu->bcode.ptr, p_cpu->bcode.data + p_cpu->bcode.ptr, POINTER_SIZE);
            break;

        case CMD_RET:

            ptr = TEMPLATE(StackPop, PTR_TYPE) (&p_cpu->stkCPU_PTR);

            if (TEMPLATE(isPOISON, PTR_TYPE) (ptr))
            {
                printf("#efee %d eefe#", __LINE__);
            }

            p_cpu->bcode.ptr = ptr;
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
    TEMPLATE(StackDestruct, NUM_TYPE) (&p_cpu->stkCPU_NUM);
    TEMPLATE(StackDestruct, PTR_TYPE) (&p_cpu->stkCPU_PTR);

    for (int i = 0; i < REG_NUM; ++i)
    {
        p_cpu->registers[i] = TEMPLATE(NUM_TYPE, POISON);
    }

    return OK;
}

//------------------------------------------------------------------------------


