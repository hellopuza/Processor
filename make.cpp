#include <windows.h>
#include <stdio.h>

int main()
{
    int err = system("cd ../Debug/ && "
                     "Assembler.exe ../Processor/factorial.asm");

    if (!err) system("cd ../Debug/ && "
                     "CPU.exe factorial.code");
    
    /*
    */
    
    return 0;
}