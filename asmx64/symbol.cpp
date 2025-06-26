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

size_t X64SectionList::getCurrSectionIndex()
{
	return 0;
}

Symbol* X64SymbolList::getSymbol(const std::string str)
{
    return NULL;
}

int64_t X64SymbolList::addSymbol(Symbol & symbol)
{
    return 0;
}

