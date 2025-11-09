#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "Symbol.h"
#include "FileInfo.h"
#include "at_elf.h"

ObjectSymbolList::ObjectSymbolList()
{
}

ObjectSymbolList::~ObjectSymbolList()
{
}

GlobalSymbolList::GlobalSymbolList()
{
    finder.reserve(10000);
}

GlobalSymbolList::~GlobalSymbolList()
{
    if (flatGlobalSymbols.size() != 0) {
        for (size_t i = 0; i < flatGlobalSymbols.size(); i++) {
            if (flatGlobalSymbols[i] != nullptr) {
                delete flatGlobalSymbols[i];
                flatGlobalSymbols[i] = nullptr;
            }
        }
        flatGlobalSymbols.clear();
    }
}

bool GlobalSymbolList::addSymbol(Elf64ObjectSymbol & srcSym, SrcFile * srcFile)
{
    if (srcSym.name == "")
        return true;
    uint64_t flatSymbolIndex;
    ElfGlobalSymbol * pGlobal = new ElfGlobalSymbol();
    pGlobal->name = srcSym.name;
    pGlobal->value = srcSym.value;
    pGlobal->size = srcSym.size;
    pGlobal->secIndex = (int64_t) srcSym.shndx;
    pGlobal->binding = srcSym.binding;
    pGlobal->type = srcSym.type;
    pGlobal->visibility = srcSym.visibility;
    pGlobal->external = srcSym.external;
    //
    pGlobal->fileIndex = srcFile->myIndex;
    pGlobal->symIndexInFile = srcSym.myIndex;
    flatSymbolIndex = (uint64_t)flatGlobalSymbols.size();
    pGlobal->flatSymbolIndex = (int64_t)flatGlobalSymbols.size();
    if (srcFile->fileType == FileType::ELF_OBJECT) {
        ObjectFile * pObj = dynamic_cast<ObjectFile *> (srcFile);
        pGlobal->flatSectionIndex = srcSym.shndx + pObj->startIndexOfFlatSections;
    }
    else {
        pGlobal->flatSectionIndex = 0;
    }
    flatGlobalSymbols.push_back(pGlobal);
    //
    auto it = finder.find(srcSym.name);
    if (it == finder.end()) {

    }
    else {
        auto a = it->first;
        auto b = it->second;
    }
    /*
    //
    int64_t             flatIndex = -1;             // symbol 在整体 sections 表(大表)中的索引
    */
    return true;
}

bool GlobalSymbolList::addExternalModuleSymbol(const std::string & str, SrcFile * srcFile)
{
    bool ret1, ret2;
    Elf64ObjectSymbol sym1;
    sym1.name = str;
    sym1.binding = SYMBOL_BINDINGS_GLOBAL;
    sym1.external = 1;
    ret1 = addSymbol(sym1, srcFile);
    Elf64ObjectSymbol sym2;
    sym2.name = std::string("__imp_") + str;
    sym2.binding = SYMBOL_BINDINGS_GLOBAL;
    sym2.external = 2;
    ret2 = addSymbol(sym2, srcFile);
    return ret1 && ret2;
}




