
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "asmx64.h"

void OpcodeInfos::reset()
{
    this->opcodesSize = 0;
    this->numberOfOperand = 0;
    this->op0 = MatchType::None;
    this->op1 = MatchType::None;
    this->op2 = MatchType::None;
    this->op3 = MatchType::None;
    this->opcodeFlag = OpcodeFlag_None;
    this->forbidInfo = MatchForbid_None;
    this->digit = 0;
}

InsnProcessFlag BaseInsn::getInsnFlag()
{
	return InsnProcessFlag::None;
}



