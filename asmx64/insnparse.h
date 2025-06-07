#ifndef ASMX64_INSTRUCTION_PARSE_H
#define ASMX64_INSTRUCTION_PARSE_H 1

#include <string>
#include <vector>

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


