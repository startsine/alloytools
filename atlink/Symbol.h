#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>

class SrcFile;

class Elf64ObjectSymbol
{
public:
    int64_t         myIndex = 0;    // 符号在自身.o文件中的索引
    std::string     name;           // Symbol name 
    uint64_t        shndx = 0;      // Section table index (本来应该uint16_t的)
    uint64_t        value = 0;      // Symbol value
    uint64_t        size = 0;       // Size of object (e.g., common) 
    uint8_t         binding = 0;    // Binding
    uint8_t         type = 0;       // type
    uint8_t         visibility = 0; // 可见性
    uint8_t         external = 0;   // 外部模块符号标志。 0: 无意义, 1: 外部模块符号  2: 外部模块符号IAT条目(前面加了__imp_)
};

class ObjectSymbolList
{
public:
    std::vector<Elf64ObjectSymbol>  symbols;
    //std::unordered_map<std::string, std::vector<size_t> > finder;         // 符号查找hash-table
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
    int64_t             secIndex;
    uint8_t             binding = 0;    // Binding
    uint8_t             type = 0;       // type
    uint8_t             visibility = 0; // 可见性
    uint8_t             external = 0;   // 外部模块符号标志。 0: 无意义, 1: 外部模块符号  2: 外部模块符号IAT条目(前面加了__imp_)
    //
    int64_t             fileIndex = -1;             // symbol 所在的文件在 InputList 中的索引
    int64_t             symIndexInFile;             // symbol 在其obj文件中的符号表中的索引 (obj文件中的第几个符号)
    int64_t             flatSymbolIndex = -1;       // symbol 在整体全局的 flat-symbol 表(大表)中的索引
    int64_t             flatSectionIndex = -1;      // symbol 在整体 sections 表(大表)中的索引
};

class GlobalSymbolList
{
public:
    std::vector<ElfGlobalSymbol*>        flatGlobalSymbols;
    std::unordered_map<std::string, std::vector<uint64_t> > finder;         // 全局符号查找hash-table
public:
    GlobalSymbolList();
    ~GlobalSymbolList();
    bool addSymbol(Elf64ObjectSymbol & symbol, SrcFile * srcFile);
    bool addExternalModuleSymbol(const std::string & str, SrcFile * srcFile);
};





