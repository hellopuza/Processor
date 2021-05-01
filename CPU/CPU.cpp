/*------------------------------------------------------------------------------
    * File:        CPU.cpp                                                     *
    * Description: Functions to executing binary programs                      *
    * Created:     7 feb 2021                                                  *
    * Author:      Artem Puzankov                                              *
    * Email:       puzankov.ao@phystech.edu                                    *
    * GitHub:      https://github.com/hellopuza                                *
    * Copyright © 2021 Artem Puzankov. All rights reserved.                    *
    *///------------------------------------------------------------------------

#include "CPU.h"

//------------------------------------------------------------------------------

CPU::CPU (char* filename) : 
    bcode_      (filename),
    filename_   (filename),
    stkCPU_INT_ ((char*)"stkCPU_INT_", DEFAULT_STACK_CAPACITY),
    stkCPU_FLT_ ((char*)"stkCPU_FLT_", DEFAULT_STACK_CAPACITY),
    stkCPU_PTR_ ((char*)"stkCPU_PTR_", DEFAULT_STACK_CAPACITY),
    state_      (CPU_OK)
{
    for (int i = 0; i < REG_NUM; ++i)
    {
        registers_[i] = POISON<FLT_TYPE>;
    }

    RAM_ = (char*)calloc(RAM_SIZE, 1);
    CPU_ASSERTOK((RAM_ == nullptr), CPU_NO_MEMORY, nullptr);
}

//------------------------------------------------------------------------------

CPU::~CPU ()
{
    CPU_ASSERTOK((this == nullptr),          CPU_NULL_INPUT_CPU_PTR, nullptr);
    CPU_ASSERTOK((state_ == CPU_DESTRUCTED), CPU_DESTRUCTED,         nullptr);

    bcode_.~BinCode();

    for (int i = 0; i < REG_NUM; ++i)
    {
        registers_[i] = POISON<FLT_TYPE>;
    }

    free(RAM_);

    state_ = CPU_DESTRUCTED;
}

//------------------------------------------------------------------------------

int CPU::Execute ()
{
    CPU_ASSERTOK((this == nullptr), CPU_NULL_INPUT_CPU_PTR, nullptr);

    bcode_.ptr_ = 0;

    char reg_code = 0;

    int width  = 0;
    int height = 0;
    char pictname[30] = "";

    PTR_TYPE ptr = POISON<PTR_TYPE>;

    INT_TYPE num_int1 = POISON<INT_TYPE>;
    INT_TYPE num_int2 = POISON<INT_TYPE>;

    FLT_TYPE num_flt1 = POISON<FLT_TYPE>;
    FLT_TYPE num_flt2 = POISON<FLT_TYPE>;

    while (bcode_.ptr_ < bcode_.size_)
    {
        char cond = 0;
        char scrnumstr[5] = "";

        unsigned char cmd_code = bcode_.data_[bcode_.ptr_++];

        switch (cmd_code)
        {
        case CMD_HLT:
            
            return PROCESS_HALT;
            break;

        case CMD_PUSH | NUM_FLAG:

            CPU_ASSERTOK((bcode_.size_ - bcode_.ptr_ < NUMBER_INT_SIZE), CPU_NO_SPACE_FOR_NUMBER_INT, this);

            num_int1 = *(INT_TYPE*)(bcode_.data_ + bcode_.ptr_);
            stkCPU_INT_.Push(num_int1);
            bcode_.ptr_ += NUMBER_INT_SIZE;
            break;

        case CMD_PUSHQ | NUM_FLAG:

            CPU_ASSERTOK((bcode_.size_ - bcode_.ptr_ < NUMBER_FLT_SIZE), CPU_NO_SPACE_FOR_NUMBER_FLT, this);

            num_flt1 = *(FLT_TYPE*)(bcode_.data_ + bcode_.ptr_);
            stkCPU_FLT_.Push(num_flt1);
            bcode_.ptr_ += NUMBER_FLT_SIZE;
            break;

        case CMD_PUSH  | REG_FLAG:
        case CMD_PUSHQ | REG_FLAG:

            CPU_ASSERTOK((bcode_.size_ - bcode_.ptr_ < 1), CPU_NO_SPACE_FOR_REGISTER, this);

            reg_code = bcode_.data_[bcode_.ptr_++];
            CPU_ASSERTOK(((reg_code > REG_NUM) || (reg_code == 0)), CPU_UNIDENTIFIED_REGISTER, this);
            CPU_ASSERTOK(isPOISON(registers_[reg_code - 1]), CPU_EMPTY_REGISTER, this);

            if (cmd_code == (CMD_PUSH | REG_FLAG))
                stkCPU_INT_.Push((INT_TYPE)registers_[reg_code - 1]);
            else
            if (cmd_code == (CMD_PUSHQ | REG_FLAG))
                stkCPU_FLT_.Push((FLT_TYPE)registers_[reg_code - 1]);
            break;

        case CMD_PUSH  | PTR_FLAG | NUM_FLAG:
        case CMD_PUSHQ | PTR_FLAG | NUM_FLAG:

            CPU_ASSERTOK((bcode_.size_ - bcode_.ptr_ < POINTER_SIZE), CPU_NO_SPACE_FOR_POINTER, this);

            ptr = *(PTR_TYPE*)(bcode_.data_ + bcode_.ptr_);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, this);
            bcode_.ptr_ += POINTER_SIZE;

            if (cmd_code == (CMD_PUSH | PTR_FLAG | NUM_FLAG))
                stkCPU_INT_.Push(*(INT_TYPE*)(RAM_ + ptr));
            else
            if (cmd_code == (CMD_PUSHQ | PTR_FLAG | NUM_FLAG))
                stkCPU_FLT_.Push(*(FLT_TYPE*)(RAM_ + ptr));
            break;

        case CMD_PUSH  | PTR_FLAG | REG_FLAG:
        case CMD_PUSHQ | PTR_FLAG | REG_FLAG:

            CPU_ASSERTOK((bcode_.size_ - bcode_.ptr_ < 1), CPU_NO_SPACE_FOR_REGISTER, this);

            reg_code = bcode_.data_[bcode_.ptr_++];
            CPU_ASSERTOK(((reg_code > REG_NUM) || (reg_code == 0)), CPU_UNIDENTIFIED_REGISTER, this);
            CPU_ASSERTOK(isPOISON(registers_[reg_code - 1]), CPU_EMPTY_REGISTER, this);

            ptr = (PTR_TYPE)(int)registers_[reg_code - 1];
            CPU_ASSERTOK((isPOISON(ptr)), CPU_EMPTY_REGISTER, this);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, this);

            if (cmd_code == (CMD_PUSH | PTR_FLAG | REG_FLAG))
                stkCPU_INT_.Push(*(INT_TYPE*)(RAM_ + ptr));
            else
            if (cmd_code == (CMD_PUSHQ | PTR_FLAG | REG_FLAG))
                stkCPU_FLT_.Push(*(FLT_TYPE*)(RAM_ + ptr));
            break;

        case CMD_PUSH  | PTR_FLAG | REG_FLAG | NUM_FLAG:
        case CMD_PUSHQ | PTR_FLAG | REG_FLAG | NUM_FLAG:

            CPU_ASSERTOK((bcode_.size_ - bcode_.ptr_ < 1 + NUMBER_INT_SIZE), CPU_NO_SPACE_FOR_POINTER, this);

            reg_code = bcode_.data_[bcode_.ptr_++];
            CPU_ASSERTOK(((reg_code > REG_NUM) || (reg_code == 0)), CPU_UNIDENTIFIED_REGISTER, this);
            CPU_ASSERTOK(isPOISON(registers_[reg_code - 1]), CPU_EMPTY_REGISTER, this);

            ptr = (PTR_TYPE)(int)registers_[reg_code - 1];
            CPU_ASSERTOK((isPOISON(ptr)), CPU_EMPTY_REGISTER, this);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, this);
            
            ptr += *(INT_TYPE*)(bcode_.data_ + bcode_.ptr_);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, this);
            bcode_.ptr_ += NUMBER_INT_SIZE;

            if (cmd_code == (CMD_PUSH | PTR_FLAG | NUM_FLAG | REG_FLAG))
                stkCPU_INT_.Push(*(INT_TYPE*)(RAM_ + ptr));
            else
            if (cmd_code == (CMD_PUSHQ | PTR_FLAG | NUM_FLAG | REG_FLAG))
                stkCPU_FLT_.Push(*(FLT_TYPE*)(RAM_ + ptr));
            break;

        case CMD_POP:

            Pop1IntNumber(&num_int1);
            break;

        case CMD_POPQ:

            Pop1FloatNumber(&num_flt1);
            break;

        case CMD_POP  | REG_FLAG:
        case CMD_POPQ | REG_FLAG:

            CPU_ASSERTOK((bcode_.size_ - bcode_.ptr_ < 1), CPU_NO_SPACE_FOR_REGISTER, this);

            reg_code = bcode_.data_[bcode_.ptr_++];
            CPU_ASSERTOK((reg_code > REG_NUM), CPU_UNIDENTIFIED_REGISTER, this);

            if (cmd_code == (CMD_POP | REG_FLAG))
            {
                Pop1IntNumber(&num_int1);
                registers_[reg_code - 1] = num_int1;
            }
            else if (cmd_code == (CMD_POPQ | REG_FLAG))
            {
                Pop1FloatNumber(&num_flt1);
                registers_[reg_code - 1] = num_flt1;
            }
            break;

        case CMD_POP  | PTR_FLAG | NUM_FLAG:
        case CMD_POPQ | PTR_FLAG | NUM_FLAG:

            CPU_ASSERTOK((bcode_.size_ - bcode_.ptr_ < POINTER_SIZE), CPU_NO_SPACE_FOR_POINTER, this);

            ptr = *(PTR_TYPE*)(bcode_.data_ + bcode_.ptr_);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, this);
            bcode_.ptr_ += POINTER_SIZE;

            if (cmd_code == (CMD_POP | PTR_FLAG | NUM_FLAG))
            {
                Pop1IntNumber(&num_int1);
                *(INT_TYPE*)(RAM_ + ptr) = num_int1;
            }
            else if (cmd_code == (CMD_POPQ | PTR_FLAG | NUM_FLAG))
            {
                Pop1FloatNumber(&num_flt1);
                *(FLT_TYPE*)(RAM_ + ptr) = num_flt1;
            }
            break;

        case CMD_POP  | PTR_FLAG | REG_FLAG:
        case CMD_POPQ | PTR_FLAG | REG_FLAG:

            CPU_ASSERTOK((bcode_.size_ - bcode_.ptr_ < 1), CPU_NO_SPACE_FOR_REGISTER, this);

            reg_code = bcode_.data_[bcode_.ptr_++];
            CPU_ASSERTOK(((reg_code > REG_NUM) || (reg_code == 0)), CPU_UNIDENTIFIED_REGISTER, this);

            ptr = (PTR_TYPE)(int)registers_[reg_code - 1];
            CPU_ASSERTOK((isPOISON(ptr)), CPU_EMPTY_REGISTER, this);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, this);

            if (cmd_code == (CMD_POP | PTR_FLAG | REG_FLAG))
            {
                Pop1IntNumber(&num_int1);
                *(INT_TYPE*)(RAM_ + ptr) = num_int1;
            }
            else if (cmd_code == (CMD_POPQ | PTR_FLAG | REG_FLAG))
            {
                Pop1FloatNumber(&num_flt1);
                *(FLT_TYPE*)(RAM_ + ptr) = num_flt1;
            }
            break;

        case CMD_POP  | PTR_FLAG | REG_FLAG | NUM_FLAG:
        case CMD_POPQ | PTR_FLAG | REG_FLAG | NUM_FLAG:

            CPU_ASSERTOK((bcode_.size_ - bcode_.ptr_ < 1 + NUMBER_INT_SIZE), CPU_NO_SPACE_FOR_POINTER, this);

            reg_code = bcode_.data_[bcode_.ptr_++];
            CPU_ASSERTOK(((reg_code > REG_NUM) || (reg_code == 0)), CPU_UNIDENTIFIED_REGISTER, this);

            ptr = (PTR_TYPE)(int)registers_[reg_code - 1];
            CPU_ASSERTOK((isPOISON(ptr)), CPU_EMPTY_REGISTER, this);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, this);

            ptr += *(INT_TYPE*)(bcode_.data_ + bcode_.ptr_);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, this);
            bcode_.ptr_ += NUMBER_INT_SIZE;

            if (cmd_code == (CMD_POP | PTR_FLAG | REG_FLAG | NUM_FLAG))
            {
                Pop1IntNumber(&num_int1);
                *(INT_TYPE*)(RAM_ + ptr) = num_int1;
            }
            else if (cmd_code == (CMD_POPQ | PTR_FLAG | REG_FLAG | NUM_FLAG))
            {
                Pop1FloatNumber(&num_flt1);
                *(FLT_TYPE*)(RAM_ + ptr) = num_flt1;
            }
            break;

        case CMD_IN:

            printf("IN: ");
            CPU_ASSERTOK((scanf(PRINT_FORMAT<INT_TYPE>, &num_int1) != 1), CPU_INCORRECT_INPUT, nullptr);
            stkCPU_INT_.Push(num_int1);
            break;

        case CMD_INQ:

            printf("IN: ");
            CPU_ASSERTOK((scanf(PRINT_FORMAT<FLT_TYPE>, &num_flt1) != 1), CPU_INCORRECT_INPUT, nullptr);
            stkCPU_FLT_.Push(num_flt1);
            break;

        case CMD_IN  | REG_FLAG:
        case CMD_INQ | REG_FLAG:

            CPU_ASSERTOK((bcode_.size_ - bcode_.ptr_ < 1), CPU_NO_SPACE_FOR_REGISTER, this);

            reg_code = bcode_.data_[bcode_.ptr_++];
            CPU_ASSERTOK(((reg_code > REG_NUM) || (reg_code == 0)), CPU_UNIDENTIFIED_REGISTER, this);

            printf("IN: ");

            if (cmd_code == (CMD_IN | REG_FLAG))
            {
                CPU_ASSERTOK((scanf(PRINT_FORMAT<INT_TYPE>, &num_int1) != 1), CPU_INCORRECT_INPUT, nullptr);
                registers_[reg_code - 1] = num_int1;
            }
            else if (cmd_code == (CMD_INQ | REG_FLAG))
            {
                CPU_ASSERTOK((scanf(PRINT_FORMAT<FLT_TYPE>, &num_flt1) != 1), CPU_INCORRECT_INPUT, nullptr);
                registers_[reg_code - 1] = num_flt1;
            }
            break;

        case CMD_OUT:

            Pop1IntNumber(&num_int1);
            stkCPU_INT_.Push(num_int1);
            printf("OUT: ");
            printf(PRINT_FORMAT<INT_TYPE>, num_int1);
            printf("\n");
            break;

        case CMD_OUTQ:

            Pop1FloatNumber(&num_flt1);
            stkCPU_FLT_.Push(num_flt1);
            printf("OUT: ");
            printf(PRINT_FORMAT<FLT_TYPE>, num_flt1);
            printf("\n");
            break;

        case CMD_OUT  | REG_FLAG:
        case CMD_OUTQ | REG_FLAG:

            CPU_ASSERTOK((bcode_.size_ - bcode_.ptr_ < 1), CPU_NO_SPACE_FOR_REGISTER, this);

            reg_code = bcode_.data_[bcode_.ptr_++];
            CPU_ASSERTOK(((reg_code > REG_NUM) || (reg_code == 0)), CPU_UNIDENTIFIED_REGISTER, this);

            printf("OUT: ");
            if (cmd_code == (CMD_OUT | REG_FLAG))
                printf(PRINT_FORMAT<INT_TYPE>, registers_[reg_code - 1]);
            else
            if (cmd_code == (CMD_OUTQ | REG_FLAG))
                printf(PRINT_FORMAT<FLT_TYPE>, registers_[reg_code - 1]);
            printf("\n");
            break;

        case CMD_ADD:

            Pop2IntNumbers(&num_int1, &num_int2);
            stkCPU_INT_.Push(num_int2 + num_int1);
            break;

        case CMD_ADDQ:

            Pop2FloatNumbers(&num_flt1, &num_flt2);
            stkCPU_FLT_.Push(num_flt2 + num_flt1);
            break;

        case CMD_SUB:

            Pop2IntNumbers(&num_int1, &num_int2);
            stkCPU_INT_.Push(num_int2 - num_int1);
            break;

        case CMD_SUBQ:

            Pop2FloatNumbers(&num_flt1, &num_flt2);
            stkCPU_FLT_.Push(num_flt2 - num_flt1);
            break;

        case CMD_MUL:

            Pop2IntNumbers(&num_int1, &num_int2);
            stkCPU_INT_.Push(num_int2 * num_int1);
            break;

        case CMD_MULQ:

            Pop2FloatNumbers(&num_flt1, &num_flt2);
            stkCPU_FLT_.Push(num_flt2 * num_flt1);
            break;

        case CMD_DIV:

            Pop2IntNumbers(&num_int1, &num_int2);
            CPU_ASSERTOK((fabs(num_int1) < NIL), CPU_DIVISION_BY_ZERO, this);
            stkCPU_INT_.Push(num_int2 / num_int1);
            break;

        case CMD_DIVQ:

            Pop2FloatNumbers(&num_flt1, &num_flt2);
            CPU_ASSERTOK((fabs(num_flt1) < NIL), CPU_DIVISION_BY_ZERO, this);
            stkCPU_FLT_.Push(num_flt2 / num_flt1);
            break;

        case CMD_NEG:

            Pop1IntNumber(&num_int1);
            stkCPU_INT_.Push(-num_int1);
            break;

        case CMD_NEGQ:

            Pop1FloatNumber(&num_flt1);
            stkCPU_FLT_.Push(-num_flt1);
            break;

        case CMD_AND:

            Pop2IntNumbers(&num_int1, &num_int2);
            stkCPU_INT_.Push(num_int2 & num_int1);
            break;

        case CMD_OR:

            Pop2IntNumbers(&num_int1, &num_int2);
            stkCPU_INT_.Push(num_int2 | num_int1);
            break;

        case CMD_XOR:

            Pop2IntNumbers(&num_int1, &num_int2);
            stkCPU_INT_.Push(num_int2 ^ num_int1);
            break;

        case CMD_SIN:

            Pop1FloatNumber(&num_flt1);
            stkCPU_FLT_.Push(sin(num_flt1));
            break;

        case CMD_COS:

            Pop1FloatNumber(&num_flt1);
            stkCPU_FLT_.Push(cos(num_flt1));
            break;

        case CMD_SQRT:

            Pop1FloatNumber(&num_flt1);
            CPU_ASSERTOK((num_flt1 < 0), CPU_ROOT_OF_A_NEG_NUMBER, this);
            stkCPU_FLT_.Push(sqrt(num_flt1));
            break;

        case CMD_JMP:

            CPU_ASSERTOK((bcode_.size_ - bcode_.ptr_ < POINTER_SIZE), CPU_NO_SPACE_FOR_POINTER, this);

            bcode_.ptr_ = *(ptr_t*)(bcode_.data_ + bcode_.ptr_);
            break;

        case CMD_JE:
        case CMD_JNE:
        case CMD_JA:
        case CMD_JAE:
        case CMD_JB:
        case CMD_JBE:

            CPU_ASSERTOK((bcode_.size_ - bcode_.ptr_ < POINTER_SIZE), CPU_NO_SPACE_FOR_POINTER, this);
            Pop2FloatNumbers(&num_flt1, &num_flt2);
            
            if (cmd_code == CMD_JE ) cond = (fabs(num_flt1 - num_flt2) <  NIL); else
            if (cmd_code == CMD_JNE) cond = (fabs(num_flt1 - num_flt2) >= NIL); else
            if (cmd_code == CMD_JA ) cond = (num_flt1 >  num_flt2);             else
            if (cmd_code == CMD_JAE) cond = (num_flt1 >= num_flt2);             else
            if (cmd_code == CMD_JB ) cond = (num_flt1 <  num_flt2);             else
            if (cmd_code == CMD_JBE) cond = (num_flt1 <= num_flt2);/////////////////
            if (cond)
                bcode_.ptr_ = *(ptr_t*)(bcode_.data_ + bcode_.ptr_);
            else
                bcode_.ptr_ += POINTER_SIZE;
            break;

        case CMD_CALL:

            CPU_ASSERTOK((bcode_.size_ - bcode_.ptr_ < POINTER_SIZE), CPU_NO_SPACE_FOR_POINTER, this);

            stkCPU_PTR_.Push((PTR_TYPE)(bcode_.ptr_ + POINTER_SIZE));
            bcode_.ptr_ = *(ptr_t*)(bcode_.data_ + bcode_.ptr_);
            break;

        case CMD_RET:

            ptr = stkCPU_PTR_.Pop();
            CPU_ASSERTOK((isPOISON(ptr)), CPU_NO_RET_ADDRESS, this);

            bcode_.ptr_ = ptr;
            break;

        case CMD_FLT2INT:

            Pop1FloatNumber(&num_flt1);
            stkCPU_INT_.Push((INT_TYPE)num_flt1);
            break;

        case CMD_INT2FLT:

            Pop1IntNumber(&num_int1);
            stkCPU_FLT_.Push((FLT_TYPE)num_int1);
            break;

        case CMD_SCREEN:

            CPU_ASSERTOK((bcode_.size_ - bcode_.ptr_ < 1), CPU_NO_SPACE_FOR_REGISTER, this);

            reg_code = bcode_.data_[bcode_.ptr_++];
            CPU_ASSERTOK(((reg_code > REG_NUM) || (reg_code == 0)), CPU_UNIDENTIFIED_REGISTER, this);
            CPU_ASSERTOK((isPOISON(registers_[reg_code - 1])), CPU_EMPTY_REGISTER, this);

            ptr = (PTR_TYPE)(int)registers_[reg_code - 1];
            CPU_ASSERTOK((isPOISON(ptr)), CPU_EMPTY_REGISTER, this);
            CPU_ASSERTOK((ptr >= RAM_SIZE), CPU_WRONG_ADDR, this);

            CPU_ASSERTOK((isPOISON(registers_[REG_SCRX - 1])), CPU_EMPTY_REGISTER, this);
            CPU_ASSERTOK((isPOISON(registers_[REG_SCRY - 1])), CPU_EMPTY_REGISTER, this);

            width  = (int)(registers_[REG_SCRX - 1]);
            height = (int)(registers_[REG_SCRY - 1]);
            CPU_ASSERTOK(((width <= 0) || (height <= 0)), CPU_INCORRECT_WINDOW_SIZES, this);
            CPU_ASSERTOK((ptr + width * height * PIXEL_SIZE > RAM_SIZE), CPU_NO_VIDEO_MEMORY, this);

            if (screens_num_ == 0)
            {
                txCreateWindow(width, height);
                filename_ = GetTrueFileName(filename_);
            }

            for (int y = 0; y < height; ++y)
            for (int x = 0; x < width;  ++x)
            {
                txSetPixel(x, y, RGB(RAM_[ptr + (y * width + x) * PIXEL_SIZE + 0],
                                     RAM_[ptr + (y * width + x) * PIXEL_SIZE + 1],
                                     RAM_[ptr + (y * width + x) * PIXEL_SIZE + 2] ));
            }

            strcpy(pictname, filename_);
            sprintf(scrnumstr, "%d", screens_num_);
            strcat(pictname, "(");
            strcat(pictname, scrnumstr);
            strcat(pictname, ")");
            strcat(pictname, ".bmp");
            txSaveImage(pictname);

            ++screens_num_;
            break;

        default:

            CPU_ASSERTOK(1, CPU_UNIDENTIFIED_COMMAND, this);

            return CPU_NOT_OK;
        }
    }

    return CPU_OK;
}

//------------------------------------------------------------------------------

void CPU::Pop1IntNumber (INT_TYPE* num)
{
    assert(num != nullptr);

    *num = stkCPU_INT_.Pop();
    CPU_ASSERTOK((isPOISON(*num)), STACK_EMPTY_STACK, this);
}

//------------------------------------------------------------------------------

void CPU::Pop2IntNumbers (INT_TYPE* num1, INT_TYPE* num2)
{
    assert(num1 != nullptr);
    assert(num2 != nullptr);

    *num1 = stkCPU_INT_.Pop();
    CPU_ASSERTOK((isPOISON(*num1)), STACK_EMPTY_STACK, this);

    *num2 = stkCPU_INT_.Pop();
    CPU_ASSERTOK((isPOISON(*num2)), STACK_EMPTY_STACK, this);
}

//------------------------------------------------------------------------------

void CPU::Pop1FloatNumber (FLT_TYPE* num)
{
    assert(num != nullptr);

    *num = stkCPU_FLT_.Pop();
    CPU_ASSERTOK((isPOISON(*num)), STACK_EMPTY_STACK, this);
}

//------------------------------------------------------------------------------

void CPU::Pop2FloatNumbers(FLT_TYPE* num1, FLT_TYPE* num2)
{
    assert(num1 != nullptr);
    assert(num2 != nullptr);

    *num1 = stkCPU_FLT_.Pop();
    CPU_ASSERTOK((isPOISON(*num1)), STACK_EMPTY_STACK, this);

    *num2 = stkCPU_FLT_.Pop();
    CPU_ASSERTOK((isPOISON(*num2)), STACK_EMPTY_STACK, this);
}

//------------------------------------------------------------------------------

void CPU::PrintCode (const char* logname)
{
    assert(logname != nullptr);

    FILE* log = fopen(logname, "a");
    assert(log != nullptr);

    --bcode_.ptr_;

    fprintf(log, " Address: %08X\n\n", bcode_.ptr_);
    printf (     " Address: %08X\n\n", bcode_.ptr_);

    fprintf(log, "//////////////////////////////////--CODE--//////////////////////////////////" "\n");
    printf (     "//////////////////////////////////--CODE--//////////////////////////////////" "\n");

    fprintf(log, "     Address ");
    printf (     "     Address ");
    for (char i = 0; i < 0x10; ++i)
    {
        fprintf(log, "| %X ", i);
        printf (     "| %X ", i);
    }
    fprintf(log, "\n");
    printf (     "\n");

    size_t line      = bcode_.ptr_  - (bcode_.ptr_  % 0x10);
    size_t last_line = bcode_.size_ - (bcode_.size_ % 0x10);

    for (char i = -0x20; i <= 0x20; i += 0x10)
    {
        if ((line + i >= 0) && (line + i <= last_line))
        {
            fprintf(log, "%s%s%08X ", ((i == 0)? "=>" : "  "), "   ", line + i);
            printf (     "%s%s%08X ", ((i == 0)? "=>" : "  "), "   ", line + i);

            for (char byte = 0; byte < 0x10; ++byte)
            {
                if (line + i + byte == bcode_.size_) break;

                fprintf(log, "%02X  ", (unsigned char)bcode_.data_[line + i + byte]);
                printf (     "%02X  ", (unsigned char)bcode_.data_[line + i + byte]);
            }

            fprintf(log, "\n");
            printf (     "\n");
        }
    }

    for (int i = 0; i < 14 + 4*(bcode_.ptr_ % 0x10); ++i)
    {
        fprintf(log, "=");
        printf (     "=");
    }
    fprintf(log, "/\\\n");
    printf (     "/\\\n");

    fprintf(log, "////////////////////////////////////////////////////////////////////////////" "\n\n");
    printf (     "////////////////////////////////////////////////////////////////////////////" "\n\n");

    fclose(log);
}

//------------------------------------------------------------------------------

void CPUPrintError(const char* logname, const char* file, int line, const char* function, int err)
{
    assert(function != nullptr);
    assert(logname  != nullptr);
    assert(file     != nullptr);

    FILE* log = fopen(logname, "a");
    assert(log != nullptr);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    fprintf(log, "###############################################################################\n");
    fprintf(log, "TIME: %d-%02d-%02d %02d:%02d:%02d\n\n",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fprintf(log, "ERROR: file %s  line %d  function %s\n\n", file, line, function);
    fprintf(log, "%s\n", cpu_errstr[err + 1]);

    printf (     "ERROR: file %s  line %d  function %s\n",   file, line, function);
    printf (     "%s\n\n", cpu_errstr[err + 1]);

    fclose(log);
}

//------------------------------------------------------------------------------
