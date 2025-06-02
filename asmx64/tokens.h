
#ifndef ASMX64_TOKENS_H
#define ASMX64_TOKENS_H 1

#include <string>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

enum class CpuInsnPrefixID;

class X64Token
{
private:
    static std::unordered_set<std::string> htAllowNamePseudoInstruction;
    static void initWithNamePseudoInstruction();
	static std::unordered_map<std::string, CpuInsnPrefixID> htInstructionPrefix;
	static void initInstructionPrefix();
public:
    // 是否是允许前面带name的伪指令
    static bool isAllowNamePseudoInstruction(std::string str);
	// 是否指令前缀
	static bool isInstructionPrefix(std::string str);
	// 获得指令前缀的ID值
	static CpuInsnPrefixID getInstructionPrefixValue(std::string str);
	// 是否CPU指令
	static bool isCpuInstruction(std::string str);
	// 是否伪指令
	static bool isPseudoInstruction(std::string str);
	// 是否虚拟指令
	static bool isVirtualInstruction(std::string str);
};

#endif // ASMX64_TOKENS_H


