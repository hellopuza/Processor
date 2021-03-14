#include <windows.h>
#include <stdio.h>

int main()
{
    int err = system("cd ../Debug/ && "
                     "Assembler.exe ../Processor/Sierpinski.asm");
    
    if (!err) system("cd ../Debug/ && "
                     "CPU.exe Sierpinski.code");
    
    /*
    */
    
    return 0;
}