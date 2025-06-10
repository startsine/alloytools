
#include <stdio.h>

#include "asmx64.h"

using namespace std;

bool X64Token::initFlag = false;

void X64Token::initTokenInfos()
{
	if (initFlag) 
		return;
	initFlag = true;
	initWithNamePseudoInstruction();
	initInstructionPrefix();
	initRegisterList();
	initOperator();
}

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

std::unordered_map<std::string, X64RegValue> X64Token::htRegister;

void X64Token::initRegisterList()
{
    htRegister.emplace("al", X64RegValue::AL);
    htRegister.emplace("cl", X64RegValue::CL);
    htRegister.emplace("dl", X64RegValue::DL);
    htRegister.emplace("bl", X64RegValue::BL);
    htRegister.emplace("ah", X64RegValue::AH);
    htRegister.emplace("ch", X64RegValue::CH);
    htRegister.emplace("dh", X64RegValue::DH);
    htRegister.emplace("bh", X64RegValue::BH);
    htRegister.emplace("spl", X64RegValue::SPL);
    htRegister.emplace("bpl", X64RegValue::BPL);
    htRegister.emplace("sil", X64RegValue::SIL);
    htRegister.emplace("dil", X64RegValue::DIL);
    htRegister.emplace("r8b", X64RegValue::R8B);
    htRegister.emplace("r9b", X64RegValue::R9B);
    htRegister.emplace("r10b", X64RegValue::R10B);
    htRegister.emplace("r11b", X64RegValue::R11B);
    htRegister.emplace("r12b", X64RegValue::R12B);
    htRegister.emplace("r13b", X64RegValue::R13B);
    htRegister.emplace("r14b", X64RegValue::R14B);
    htRegister.emplace("r15b", X64RegValue::R15B);
    htRegister.emplace("ax", X64RegValue::AX);
    htRegister.emplace("cx", X64RegValue::CX);
    htRegister.emplace("dx", X64RegValue::DX);
    htRegister.emplace("bx", X64RegValue::BX);
    htRegister.emplace("sp", X64RegValue::SP);
    htRegister.emplace("bp", X64RegValue::BP);
    htRegister.emplace("si", X64RegValue::SI);
    htRegister.emplace("di", X64RegValue::DI);
    htRegister.emplace("r8w", X64RegValue::R8W);
    htRegister.emplace("r9w", X64RegValue::R9W);
    htRegister.emplace("r10w", X64RegValue::R10W);
    htRegister.emplace("r11w", X64RegValue::R11W);
    htRegister.emplace("r12w", X64RegValue::R12W);
    htRegister.emplace("r13w", X64RegValue::R13W);
    htRegister.emplace("r14w", X64RegValue::R14W);
    htRegister.emplace("r15w", X64RegValue::R15W);
    htRegister.emplace("eax", X64RegValue::EAX);
    htRegister.emplace("ecx", X64RegValue::ECX);
    htRegister.emplace("edx", X64RegValue::EDX);
    htRegister.emplace("ebx", X64RegValue::EBX);
    htRegister.emplace("esp", X64RegValue::ESP);
    htRegister.emplace("ebp", X64RegValue::EBP);
    htRegister.emplace("esi", X64RegValue::ESI);
    htRegister.emplace("edi", X64RegValue::EDI);
    htRegister.emplace("r8d", X64RegValue::R8D);
    htRegister.emplace("r9d", X64RegValue::R9D);
    htRegister.emplace("r10d", X64RegValue::R10D);
    htRegister.emplace("r11d", X64RegValue::R11D);
    htRegister.emplace("r12d", X64RegValue::R12D);
    htRegister.emplace("r13d", X64RegValue::R13D);
    htRegister.emplace("r14d", X64RegValue::R14D);
    htRegister.emplace("r15d", X64RegValue::R15D);
    htRegister.emplace("rax", X64RegValue::RAX);
    htRegister.emplace("rcx", X64RegValue::RCX);
    htRegister.emplace("rdx", X64RegValue::RDX);
    htRegister.emplace("rbx", X64RegValue::RBX);
    htRegister.emplace("rsp", X64RegValue::RSP);
    htRegister.emplace("rbp", X64RegValue::RBP);
    htRegister.emplace("rsi", X64RegValue::RSI);
    htRegister.emplace("rdi", X64RegValue::RDI);
    htRegister.emplace("r8", X64RegValue::R8);
    htRegister.emplace("r9", X64RegValue::R9);
    htRegister.emplace("r10", X64RegValue::R10);
    htRegister.emplace("r11", X64RegValue::R11);
    htRegister.emplace("r12", X64RegValue::R12);
    htRegister.emplace("r13", X64RegValue::R13);
    htRegister.emplace("r14", X64RegValue::R14);
    htRegister.emplace("r15", X64RegValue::R15);
    // 省去 cs、ds、es、ss
    htRegister.emplace("fs", X64RegValue::FS);
    htRegister.emplace("gs", X64RegValue::GS);
    //
}

std::unordered_map<std::string, X64AsmOperator> X64Token::htOperator;

void X64Token::initOperator()
{
    htOperator.emplace("+", X64AsmOperator::Plus);
    htOperator.emplace("-", X64AsmOperator::Minus);
    htOperator.emplace("*", X64AsmOperator::Multiplication);
    htOperator.emplace("/", X64AsmOperator::Division);
    htOperator.emplace("[", X64AsmOperator::AddressStart);
    htOperator.emplace("]", X64AsmOperator::AddressEnd);
    htOperator.emplace("(", X64AsmOperator::ParenthesesL);
    htOperator.emplace(")", X64AsmOperator::ParenthesesR);
}

// 是否是允许前面带name的伪指令
bool X64Token::isAllowNamePseudoInstruction(const std::string & str)
{
    std::string str2 = str;
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return std::tolower(c); });
    return htAllowNamePseudoInstruction.find(str2) != htAllowNamePseudoInstruction.end();
}

// 是否指令前缀
bool X64Token::isInstructionPrefix(const std::string & str) 
{
	std::string str2 = str;
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return std::tolower(c); });
	return htInstructionPrefix.find(str2) != htInstructionPrefix.end();
}

// 获得指令前缀的ID值
CpuInsnPrefixID X64Token::getInstructionPrefixValue(const std::string & str)
{
	std::string str2 = str;
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return std::tolower(c); });
	auto obj = htInstructionPrefix.find(str2);
	if (obj == htInstructionPrefix.end()) {
		return CpuInsnPrefixID::None;
	}
	return obj->second;
}

// 是否CPU指令
bool X64Token::isCpuInstruction(const std::string & str)
{
	return X64CpuInsnList::getInstance().isCpuInstruction(str);
}

// 是否伪指令
bool X64Token::isPseudoInstruction(const std::string & str)
{

}

// 是否虚拟指令
bool X64Token::isVirtualInstruction(const std::string & str)
{

}


