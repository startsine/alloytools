#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>

class Elf64ObjectSymbol
{
public:
    int64_t         myIndex;    // 符号在自身.o文件中的索引
    std::string     name;       // Symbol name 
    uint64_t        shndx;      // Section table index (本来应该uint16_t的)
    uint64_t        value;      // Symbol value
    uint64_t        size;       // Size of object (e.g., common) 
    uint8_t         info;       // Type and Binding attributes
    uint8_t         other;      // Reserved
};

class ObjectSymbolList
{
public:
    std::vector<Elf64ObjectSymbol>  symbols;
    std::unordered_map<std::string, std::vector<size_t> > finder;         // 符号查找hash-table
    //
    ObjectSymbolList();
    ~ObjectSymbolList();
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

class GlobalSymbolList
{
public:
    std::vector<ElfGlobalSymbol*>        symbols;
    std::unordered_map<std::string, std::vector<uint64_t> > finder;         // 全局符号查找hash-table
public:
    GlobalSymbolList();
    ~GlobalSymbolList();
};





