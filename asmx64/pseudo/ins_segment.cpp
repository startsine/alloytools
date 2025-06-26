#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "asmx64.h"

InsnProcessFlag Segment::getInsnFlag() 
{
    return InsnProcessFlag::None;
}

int Segment::process(X64Assembler &assembler, const std::string & insnStr, SourceLine &sourceLine, int pass)
{
    return 0;
}



