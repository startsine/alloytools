#ifndef ASMX64_INSTRUCTION_H
#define ASMX64_INSTRUCTION_H 1

#include <string>
#include <vector>

enum class CpuInsnPrefixID
{
    None = 0,
	Rep = 0x01,
};

class IInsnProcessor;

class X64CpuInsnList
{
public:
	static X64CpuInsnList & getInstance();
	IInsnProcessor * getInsnProcessor(std::string insn);
private:
	X64CpuInsnList();
};

#endif // ASMX64_INSTRUCTION_H


