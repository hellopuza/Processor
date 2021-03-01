#include <windows.h>

int main()
{
    int err = system("cd ../Debug/ && "
                     "Assembler.exe ../Processor/prog.code ../Processor/prog.asm");

    if (!err) system("cd ../Debug/ && "
                     "CPU.exe ../Processor/prog.code");

    return 0;
}