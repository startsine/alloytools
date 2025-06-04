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

class IInsnProcessor
{
public:
    //int process(X64Assembler asm, string insnStr, SourceLine sourceLine, int pass) = 0;
    virtual InsnProcessFlag getInsnFlag() = 0;
};

#endif // ASMX64_INSTRUCTION_PARSE_H


