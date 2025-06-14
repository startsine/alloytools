
#include <stdio.h>

#include "asmx64.h"

using namespace std;

X64Token::X64Token()
{

}

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

std::unordered_set<std::string> X64Token::htAllowNamePseudoInstruction;

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
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return ::tolower(c); });
    return htAllowNamePseudoInstruction.find(str2) != htAllowNamePseudoInstruction.end();
}

// 是否指令前缀
bool X64Token::isInstructionPrefix(const std::string & str) 
{
	std::string str2 = str;
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return ::tolower(c); });
	return htInstructionPrefix.find(str2) != htInstructionPrefix.end();
}

// 获得指令前缀的ID值
CpuInsnPrefixID X64Token::getInstructionPrefixValue(const std::string & str)
{
	std::string str2 = str;
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return ::tolower(c); });
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
	return X64PseudoInsnList::getInstance().isPseudoInstruction(str);
}

// 是否虚拟指令
bool X64Token::isVirtualInstruction(const std::string & str)
{
	return X64VirtualInsnList::getInstance().isVirtualInstruction(str);
}

// 判断字符串是否为数字token
bool X64Token::isNumericStr(const std::string & str)
{
	if (str.length() == 0)
		return false;
	if (str[0] >= '0' && str[0] <= '9') {
		return true;
	}
	return false;
}

// 
void X64Token::tryParseToU64Value()
{
	if (this->tokenType == X64TokenType::Numeric) {
		uint64_t value1 = 0;
		bool succeed = false;
		if (0 == strncmp(str.c_str(), "0x", 2) || 0 == strncmp(str.c_str(), "0X", 2))
			parseHexToU64(str.substr(2), value1, succeed);
		else if (str.length() > 0 && (str[str.length() - 1] == 'h') && (str[str.length() - 1] == 'H'))
			parseHexToU64(str.substr(0, str.length() - 1), value1, succeed);
		else if (0 == strncmp(str.c_str(), "0b", 2) || 0 == strncmp(str.c_str(), "0B", 2))
			parseBinaryToU64(str.substr(2), value1, succeed);
		else 
			parseDecimalToU64(str, value1, succeed);
		//
		if (succeed) {
			*((uint64_t*) &this->flag) |= (uint64_t) X64TokenFlag::NumericToBeUlong;
			this->ulongValue = value1;
		}
	}
}

// 是否为寄存器
bool X64Token::isRegister(const std::string & str)
{
	std::string str2 = str;
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return ::tolower(c); });
	return htRegister.find(str2) != htRegister.end();
}

// 获取寄存器值
X64RegValue X64Token::getRegisterValue(const std::string & str)
{
	std::string str2 = str;
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return ::tolower(c); });
	auto obj = htRegister.find(str2);
	if (obj == htRegister.end()) {
		return X64RegValue::None;
	}
	return obj->second;
}

// 是否为操作符
bool X64Token::isOperator(const std::string & str)
{
	std::string str2 = str;
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return ::tolower(c); });
	return htOperator.find(str2) != htOperator.end();
}

X64AsmOperator X64Token::getOperatorValue(const std::string & str)
{
	std::string str2 = str;
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return ::tolower(c); });
	auto obj = htOperator.find(str2);
	if (obj == htOperator.end()) {
		return X64AsmOperator::None;
	}
	return obj->second;
}

void parseHexToU64(const std::string & hexStr, uint64_t & value1, bool & succeed)
{
	value1 = 0;
    succeed = false;
    uint8_t tmpInt;
    uint64_t tmpLong = 0;
    for (auto & ch : hexStr) {
		if (ch >= '0' && ch <= '9')
			tmpInt = (uint8_t)(ch - '0');
		else if (ch >= 'A' && ch <= 'F')
			tmpInt = (uint8_t)(ch - 'A' + 10);
		else if (ch >= 'a' && ch <= 'f')
			tmpInt = (uint8_t)(ch - 'a' + 10);
		else if (ch == '_')
			continue;
		else {
			//  不合适字符
			return;
		}
		//
		tmpLong <<= 4;
		tmpLong += tmpInt;
	}
	value1 = tmpLong;
	succeed = true;
}

void parseBinaryToU64(const std::string & binStr, uint64_t & value1, bool & succeed)
{
	value1 = 0;
    succeed = false;
    uint8_t tmpInt;
    uint64_t tmpLong = 0;
    for (auto & ch : binStr) {
		if (ch == '0' || ch == '1')
			tmpInt = (uint8_t)(ch - '0');
		else if (ch == '_')
			continue;
		else {
			//  不合适字符
			return;
		}
		//
		tmpLong <<= 1;
		tmpLong += tmpInt;
	}
	value1 = tmpLong;
	succeed = true;
}

void parseDecimalToU64(const std::string & decStr, uint64_t & value1, bool & succeed)
{
	value1 = 0;
    succeed = false;
    uint8_t tmpInt;
    uint64_t tmpLong = 0;
    for (auto & ch : decStr) {
		if (ch >= '0' && ch <= '9')
			tmpInt = (uint8_t)(ch - '0');
		else if (ch == '_')
			continue;
		else {
			//  不合适字符
			return;
		}
		//
		tmpLong *= 10;
		tmpLong += tmpInt;
	}
	value1 = tmpLong;
	succeed = true;
}



