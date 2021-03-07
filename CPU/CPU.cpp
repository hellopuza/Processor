/*------------------------------------------------------------------------------
    * File:        CPU.cpp                                                     *
    * Description: Functions to executing binary programs                      *
    * Created:     7 feb 2021                                                  *
    * Copyright:   (C) 2021 MIPT                                               *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    *///------------------------------------------------------------------------

#include "CPU.h"

//------------------------------------------------------------------------------

int CPUConstruct(cpu_t* p_cpu, const char* filename)
{
    assert(filename != nullptr);

    CPU_ASSERTOK((p_cpu == nullptr), CPU_NULL_INPUT_CPU_PTR, 0, {}, 0);
    CPU_ASSERTOK(((p_cpu->state != CPU_NOT_CONSTRUCTED) && (p_cpu->state != CPU_DESTRUCTED)), CPU_CONSTRUCTED, 0, {}, 0);

    int err = 0;

    err = fillinBCodeStruct(&p_cpu->bcode, filename);
    CPU_ASSERTOK(err, err, 0, {});

    TEMPLATE(_StackConstruct, NUM_TYPE) (&p_cpu->stkCPU_NUM, DEFAULT_STACK_CAPACITY, (char*)"stkCPU_NUM");
    TEMPLATE(_StackConstruct, PTR_TYPE) (&p_cpu->stkCPU_PTR, DEFAULT_STACK_CAPACITY, (char*)"stkCPU_PTR");

    for (int i = 0; i < REG_NUM; ++i)
    {
        p_cpu->registers[i] = TEMPLATE(NUM_TYPE, POISON);
    }

    p_cpu->state = CPU_CONSTRUCTED;

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
        char cond = 0;

        cmd = p_cpu->bcode.data[p_cpu->bcode.ptr++];

        switch (cmd)
        {
        case CMD_HLT:

            return PROCESS_HALT;
            break;

        case CMD_PUSH | NUM_FLAG:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < NUMBER_SIZE), CPU_NO_SPACE_FOR_NUMBER, 1, p_cpu);

            num = *(NUM_TYPE*)(p_cpu->bcode.data + p_cpu->bcode.ptr);
            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, num);
            p_cpu->bcode.ptr += NUMBER_SIZE;
            break;

        case CMD_PUSH | REG_FLAG:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < 1), CPU_NO_SPACE_FOR_REGISTER, 1, p_cpu);

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            CPU_ASSERTOK((reg > REG_NUM), CPU_UNIDENTIFIED_REGISTER, 1, p_cpu);
            CPU_ASSERTOK((TEMPLATE(isPOISON, NUM_TYPE) (p_cpu->registers[reg - 1])), CPU_EMPTY_REGISTER, 1, p_cpu);

            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, p_cpu->registers[reg - 1]);
            break;

        case CMD_POP:

            Pop1Number(p_cpu, &num);
            break;

        case CMD_POP | REG_FLAG:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < 1), CPU_NO_SPACE_FOR_REGISTER, 1, p_cpu);

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            CPU_ASSERTOK((reg > REG_NUM), CPU_UNIDENTIFIED_REGISTER, 1, p_cpu);

            Pop1Number(p_cpu, &num);

            p_cpu->registers[reg - 1] = num;
            break;

        case CMD_ADD:

            Pop2Numbers(p_cpu, &num1, &num2);
            
            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, num2 + num1);
            break;

        case CMD_SUB:

            Pop2Numbers(p_cpu, &num1, &num2);

            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, num2 - num1);
            break;

        case CMD_MUL:

            Pop2Numbers(p_cpu, &num1, &num2);

            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, num2 * num1);
            break;

        case CMD_DIV:

            Pop2Numbers(p_cpu, &num1, &num2);
            
            CPU_ASSERTOK((fabs(num1) < NIL), CPU_DIVISION_BY_ZERO, 0, {});

            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, num2 / num1);
            break;

        case CMD_NEG:

            Pop1Number(p_cpu, &num);

            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, -num);
            break;

        case CMD_SIN:

            Pop1Number(p_cpu, &num);

            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, (NUM_TYPE)sin(num));
            break;

        case CMD_COS:

            Pop1Number(p_cpu, &num);

            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, (NUM_TYPE)cos(num));
            break;

        case CMD_SQRT:

            Pop1Number(p_cpu, &num);

            CPU_ASSERTOK((num < 0), CPU_ROOT_OF_A_NEG_NUMBER, 0, {});

            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, (NUM_TYPE)sqrt(num));
            break;

        case CMD_IN:

            printf("IN: ");
            CPU_ASSERTOK((scanf(TEMPLATE(NUM_TYPE, PRINT_FORMAT), &num) != 1), CPU_INCORRECT_INPUT, 0, {});

            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, num);
            break;

        case CMD_IN | REG_FLAG:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < 1), CPU_NO_SPACE_FOR_REGISTER, 1, p_cpu);

            printf("IN: ");
            CPU_ASSERTOK((scanf(TEMPLATE(NUM_TYPE, PRINT_FORMAT), &num) != 1), CPU_INCORRECT_INPUT, 0, {});

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            CPU_ASSERTOK((reg > REG_NUM), CPU_UNIDENTIFIED_REGISTER, 1, p_cpu);

            p_cpu->registers[reg - 1] = num;
            break;

        case CMD_OUT:

            Pop1Number(p_cpu, &num);

            TEMPLATE(StackPush, NUM_TYPE) (&p_cpu->stkCPU_NUM, num);

            printf("OUT: " TEMPLATE(NUM_TYPE, PRINT_FORMAT) "\n", num);
            break;

        case CMD_JMP:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < POINTER_SIZE), CPU_NO_SPACE_FOR_POINTER, 1, p_cpu);

            p_cpu->bcode.ptr = *(ptr_t*)(p_cpu->bcode.data + p_cpu->bcode.ptr);
            break;

        case CMD_JE:
        case CMD_JNE:
        case CMD_JA:
        case CMD_JAE:
        case CMD_JB:
        case CMD_JBE:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < POINTER_SIZE), CPU_NO_SPACE_FOR_POINTER, 1, p_cpu);
            Pop2Numbers(p_cpu, &num1, &num2);
            
            if (cmd == CMD_JE)  cond = (num2 == num1);
            if (cmd == CMD_JNE) cond = (num2 != num1);
            if (cmd == CMD_JA)  cond = (num2 >  num1);
            if (cmd == CMD_JAE) cond = (num2 >= num1);
            if (cmd == CMD_JB)  cond = (num2 <  num1);
            if (cmd == CMD_JBE) cond = (num2 <= num1);
            if (cond)
                p_cpu->bcode.ptr = *(ptr_t*)(p_cpu->bcode.data + p_cpu->bcode.ptr);
            else
                p_cpu->bcode.ptr += POINTER_SIZE;
            break;

        case CMD_CALL:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < POINTER_SIZE), CPU_NO_SPACE_FOR_POINTER, 1, p_cpu);

            TEMPLATE(StackPush, PTR_TYPE) (&p_cpu->stkCPU_PTR, (PTR_TYPE)(p_cpu->bcode.ptr + POINTER_SIZE));
            p_cpu->bcode.ptr = *(ptr_t*)(p_cpu->bcode.data + p_cpu->bcode.ptr);
            break;

        case CMD_RET:

            ptr = TEMPLATE(StackPop, PTR_TYPE) (&p_cpu->stkCPU_PTR);
            CPU_ASSERTOK((TEMPLATE(isPOISON, PTR_TYPE) (ptr)), CPU_NO_RET_ADDRESS, 1, p_cpu);

            p_cpu->bcode.ptr = ptr;
            break;

        default:

            CPU_ASSERTOK(1, CPU_UNIDENTIFIED_COMMAND, 1, p_cpu);

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

    p_cpu->state = CPU_DESTRUCTED;

    return OK;
}

//------------------------------------------------------------------------------

void printCode(cpu_t* p_cpu, const char* logname, int err)
{
    assert(p_cpu   != nullptr);
    assert(logname != nullptr);

    FILE* log = fopen(logname, "a");
    assert(log != nullptr);

    fprintf(log, " Address: %08X\n\n", p_cpu->bcode.ptr);
    printf (     " Address: %08X\n\n", p_cpu->bcode.ptr);

    fprintf(log, "//////////////////////--CODE--//////////////////////" "\n");
    printf (     "//////////////////////--CODE--//////////////////////" "\n");

    fprintf(log, "     Address ");
    printf (     "     Address ");
    for (char i = 0; i < 0x10; ++i)
    {
        fprintf(log, "| %X ", i);
        printf (     "| %X ", i);
    }
    fprintf(log, "\n");
    printf (     "\n");

    size_t line = p_cpu->bcode.ptr - (p_cpu->bcode.ptr % 0x10);
    size_t last_line = p_cpu->bcode.size - (p_cpu->bcode.size % 0x10);

    for (char i = -0x20; i <= 0x20; i += 0x10)
    {
        if ((line + i >= 0) && (line + i <= last_line))
        {
            fprintf(log, "%s%s%08X ", ((i == 0)? "=>" : "  "), "   ", line + i);
            printf (     "%s%s%08X ", ((i == 0)? "=>" : "  "), "   ", line + i);

            for (char b = 0; b < 0x10; ++b)
            {
                if (line + i + b == p_cpu->bcode.size) break;

                fprintf(log, "%02X  ", (unsigned char)p_cpu->bcode.data[line + i + b]);
                printf (     "%02X  ", (unsigned char)p_cpu->bcode.data[line + i + b]);
            }

            fprintf(log, "\n");
            printf (     "\n");
        }
    }

    for (int i = 0; i < 14 + 4*(p_cpu->bcode.ptr % 0x10); ++i)
    {
        fprintf(log, "=");
        printf (     "=");
    }
    fprintf(log, "/\\\n");
    printf (     "/\\\n");

    fprintf(log, "////////////////////////////////////////////////////" "\n\n");
    printf (     "////////////////////////////////////////////////////" "\n\n");

    fclose(log);
}

//------------------------------------------------------------------------------

void Pop1Number(cpu_t* p_cpu, NUM_TYPE* num)
{
    assert(p_cpu != nullptr);
    assert(num   != nullptr);

    *num = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
    CPU_ASSERTOK((TEMPLATE(isPOISON, NUM_TYPE) (*num)), STACK_EMPTY_STACK, 1, p_cpu);
}

//------------------------------------------------------------------------------

void Pop2Numbers(cpu_t* p_cpu, NUM_TYPE* num1, NUM_TYPE* num2)
{
    assert(p_cpu != nullptr);
    assert(num1  != nullptr);
    assert(num2  != nullptr);

    *num1 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
    CPU_ASSERTOK((TEMPLATE(isPOISON, NUM_TYPE) (*num1)), STACK_EMPTY_STACK, 1, p_cpu);

    *num2 = TEMPLATE(StackPop, NUM_TYPE) (&p_cpu->stkCPU_NUM);
    CPU_ASSERTOK((TEMPLATE(isPOISON, NUM_TYPE) (*num2)), STACK_EMPTY_STACK, 1, p_cpu);
}

//------------------------------------------------------------------------------
