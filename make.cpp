#include <windows.h>
#include <stdio.h>

int main()
{
    int err = system("cd ../Debug/ && "
                     "Assembler.exe ../Processor/Circle.asm");

    if (!err) system("cd ../Debug/ && "
                     "CPU.exe Circle.code");
    
    if (!err) system("cd ../Debug/ && "
                     "Disassembler.exe Circle.code");
    
    return 0;
}