
#ifndef ASMX64_TOKENS_H
#define ASMX64_TOKENS_H 1

#include <stddef.h>
#include <stdint.h>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

enum class CpuInsnPrefixID;
enum class X64RegValue: uint32_t;

enum class X64TokenType
{
    None = 0,
    String,                                      // 字符串
    Numeric,                                     // 数字
    Register,                                    // 寄存器
    Operator,                                    // 运算符
    Symbol,                                      // 符号
    TempOperand,                                 // 临时操作数
};

enum class X64TokenFlag // [flag]
{
    None = 0,
    NumericToBeUlong = 0x1000,                   // 数字已经转换为ulong, type为Numeric时有效
};

enum class X64AsmOperator
{
    None = 0,
    Plus,               // +
    Minus,              // -
    Multiplication,     // *
    Division,           // /
    AddressStart,       // [
    AddressEnd,         // ]
    ParenthesesL,       // (
    ParenthesesR,       // )
    PositiveSign,       // + 单目正号
    NegativeSign,       // - 单目负号
    RegPlus,            // 寄存器加
    RegMinus,           // 寄存器减
    RegMulti,           // 寄存器乘

};

class X64Token
{
private:
    static std::unordered_set<std::string> htAllowNamePseudoInstruction;
    static void initWithNamePseudoInstruction();
	static std::unordered_map<std::string, CpuInsnPrefixID> htInstructionPrefix;
	static void initInstructionPrefix();
	static std::unordered_map<std::string, X64RegValue> htRegister;
	static void initRegisterList();
	static std::unordered_map<std::string, X64AsmOperator> htOperator;
	static void initOperator();
public:
	X64TokenType tokenType;
	X64TokenFlag flag;
	std::string str;                   			// token的字符串, string 类型
	X64RegValue regValue;                        // 寄存器的值, tokenType 为 Register 时有效
	X64AsmOperator asmOperator;                  // 运算符的值, tokenType 为 Operator 时有效
	uint64_t ulongValue;                            // 数字的值, tokenType 为 Numeric 时有效
	uint64_t symbolIndex;                           // 符号在符号表中的索引, tokenType 为 Symbol 时有效
	X64Operand tempOperand;              // 临时操作数, tokenType 为 TempOperand 时有效, 用于计算表达式时中间值

	X64Token();

    // 是否是允许前面带name的伪指令
    static bool isAllowNamePseudoInstruction(const std::string & str);
	// 是否指令前缀
	static bool isInstructionPrefix(const std::string & str);
	// 获得指令前缀的ID值
	static CpuInsnPrefixID getInstructionPrefixValue(const std::string & str);
	// 是否CPU指令
	static bool isCpuInstruction(const std::string & str);
	// 是否伪指令
	static bool isPseudoInstruction(const std::string & str);
	// 是否虚拟指令
	static bool isVirtualInstruction(const std::string & str);
};

#endif // ASMX64_TOKENS_H


