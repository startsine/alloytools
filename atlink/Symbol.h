#pragma once
#include <stdint.h>
#include <string>
#include <vector>

class ElfSymbol
{
public:
    std::string         name;
    uint64_t            value;
    uint64_t            size;
    uint32_t            secIndex;
    uint8_t             info;
    uint8_t             other;
    //
    int64_t             fileIndex = -1;             // symbol 所在的文件在 InputList 中的索引
    int64_t             bigSecTableIndex = -1;      // symbol 在整体 sections 表(大表)中的索引
};

class SymbolList
{
public:
    std::vector<ElfSymbol*>        symbols;
public:
    SymbolList();
    ~SymbolList();
};





