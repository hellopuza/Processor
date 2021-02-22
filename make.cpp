#include <windows.h>

int main(int argc, char* argv[])
{
    system("cd ../Debug/ && Assembler.exe ../Processor/prog.code ../Processor/prog.asm && CPU.exe ../Processor/prog.code");

    return 0;
}