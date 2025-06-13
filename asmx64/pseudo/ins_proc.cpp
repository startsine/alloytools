#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "asmx64.h"

InsnProcessFlag Proc::getInsnFlag() 
{
    return InsnProcessFlag::None;
}

int Proc::process(X64Assembler &assembler, std::string insnStr, SourceLine &sourceLine, int pass) 
{
    return 0;
}


