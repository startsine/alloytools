#ifndef ASMX64_INSTRUCTION_PARSE_H
#define ASMX64_INSTRUCTION_PARSE_H 1

#include <stdint.h>
#include <string>
#include <vector>

enum MatchType
{
    None = 0,
    imm,                            // 操作数是立即数
    acc,                            // 操作数是AL,AX,EAX,RAX
    reg,                            // 操作数是一个通用寄存器
    rm,                             // 操作数是寄存器或者内存寻址
    //moffset32,                      // 操作数是内存寻址，用[imm]寻址的
    moffset64,                      // 操作数是内存寻址，用[imm64]寻址的
    segReg,                         // 操作数是段寄存器
    debugReg,                       // 操作数是DR0-DR15
    ctrlReg,                        // 操作数是CR0-CR15
};

enum MatchForbid
{
    None = 0,
    Forbid_8bit = 0x01,
    Forbid_16bit = 0x02,
    Forbid_32bit = 0x04,
    Forbid_64bit = 0x08,
    Forbid_mem = 0x10,
};

enum OpcodeFlag
{
    None = 0,
    ModRM_R = 0x01,                 //  /r :     带 ModRM 并且 其中代表两个值，reg 和 r/m
    ModRM_Digit = 0x02,             //  /digit:  带 ModRM 并且 r/m 域代表r/m, reg 域代表 3bit 的额外 opcode
    bit0Size = 0x04,                //  指令码的 bit0 代表操作数大小, bit0==0是为8bit, bit0==1是为 16/32/64 bit, 
    bit3Size = 0x08,                //  指令码的 bit3 代表操作数大小, bit3==0是为8bit, bit3==1是为 16/32/64 bit
    opcodeWithReg = 0x10,           //  将寄存器插入到 opcode 的 bit2-bit0 位置
    withImm = 0x20,                 //  指令码最后带立即数作为操作数
    withM64 = 0x40,                 //  指令码最后带64位立即数做寻址
    RMInRight = 0x80,               //  该bit为1时表示匹配的 R/M 域放置于第2个操作数，为0则R/M 域放置于第1个操作数
};

class OpcodeInfos
{
public:
    uint8_t opcodes[16];
    int opcodesSize;
    int numberOfOperand;
    MatchType op0;
    MatchType op1;
    MatchType op2;
    MatchType op3;
    OpcodeFlag opcodeFlag;
    MatchForbid forbidInfo;
    uint8_t digit;
    // 复位
    void reset();
};

enum class InsnProcessFlag
{
    None = 0,
    ProcessTokens = 0x01,                           // 伪指令自己处理 token
    ProcessExpressions = 0x02,                      // 伪指令处理ulong值以内大小的表达式(如DB、DW、DD、DQ)
};

class SourceLine;
class X64Assembler;

class IInsnProcessor
{
public:
    virtual int process(X64Assembler & assembler, std::string insnStr, SourceLine & sourceLine, int pass) = 0;
    virtual InsnProcessFlag getInsnFlag() = 0;
};

class BaseInsn: public IInsnProcessor
{
public:
	InsnProcessFlag getInsnFlag() override;
};

// 基本数据定义伪指令 (DB,DW,DD,DQ)
class DataDefineBaseInsn : IInsnProcessor
{
	////
};


#endif // ASMX64_INSTRUCTION_PARSE_H


