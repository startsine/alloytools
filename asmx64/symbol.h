#ifndef ASMX64_SYMBOL_H
#define ASMX64_SYMBOL_H 1

#include <stdint.h>
#include <vector>
#include <string>

// 符号大小类型
enum class SymbolSizeType
{
	None = 0,
	Const,                              // 常量符号，用等号 = 定义
	Proc,                               // 用 : 定义 (注：用 proc定义也会产生)
	Size1Byte,                          // 用 DB 定义, 或者是 COMM/LCOMM BYTE
	Size2Bytes,                         // 用 DW 定义, 或者是 COMM/LCOMM WORD
	Size4Bytes,                         // 用 DD 定义, 或者是 COMM/LCOMM DWORD
	Size8Bytes,                         // 用 DQ 定义, 或者是 COMM/LCOMM QWORD
};

// 符号可见性
enum class SymbolVisibilityType
{
	None = 0,
	Static,                             // 默认，本文件可见
	Public,                             // public 公共
	Weak,                               // public 公共, 但是是弱符号
	Comdat,                             // public 公共, 但是是 Comdat 类型
	Extern,                             // 用 extern 声明
	//Common,                             // 用 COMM 定义的符号
	//LocalCommon,                        // 用 LCOMM 定义的符号
	Local,                              // local，非全局符号  (只在同一个 fragment 内有效)
};

class Symbol 
{
public:
	std::string symbolName;                                          // 符号名
	uint64_t offsetValue = 0;                                        // 在 record 中的偏移量 (为const时，这里存放值)
	SymbolSizeType sizeType = SymbolSizeType::None;                    // 符号的大小类型
	SymbolVisibilityType visibType = SymbolVisibilityType::None;       // 可见性
	int64_t sectionIndex = -1;                                          // 位于哪个 section 
};

// 重定位类型
enum class RelocType
{
    None = 0,
    ADDR64,                                 // 64位绝对地址
    ADDR32,                                 // 32位绝对地址
    REL32,                                  // 相对下一条指令的相对PC寻址
};

class Section
{
public:
	std::string sectionName;                                       		// 属于哪个 section，默认情况下,代码段是 ".text"，数据段是 ".data"
	uint32_t align;                                                 	// 对齐
	bool isDynamicSize = false;											// section 是否动态大小
	uint64_t size = 0;													// section 的大小 (动态大小时，表示最大的可能大小)

	Section(const std::string & sectionName);
	Section(const std::string & sectionName, uint32_t align);
};

class RelocInfo
{
public:
    std::string name;
    RelocType type = RelocType::None;
    uint32_t offset = 0;
};


class X64Section
{
public:
};

class X64SectionList
{
public:
	size_t getCurrSectionIndex();
};


class X64SymbolList
{
public:
    Symbol* getSymbol(const std::string str);
    long addSymbol(Symbol & symbol);
};

#endif // ASMX64_SYMBOL_H

