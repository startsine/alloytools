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
    : currSectionIndex(-1), assembler(assem)
{
}

int64_t X64SectionList::getCurrSectionIndex()
{
    if (sectionList.size() == 0) {
        Section x64TextSection(".text");
        sectionList.push_back(x64TextSection);
        currSectionIndex = (int64_t)(sectionList.size() - 1);
    }
    return currSectionIndex;
}

Section & X64SectionList::getCurrSection()
{
    if (sectionList.size() == 0) {
        Section x64TextSection(".text");
        sectionList.push_back(x64TextSection);
        currSectionIndex = (int64_t)(sectionList.size() - 1);
    }
    return sectionList[currSectionIndex];
}

Section & X64SectionList::getSectionByIndex(int64_t index)
{
    return sectionList[index];
}

Symbol* X64SymbolList::getSymbol(const std::string str)
{
    auto obj = symbols.find(str);
    if (obj == symbols.end()) {
        return nullptr;
    }
    return & obj->second;
}

int X64SymbolList::addSymbol(Symbol & symbol)
{
    std::string key = symbol.symbolName;
    auto obj = symbols.find(key);
    if (obj != symbols.end()) {
        return -1;
    }
    symbols[key] = symbol;
    return 0;
}

