#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>

class Elf64OriSymbol
{
    std::string     name;   // Symbol name 
    uint8_t         info;   // Type and Binding attributes
    uint8_t         other;  // Reserved
    uint16_t        shndx;  // Section table index
    uint64_t        value;  // Symbol value
    uint64_t        size;   // Size of object (e.g., common) 
};

class ElfGlobalSymbol
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
    std::vector<ElfGlobalSymbol*>        symbols;
    std::unordered_map<std::string, std::vector<uint64_t> > globalFinder;         // 全局符号查找hash-table
public:
    SymbolList();
    ~SymbolList();
};





