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

/*

/r      OpcodeFlag_ModRM_R           表示该指令带ModRM域，并且同时有reg和r/m两个操作数
/0-7    OpcodeFlag_ModRM_Digit       表示该指令带ModRM域，r/m 域代表r/m, 但是 reg 域不代表寄存器，而是代表 3bit 的额外 opcode
/s0     OpcodeFlag_bit0Size          指令码的 bit0 代表操作数大小, bit0==0是为8bit, bit0==1是为 16/32/64 bit
/s3     OpcodeFlag_bit3Size          指令码的 bit3 代表操作数大小, bit3==0是为8bit, bit3==1是为 16/32/64 bit

*/


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
    // MOV moffset64, acc  == A2 /s0
    info.reset();
    info.opcodesSize = 1;
    info.opcodes[0] = 0xA2;
    info.numberOfOperand = 2;
    info.op0 = MatchType::moffset64;
    info.op1 = MatchType::acc;
    info.opcodeFlag = OpcodeFlag_bit0Size | OpcodeFlag_withM64;
    opcodeInfos.push_back(info);
    // MOV reg/mem, reg  == 88 /r /s0
    info.reset();
    info.opcodesSize = 1;
    info.opcodes[0] = 0x88;
    info.numberOfOperand = 2;
    info.op0 = MatchType::rm;
    info.op1 = MatchType::reg;
    info.opcodeFlag = OpcodeFlag_ModRM_R | OpcodeFlag_bit0Size;
    opcodeInfos.push_back(info);
    // MOV reg, reg/mem,  == 8A /r /s0
    info.reset();
    info.opcodesSize = 1;
    info.opcodes[0] = 0x8A;
    info.numberOfOperand = 2;
    info.op0 = MatchType::reg;
    info.op1 = MatchType::rm;
    info.opcodeFlag = OpcodeFlag_ModRM_R | OpcodeFlag_bit0Size | OpcodeFlag_RMOnRight;
    opcodeInfos.push_back(info);
    // MOV reg, imm == B0 +r i /s3
    info.reset();
    info.opcodesSize = 1;
    info.opcodes[0] = 0xB0;
    info.numberOfOperand = 2;
    info.op0 = MatchType::reg;
    info.op1 = MatchType::imm;
    info.opcodeFlag = OpcodeFlag_bit3Size | OpcodeFlag_withImm | OpcodeFlag_opcodeWithReg;
    opcodeInfos.push_back(info);
    // MOV reg/mem, imm  == C6 /0 i /s0
    info.reset();
    info.opcodesSize = 1;
    info.opcodes[0] = 0xC6;
    info.numberOfOperand = 2;
    info.op0 = MatchType::rm;
    info.op1 = MatchType::imm;
    info.opcodeFlag = OpcodeFlag_bit0Size | OpcodeFlag_withImm | OpcodeFlag_ModRM_Digit;
    info.digit = 0;
    opcodeInfos.push_back(info);
    // MOV reg/mem, segReg == 8C /r
    info.reset();
    info.opcodesSize = 1;
    info.opcodes[0] = 0x8C;
    info.numberOfOperand = 2;
    info.op0 = MatchType::rm;
    info.op1 = MatchType::segReg;
    info.opcodeFlag = OpcodeFlag_ModRM_R;
    info.forbidInfo = MatchForbid_8bit;              // 禁止8bit操作
    opcodeInfos.push_back(info);
    // MOV segReg, reg/mem16 == 8E /r
    info.reset();
    info.opcodesSize = 1;
    info.opcodes[0] = 0x8E;
    info.numberOfOperand = 2;
    info.op0 = MatchType::segReg;
    info.op1 = MatchType::rm;
    info.opcodeFlag = OpcodeFlag_ModRM_R | OpcodeFlag_RMOnRight;
    info.forbidInfo = MatchForbid_8bit | MatchForbid_32bit | MatchForbid_64bit;    // 禁止8/32/64bit操作
    opcodeInfos.push_back(info);
    // MOV reg64, DRn == 0F 21 /r
    info.reset();
    info.opcodesSize = 2;
    info.opcodes[0] = 0x0F;
    info.opcodes[1] = 0x21;
    info.numberOfOperand = 2;
    info.op0 = MatchType::rm;
    info.op1 = MatchType::debugReg;
    info.opcodeFlag = OpcodeFlag_ModRM_R;
    info.forbidInfo = MatchForbid_8bit | MatchForbid_16bit | MatchForbid_32bit | MatchForbid_mem;   // 禁止内存操作和8/16/32bit寄存器
    opcodeInfos.push_back(info);
    // MOV DRn, reg64 == 0F 23 /r
    info.reset();
    info.opcodesSize = 2;
    info.opcodes[0] = 0x0F;
    info.opcodes[1] = 0x23;
    info.numberOfOperand = 2;
    info.op0 = MatchType::debugReg;
    info.op1 = MatchType::rm;
    info.opcodeFlag = OpcodeFlag_ModRM_R | OpcodeFlag_RMOnRight;
    info.forbidInfo = MatchForbid_8bit | MatchForbid_16bit | MatchForbid_32bit | MatchForbid_mem;   // 禁止内存操作和8/16/32bit寄存器
    opcodeInfos.push_back(info);
    // MOV reg64, CRn == 0F 20 /r
    info.reset();
    info.opcodesSize = 2;
    info.opcodes[0] = 0x0F;
    info.opcodes[1] = 0x20;
    info.numberOfOperand = 2;
    info.op0 = MatchType::rm;
    info.op1 = MatchType::ctrlReg;
    info.opcodeFlag = OpcodeFlag_ModRM_R;
    info.forbidInfo = MatchForbid_8bit | MatchForbid_16bit | MatchForbid_32bit | MatchForbid_mem;   // 禁止内存操作和8/16/32bit寄存器
    opcodeInfos.push_back(info);
    // MOV CRn, reg64 == 0F 22 /r
    info.reset();
    info.opcodesSize = 2;
    info.opcodes[0] = 0x0F;
    info.opcodes[1] = 0x22;
    info.numberOfOperand = 2;
    info.op0 = MatchType::ctrlReg;
    info.op1 = MatchType::rm;
    info.opcodeFlag = OpcodeFlag_ModRM_R | OpcodeFlag_RMOnRight;
    info.forbidInfo = MatchForbid_8bit | MatchForbid_16bit | MatchForbid_32bit | MatchForbid_mem;   // 禁止内存操作和8/16/32bit寄存器
    opcodeInfos.push_back(info);
}

