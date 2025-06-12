#ifndef ASMX64_INSTRUCTION_H
#define ASMX64_INSTRUCTION_H 1

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

enum class CpuInsnPrefixID
{
    None = 0,
	Rep = 0x01,
};

class IInsnProcessor;

class X64CpuInsnList
{
	friend std::unique_ptr<X64CpuInsnList> std::make_unique<X64CpuInsnList>();
public:
	static X64CpuInsnList & getInstance();
	IInsnProcessor * getInsnProcessor(const std::string & insn);
	bool isCpuInstruction(const std::string & str);
private:
	static std::unique_ptr<X64CpuInsnList> instance;
	X64CpuInsnList();
	std::unordered_map<std::string, std::unique_ptr<IInsnProcessor>> htCpuInsns;
};

class X64PseudoInsnList
{
	friend std::unique_ptr<X64PseudoInsnList> std::make_unique<X64PseudoInsnList>();
public:
    static X64PseudoInsnList & getInstance();
	IInsnProcessor * getPseudoInsnProcessor(const std::string & insn);
	bool isPseudoInstruction(const std::string & str);
private:
	static std::unique_ptr<X64PseudoInsnList> instance;
	X64PseudoInsnList();
	std::unordered_map<std::string, std::unique_ptr<IInsnProcessor>> htPseudoInsns;
};

class X64VirtualInsnList
{
    friend std::unique_ptr<X64VirtualInsnList> std::make_unique<X64VirtualInsnList>();
public:
    static X64VirtualInsnList & getInstance();
	bool isVirtualInstruction(const std::string & str);
private:
	static std::unique_ptr<X64VirtualInsnList> instance;
    X64VirtualInsnList();
};

#endif // ASMX64_INSTRUCTION_H


