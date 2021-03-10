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

    TEMPLATE(_StackConstruct, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT, DEFAULT_STACK_CAPACITY, (char*)"stkCPU_NUM_INT");
    TEMPLATE(_StackConstruct, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, DEFAULT_STACK_CAPACITY, (char*)"stkCPU_NUM_FLT");

    TEMPLATE(_StackConstruct, PTR_TYPE) (&p_cpu->stkCPU_PTR, DEFAULT_STACK_CAPACITY, (char*)"stkCPU_PTR");

    for (int i = 0; i < REG_NUM; ++i)
    {
        p_cpu->registers[i] = TEMPLATE(NUM_FLT_TYPE, POISON);
    }

    p_cpu->RAM = (char*)calloc(RAM_SIZE, 1);
    CPU_ASSERTOK((p_cpu->RAM == nullptr), NO_MEMORY, 0, {}, 0);

    p_cpu->state = CPU_CONSTRUCTED;

    return OK;
}

//------------------------------------------------------------------------------

int Execute(cpu_t* p_cpu, char* filename)
{
    assert(p_cpu != nullptr);
    assert(filename != nullptr);
    assert(p_cpu->bcode.size != 0);

    p_cpu->bcode.ptr = 0;

    unsigned char cmd = 0;
    char reg = 0;

    int width  = 0;
    int height = 0;

    PTR_TYPE ptr = TEMPLATE(PTR_TYPE, POISON);

    NUM_INT_TYPE num_int1 = TEMPLATE(NUM_INT_TYPE, POISON);
    NUM_INT_TYPE num_int2 = TEMPLATE(NUM_INT_TYPE, POISON);

    NUM_FLT_TYPE num_flt1 = TEMPLATE(NUM_FLT_TYPE, POISON);
    NUM_FLT_TYPE num_flt2 = TEMPLATE(NUM_FLT_TYPE, POISON);

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

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < NUMBER_INT_SIZE), CPU_NO_SPACE_FOR_NUMBER_INT, 1, p_cpu);

            num_int1 = *(NUM_INT_TYPE*)(p_cpu->bcode.data + p_cpu->bcode.ptr);
            TEMPLATE(StackPush, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT, num_int1);
            p_cpu->bcode.ptr += NUMBER_INT_SIZE;
            break;

        case CMD_PUSHQ | NUM_FLAG:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < NUMBER_FLT_SIZE), CPU_NO_SPACE_FOR_NUMBER_FLT, 1, p_cpu);

            num_flt1 = *(NUM_FLT_TYPE*)(p_cpu->bcode.data + p_cpu->bcode.ptr);
            TEMPLATE(StackPush, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, num_flt1);
            p_cpu->bcode.ptr += NUMBER_FLT_SIZE;
            break;

        case CMD_PUSH  | REG_FLAG:
        case CMD_PUSHQ | REG_FLAG:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < 1), CPU_NO_SPACE_FOR_REGISTER, 1, p_cpu);

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            CPU_ASSERTOK(((reg > REG_NUM) || (reg == 0)), CPU_UNIDENTIFIED_REGISTER, 1, p_cpu);
            CPU_ASSERTOK((TEMPLATE(isPOISON, NUM_FLT_TYPE) (p_cpu->registers[reg - 1])), CPU_EMPTY_REGISTER, 1, p_cpu);

            if (cmd == (CMD_PUSH | REG_FLAG))
                TEMPLATE(StackPush, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT, (NUM_INT_TYPE)p_cpu->registers[reg - 1]); else
            if (cmd == (CMD_PUSHQ | REG_FLAG))
                TEMPLATE(StackPush, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, (NUM_FLT_TYPE)p_cpu->registers[reg - 1]);
            break;

        case CMD_PUSH  | PTR_FLAG | NUM_FLAG:
        case CMD_PUSHQ | PTR_FLAG | NUM_FLAG:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < POINTER_SIZE), CPU_NO_SPACE_FOR_POINTER, 1, p_cpu);

            ptr = *(PTR_TYPE*)(p_cpu->bcode.data + p_cpu->bcode.ptr);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, 1, p_cpu);
            p_cpu->bcode.ptr += POINTER_SIZE;

            if (cmd == (CMD_PUSH | PTR_FLAG | NUM_FLAG))
                TEMPLATE(StackPush, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT, *(NUM_INT_TYPE*)(p_cpu->RAM + ptr)); else
            if (cmd == (CMD_PUSHQ | PTR_FLAG | NUM_FLAG))
                TEMPLATE(StackPush, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, *(NUM_FLT_TYPE*)(p_cpu->RAM + ptr));
            break;

        case CMD_PUSH  | PTR_FLAG | REG_FLAG:
        case CMD_PUSHQ | PTR_FLAG | REG_FLAG:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < 1), CPU_NO_SPACE_FOR_REGISTER, 1, p_cpu);

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            CPU_ASSERTOK(((reg > REG_NUM) || (reg == 0)), CPU_UNIDENTIFIED_REGISTER, 1, p_cpu);
            CPU_ASSERTOK((TEMPLATE(isPOISON, NUM_FLT_TYPE) (p_cpu->registers[reg - 1])), CPU_EMPTY_REGISTER, 1, p_cpu);

            ptr = (PTR_TYPE)p_cpu->registers[reg - 1];
            CPU_ASSERTOK((TEMPLATE(isPOISON, PTR_TYPE) (ptr)), CPU_EMPTY_REGISTER, 1, p_cpu);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, 1, p_cpu);

            if (cmd == (CMD_PUSH | PTR_FLAG | REG_FLAG))
                TEMPLATE(StackPush, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT, *(NUM_INT_TYPE*)(p_cpu->RAM + ptr)); else
            if (cmd == (CMD_PUSHQ | PTR_FLAG | REG_FLAG))
                TEMPLATE(StackPush, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, *(NUM_FLT_TYPE*)(p_cpu->RAM + ptr));
            break;

        case CMD_PUSH  | PTR_FLAG | REG_FLAG | NUM_FLAG:
        case CMD_PUSHQ | PTR_FLAG | REG_FLAG | NUM_FLAG:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < 1 + NUMBER_INT_SIZE), CPU_NO_SPACE_FOR_POINTER, 1, p_cpu);

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            CPU_ASSERTOK(((reg > REG_NUM) || (reg == 0)), CPU_UNIDENTIFIED_REGISTER, 1, p_cpu);
            CPU_ASSERTOK((TEMPLATE(isPOISON, NUM_FLT_TYPE) (p_cpu->registers[reg - 1])), CPU_EMPTY_REGISTER, 1, p_cpu);

            ptr = (PTR_TYPE)p_cpu->registers[reg - 1];
            CPU_ASSERTOK((TEMPLATE(isPOISON, PTR_TYPE) (ptr)), CPU_EMPTY_REGISTER, 1, p_cpu);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, 1, p_cpu);

            ptr += *(NUM_INT_TYPE*)(p_cpu->bcode.data + p_cpu->bcode.ptr);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, 1, p_cpu);
            p_cpu->bcode.ptr += NUMBER_INT_SIZE;

            if (cmd == (CMD_PUSH | PTR_FLAG | NUM_FLAG | REG_FLAG))
                TEMPLATE(StackPush, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT, *(NUM_INT_TYPE*)(p_cpu->RAM + ptr)); else
            if (cmd == (CMD_PUSHQ | PTR_FLAG | NUM_FLAG | REG_FLAG))
                TEMPLATE(StackPush, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, *(NUM_FLT_TYPE*)(p_cpu->RAM + ptr));

            break;

        case CMD_POP:

            Pop1IntNumber(p_cpu, &num_int1);
            break;

        case CMD_POPQ:

            Pop1FloatNumber(p_cpu, &num_flt1);
            break;

        case CMD_POP  | REG_FLAG:
        case CMD_POPQ | REG_FLAG:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < 1), CPU_NO_SPACE_FOR_REGISTER, 1, p_cpu);

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            CPU_ASSERTOK((reg > REG_NUM), CPU_UNIDENTIFIED_REGISTER, 1, p_cpu);

            if (cmd == (CMD_POP | REG_FLAG))
            {
                Pop1IntNumber(p_cpu, &num_int1);
                p_cpu->registers[reg - 1] = num_int1;
            }
            else if (cmd == (CMD_POPQ | REG_FLAG))
            {
                Pop1FloatNumber(p_cpu, &num_flt1);
                p_cpu->registers[reg - 1] = num_flt1;
            }
            break;

        case CMD_POP  | PTR_FLAG | NUM_FLAG:
        case CMD_POPQ | PTR_FLAG | NUM_FLAG:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < POINTER_SIZE), CPU_NO_SPACE_FOR_POINTER, 1, p_cpu);

            ptr = *(PTR_TYPE*)(p_cpu->bcode.data + p_cpu->bcode.ptr);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, 1, p_cpu);
            p_cpu->bcode.ptr += POINTER_SIZE;

            if (cmd == (CMD_POP | PTR_FLAG | NUM_FLAG))
            {
                Pop1IntNumber(p_cpu, &num_int1);
                *(NUM_INT_TYPE*)(p_cpu->RAM + ptr) = num_int1;
            }
            else if (cmd == (CMD_POPQ | PTR_FLAG | NUM_FLAG))
            {
                Pop1FloatNumber(p_cpu, &num_flt1);
                *(NUM_FLT_TYPE*)(p_cpu->RAM + ptr) = num_flt1;
            }
            break;

        case CMD_POP  | PTR_FLAG | REG_FLAG:
        case CMD_POPQ | PTR_FLAG | REG_FLAG:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < 1), CPU_NO_SPACE_FOR_REGISTER, 1, p_cpu);

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            CPU_ASSERTOK(((reg > REG_NUM) || (reg == 0)), CPU_UNIDENTIFIED_REGISTER, 1, p_cpu);

            ptr = (PTR_TYPE)p_cpu->registers[reg - 1];
            CPU_ASSERTOK((TEMPLATE(isPOISON, PTR_TYPE) (ptr)), CPU_EMPTY_REGISTER, 1, p_cpu);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, 1, p_cpu);

            if (cmd == (CMD_POP | PTR_FLAG | REG_FLAG))
            {
                Pop1IntNumber(p_cpu, &num_int1);
                *(NUM_INT_TYPE*)(p_cpu->RAM + ptr) = num_int1;
            }
            else if (cmd == (CMD_POPQ | PTR_FLAG | REG_FLAG))
            {
                Pop1FloatNumber(p_cpu, &num_flt1);
                *(NUM_FLT_TYPE*)(p_cpu->RAM + ptr) = num_flt1;
            }
            break;

        case CMD_POP  | PTR_FLAG | REG_FLAG | NUM_FLAG:
        case CMD_POPQ | PTR_FLAG | REG_FLAG | NUM_FLAG:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < 1 + NUMBER_INT_SIZE), CPU_NO_SPACE_FOR_POINTER, 1, p_cpu);

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            CPU_ASSERTOK(((reg > REG_NUM) || (reg == 0)), CPU_UNIDENTIFIED_REGISTER, 1, p_cpu);

            ptr = (PTR_TYPE)p_cpu->registers[reg - 1];
            CPU_ASSERTOK((TEMPLATE(isPOISON, PTR_TYPE) (ptr)), CPU_EMPTY_REGISTER, 1, p_cpu);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, 1, p_cpu);

            ptr += *(NUM_INT_TYPE*)(p_cpu->bcode.data + p_cpu->bcode.ptr);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, 1, p_cpu);
            p_cpu->bcode.ptr += NUMBER_INT_SIZE;

            if (cmd == (CMD_POP | PTR_FLAG | REG_FLAG | NUM_FLAG))
            {
                Pop1IntNumber(p_cpu, &num_int1);
                *(NUM_INT_TYPE*)(p_cpu->RAM + ptr) = num_int1;
            }
            else if (cmd == (CMD_POPQ | PTR_FLAG | REG_FLAG | NUM_FLAG))
            {
                Pop1FloatNumber(p_cpu, &num_flt1);
                *(NUM_FLT_TYPE*)(p_cpu->RAM + ptr) = num_flt1;
            }
            break;

        case CMD_IN:

            printf("IN: ");
            CPU_ASSERTOK((scanf(TEMPLATE(NUM_INT_TYPE, PRINT_FORMAT), &num_int1) != 1), CPU_INCORRECT_INPUT, 0, {});
            TEMPLATE(StackPush, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT, num_int1);
            break;

        case CMD_INQ:

            printf("IN: ");
            CPU_ASSERTOK((scanf(TEMPLATE(NUM_FLT_TYPE, PRINT_FORMAT), &num_flt1) != 1), CPU_INCORRECT_INPUT, 0, {});
            TEMPLATE(StackPush, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, num_flt1);
            break;

        case CMD_IN  | REG_FLAG:
        case CMD_INQ | REG_FLAG:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < 1), CPU_NO_SPACE_FOR_REGISTER, 1, p_cpu);

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            CPU_ASSERTOK(((reg > REG_NUM) || (reg == 0)), CPU_UNIDENTIFIED_REGISTER, 1, p_cpu);

            printf("IN: ");

            if (cmd == (CMD_IN | REG_FLAG))
            {
                CPU_ASSERTOK((scanf(TEMPLATE(NUM_INT_TYPE, PRINT_FORMAT), &num_int1) != 1), CPU_INCORRECT_INPUT, 0, {});
                p_cpu->registers[reg - 1] = num_int1;
            }
            else if (cmd == (CMD_INQ | REG_FLAG))
            {
                CPU_ASSERTOK((scanf(TEMPLATE(NUM_FLT_TYPE, PRINT_FORMAT), &num_flt1) != 1), CPU_INCORRECT_INPUT, 0, {});
                p_cpu->registers[reg - 1] = num_flt1;
            }
            break;

        case CMD_OUT:

            Pop1IntNumber(p_cpu, &num_int1);
            TEMPLATE(StackPush, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT, num_int1);
            printf("OUT: " TEMPLATE(NUM_INT_TYPE, PRINT_FORMAT) "\n", num_int1);
            break;

        case CMD_OUTQ:

            Pop1FloatNumber(p_cpu, &num_flt1);
            TEMPLATE(StackPush, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, num_flt1);
            printf("OUT: " TEMPLATE(NUM_FLT_TYPE, PRINT_FORMAT) "\n", num_flt1);
            break;

        case CMD_OUT  | REG_FLAG:
        case CMD_OUTQ | REG_FLAG:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < 1), CPU_NO_SPACE_FOR_REGISTER, 1, p_cpu);

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            CPU_ASSERTOK(((reg > REG_NUM) || (reg == 0)), CPU_UNIDENTIFIED_REGISTER, 1, p_cpu);

            if (cmd == (CMD_OUT | REG_FLAG))
                printf("OUT: " TEMPLATE(NUM_INT_TYPE, PRINT_FORMAT) "\n", p_cpu->registers[reg - 1]); else
            if (cmd == (CMD_OUTQ | REG_FLAG))
                printf("OUT: " TEMPLATE(NUM_FLT_TYPE, PRINT_FORMAT) "\n", p_cpu->registers[reg - 1]);
            break;

        case CMD_ADD:

            Pop2IntNumbers(p_cpu, &num_int1, &num_int2);
            TEMPLATE(StackPush, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT, num_int2 + num_int1);
            break;

        case CMD_ADDQ:

            Pop2FloatNumbers(p_cpu, &num_flt1, &num_flt2);
            TEMPLATE(StackPush, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, num_flt2 + num_flt1);
            break;

        case CMD_SUB:

            Pop2IntNumbers(p_cpu, &num_int1, &num_int2);
            TEMPLATE(StackPush, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT, num_int2 - num_int1);
            break;

        case CMD_SUBQ:

            Pop2FloatNumbers(p_cpu, &num_flt1, &num_flt2);
            TEMPLATE(StackPush, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, num_flt2 - num_flt1);
            break;

        case CMD_MUL:

            Pop2IntNumbers(p_cpu, &num_int1, &num_int2);
            TEMPLATE(StackPush, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT, num_int2 * num_int1);
            break;

        case CMD_MULQ:

            Pop2FloatNumbers(p_cpu, &num_flt1, &num_flt2);
            TEMPLATE(StackPush, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, num_flt2 * num_flt1);
            break;

        case CMD_DIV:

            Pop2IntNumbers(p_cpu, &num_int1, &num_int2);
            CPU_ASSERTOK((fabs(num_int1) < NIL), CPU_DIVISION_BY_ZERO, 1, p_cpu);
            TEMPLATE(StackPush, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT, num_int2 / num_int1);
            break;

        case CMD_DIVQ:

            Pop2FloatNumbers(p_cpu, &num_flt1, &num_flt2);
            CPU_ASSERTOK((fabs(num_flt1) < NIL), CPU_DIVISION_BY_ZERO, 1, p_cpu);
            TEMPLATE(StackPush, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, num_flt2 / num_flt1);
            break;

        case CMD_NEG:

            Pop1IntNumber(p_cpu, &num_int1);
            TEMPLATE(StackPush, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT, -num_int1);
            break;

        case CMD_NEGQ:

            Pop1FloatNumber(p_cpu, &num_flt1);
            TEMPLATE(StackPush, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, -num_flt1);
            break;

        case CMD_SIN:

            Pop1FloatNumber(p_cpu, &num_flt1);
            TEMPLATE(StackPush, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, sin(num_flt1));
            break;

        case CMD_COS:

            Pop1FloatNumber(p_cpu, &num_flt1);
            TEMPLATE(StackPush, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, cos(num_flt1));
            break;

        case CMD_SQRT:

            Pop1FloatNumber(p_cpu, &num_flt1);
            CPU_ASSERTOK((num_flt1 < 0), CPU_ROOT_OF_A_NEG_NUMBER, 1, p_cpu);
            TEMPLATE(StackPush, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, sqrt(num_flt1));
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
            Pop2FloatNumbers(p_cpu, &num_flt1, &num_flt2);
            
            if (cmd == CMD_JE ) cond = (num_flt1 == num_flt2); else
            if (cmd == CMD_JNE) cond = (num_flt1 != num_flt2); else
            if (cmd == CMD_JA ) cond = (num_flt1 >  num_flt2); else
            if (cmd == CMD_JAE) cond = (num_flt1 >= num_flt2); else
            if (cmd == CMD_JB ) cond = (num_flt1 <  num_flt2); else
            if (cmd == CMD_JBE) cond = (num_flt1 <= num_flt2);
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

        case CMD_FLT2INT:

            Pop1FloatNumber(p_cpu, &num_flt1);
            TEMPLATE(StackPush, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT, (NUM_INT_TYPE)num_flt1);
            break;

        case CMD_INT2FLT:

            Pop1IntNumber(p_cpu, &num_int1);
            TEMPLATE(StackPush, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT, (NUM_FLT_TYPE)num_int1);
            break;

        case CMD_SCREEN | REG_FLAG:

            CPU_ASSERTOK((p_cpu->bcode.size - p_cpu->bcode.ptr < 1), CPU_NO_SPACE_FOR_REGISTER, 1, p_cpu);

            reg = p_cpu->bcode.data[p_cpu->bcode.ptr++];
            CPU_ASSERTOK(((reg > REG_NUM) || (reg == 0)), CPU_UNIDENTIFIED_REGISTER, 1, p_cpu);
            CPU_ASSERTOK((TEMPLATE(isPOISON, NUM_FLT_TYPE) (p_cpu->registers[reg - 1])), CPU_EMPTY_REGISTER, 1, p_cpu);

            ptr = (PTR_TYPE)p_cpu->registers[reg - 1];
            CPU_ASSERTOK((TEMPLATE(isPOISON, PTR_TYPE) (ptr)), CPU_EMPTY_REGISTER, 1, p_cpu);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, 1, p_cpu);

            CPU_ASSERTOK((TEMPLATE(isPOISON, NUM_FLT_TYPE) (p_cpu->registers[REG_SCRX - 1])), CPU_EMPTY_REGISTER, 1, p_cpu);
            CPU_ASSERTOK((TEMPLATE(isPOISON, NUM_FLT_TYPE) (p_cpu->registers[REG_SCRY - 1])), CPU_EMPTY_REGISTER, 1, p_cpu);

            width  = (int)(p_cpu->registers[REG_SCRX - 1]);
            height = (int)(p_cpu->registers[REG_SCRY - 1]);
            CPU_ASSERTOK(((width <= 0) || (height <= 0)), CPU_INCORRECT_WINDOW_SIZES, 0, {});
            CPU_ASSERTOK((ptr + width * height * PIXEL_SIZE > RAM_SIZE), CPU_NO_VIDEO_MEMORY, 1, p_cpu);

            txCreateWindow(width, height);

            for (int y = 0; y < height; ++y)
            for (int x = 0; x < width;  ++x)
            {
                txSetPixel(x, y, RGB(p_cpu->RAM[ptr + (y * width + x) * PIXEL_SIZE + 0],
                                     p_cpu->RAM[ptr + (y * width + x) * PIXEL_SIZE + 1],
                                     p_cpu->RAM[ptr + (y * width + x) * PIXEL_SIZE + 2] ));
            }

            txSaveImage("picture.bmp");
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
    TEMPLATE(StackDestruct, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT);
    TEMPLATE(StackDestruct, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT);

    TEMPLATE(StackDestruct, PTR_TYPE) (&p_cpu->stkCPU_PTR);

    for (int i = 0; i < REG_NUM; ++i)
    {
        p_cpu->registers[i] = TEMPLATE(NUM_FLT_TYPE, POISON);
    }

    free(p_cpu->RAM);

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

    --p_cpu->bcode.ptr;

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

void Pop1IntNumber (cpu_t* p_cpu, NUM_INT_TYPE* num)
{
    assert(p_cpu != nullptr);
    assert(num   != nullptr);

    *num = TEMPLATE(StackPop, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT);
    CPU_ASSERTOK((TEMPLATE(isPOISON, NUM_INT_TYPE) (*num)), STACK_EMPTY_STACK, 1, p_cpu);
}

//------------------------------------------------------------------------------

void Pop2IntNumbers (cpu_t* p_cpu, NUM_INT_TYPE* num1, NUM_INT_TYPE* num2)
{
    assert(p_cpu != nullptr);
    assert(num1  != nullptr);
    assert(num2  != nullptr);

    *num1 = TEMPLATE(StackPop, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT);
    CPU_ASSERTOK((TEMPLATE(isPOISON, NUM_INT_TYPE) (*num1)), STACK_EMPTY_STACK, 1, p_cpu);

    *num2 = TEMPLATE(StackPop, NUM_INT_TYPE) (&p_cpu->stkCPU_NUM_INT);
    CPU_ASSERTOK((TEMPLATE(isPOISON, NUM_INT_TYPE) (*num2)), STACK_EMPTY_STACK, 1, p_cpu);
}

//------------------------------------------------------------------------------

void Pop1FloatNumber (cpu_t* p_cpu, NUM_FLT_TYPE* num)
{
    assert(p_cpu != nullptr);
    assert(num   != nullptr);

    *num = TEMPLATE(StackPop, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT);
    CPU_ASSERTOK((TEMPLATE(isPOISON, NUM_FLT_TYPE) (*num)), STACK_EMPTY_STACK, 1, p_cpu);
}

//------------------------------------------------------------------------------

void Pop2FloatNumbers (cpu_t* p_cpu, NUM_FLT_TYPE* num1, NUM_FLT_TYPE* num2)
{
    assert(p_cpu != nullptr);
    assert(num1  != nullptr);
    assert(num2  != nullptr);

    *num1 = TEMPLATE(StackPop, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT);
    CPU_ASSERTOK((TEMPLATE(isPOISON, NUM_FLT_TYPE) (*num1)), STACK_EMPTY_STACK, 1, p_cpu);

    *num2 = TEMPLATE(StackPop, NUM_FLT_TYPE) (&p_cpu->stkCPU_NUM_FLT);
    CPU_ASSERTOK((TEMPLATE(isPOISON, NUM_FLT_TYPE) (*num2)), STACK_EMPTY_STACK, 1, p_cpu);
}
