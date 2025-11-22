#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FileInfo.h"
#include "Section.h"
#include "Symbol.h"
#include "Dependence.h"

class Linker
{
public:
    InputList           inputList;
    SectionList         flatSections;
    GlobalSymbolList    flatSymbols;
    SymbolDepend        symbolDepend;
    FoundExternSymbolList foundExternSymbols;           // 已找到的外部符号列表
    std::vector<uint64_t> linkedDynamicSymbolIndies;    // 需要链接的外部动态库的符号，在全局符号表中的索引 

    void scanInputObjects();
    void scanObject(ObjectFile & obj);
    void scanDef(DynamicModuleFile & def);
    void resolveDependences();
    void resolveSymbol(const std::string & symName);
    void resolveSection(ElfSection * pSection);
    void resolveDynamicSymbol(const std::string & symName, uint64_t index);
};

