#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "asmx64.h"

using namespace std;

Section::Section(const std::string & sectionName) : Section(sectionName, 1u)
{
}

Section::Section(const std::string & sectionName, uint32_t align)
{
	this->sectionName = sectionName;
	this->align = align;
}

X64SectionList::X64SectionList(X64Assembler * assem)
    : currFragmentIndex(-1), assembler(assem)
{
}

size_t X64SectionList::getCurrSectionIndex()
{
	return 0;
}

Symbol* X64SymbolList::getSymbol(const std::string str)
{
    auto obj = symbols.find(str);
    if (obj == symbols.end()) {
        return nullptr;
    }
    return & obj->second;
}

int64_t X64SymbolList::addSymbol(Symbol & symbol)
{
    std::string key = symbol.symbolName;
    auto obj = symbols.find(key);
    if (obj != symbols.end()) {
        return -1;
    }
    symbols[key] = symbol;
    return 0;
}

