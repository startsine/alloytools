#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "asmx64.h"

bool InsnMov::insnInitialized = false;
std::list<OpcodeInfos> InsnMov::opcodeInfos;

int InsnMov::process(X64Assembler &assembler, const std::string & insnStr, SourceLine &sourceLine, int pass)
{
	return processCpuIns(assembler, insnStr, sourceLine, pass, opcodeInfos);
}

InsnMov::InsnMov()
{
    if (insnInitialized)
        return;
    insnInitialized = true;
    //
    OpcodeInfos info;
    // MOV acc, moffset64  == A0 /s0
    info.reset();
    info.opcodesSize = 1;
    info.opcodes[0] = 0xA0;
    info.numberOfOperand = 2;
    info.op0 = MatchType::acc;
    info.op1 = MatchType::moffset64;
    info.opcodeFlag = OpcodeFlag_bit0Size | OpcodeFlag_withM64;
    opcodeInfos.push_back(info);
}

