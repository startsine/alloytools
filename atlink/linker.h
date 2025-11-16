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

    void scanInputObjects();
    void scanObject(ObjectFile & obj);
    void scanDef(DynamicModuleFile & def);
    void resolveDependences();
    void resolveSymbol(const std::string & symName);
    void resolveSection(ElfSection * pSection);
};

