
#include <stdio.h>

#include "asmx64.h"

using namespace std;

std::unordered_set<std::string> X64Token::htAllowNamePseudoInstruction = {0};

void X64Token::initWithNamePseudoInstruction()
{
    htAllowNamePseudoInstruction.insert("db");
    htAllowNamePseudoInstruction.insert("dw");
    htAllowNamePseudoInstruction.insert("dd");
    htAllowNamePseudoInstruction.insert("dq");
    htAllowNamePseudoInstruction.insert("proc");
    htAllowNamePseudoInstruction.insert("endp");        // 忽略
    htAllowNamePseudoInstruction.insert("segment");
    htAllowNamePseudoInstruction.insert("ends");        // 忽略
}

std::unordered_map<std::string, CpuInsnPrefixID> X64Token::htInstructionPrefix;

void X64Token::initInstructionPrefix()
{
	htInstructionPrefix.emplace("rep", CpuInsnPrefixID::Rep);
}

// 是否是允许前面带name的伪指令
bool X64Token::isAllowNamePseudoInstruction(std::string str)
{
    std::string str2 = str;
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return std::tolower(c); });
    return htAllowNamePseudoInstruction.find(str2) != htAllowNamePseudoInstruction.end();
}

// 是否指令前缀
bool X64Token::isInstructionPrefix(std::string str) 
{
	std::string str2 = str;
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return std::tolower(c); });
	return htInstructionPrefix.find(str2) != htInstructionPrefix.end();
}

// 获得指令前缀的ID值
CpuInsnPrefixID X64Token::getInstructionPrefixValue(string str)
{
	std::string str2 = str;
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return std::tolower(c); });
	auto obj = htInstructionPrefix.find(str2);
	if (obj == htInstructionPrefix.end()) {
		return CpuInsnPrefixID::None;
	}
	return obj->second;
}




