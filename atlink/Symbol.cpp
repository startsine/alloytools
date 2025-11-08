#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "Symbol.h"
#include "FileInfo.h"

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
    pGlobal->info = srcSym.info;
    pGlobal->other = srcSym.other;
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



