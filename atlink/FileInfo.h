#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "Symbol.h"

enum class FileType
{
    ELF_OBJECT,             // .o 文件
    STATIC_LIB,             // .a 文件
    ELF_DSO,                // .so 文件
    PE_DLL,                 // .dll 文件
    SYM_DEF,                // .dsosym 文件
};

class Linker;

class SrcFile
{
    friend class Linker;
private:
    FILE        * fp = nullptr;
    long long   fileStartOffset = 0;
    long long   fileTotalSize = 0;
    
public:
    std::string inputName;                  // 以参数输入时，它的文件名路径
    std::string fullPathName;               // 文件名全路径
    FileType fileType;
    bool        isBigEndian = false;
    bool        isElf64 = false;
    int64_t     myIndex = -1;               // 我自身在文件列表中的索引 
    SrcFile(const std::string & name, bool libraryFlag);

    virtual void open();
    void close();
    int seek(long long offset, int origin);
    size_t fread(void* buffer, size_t eSize, size_t eCount);
    uint8_t read_u8();
    uint16_t read_u16();
    uint32_t read_u32();
    uint64_t read_u64();
};

class ObjectFile : public SrcFile
{
    friend class Linker;
private:
    uint16_t type;    // Object file type 
    uint16_t machine; // Machine type 
    uint32_t version; // Object file version 
    uint64_t entry;   // Entry point address 
    uint64_t phoff;   // Program header offset 
    uint64_t shoff;   // Section header offset 
    uint32_t flags;   // Processor-specific flags 
    uint16_t ehsize;  // ELF header size 
    uint16_t phentsize; // Size of program header entry 
    uint16_t phnum;   // Number of program header entries 
    uint16_t shentsize; // Size of section header entry 
    uint16_t shnum;   // Number of section header entries 
    uint16_t shstrndx;// Section name string table index 
    //
    size_t   startIndexOfFlatSections;       // 本文件的section表在flatSection表中的起始位置
    ObjectSymbolList objSymbols;             // 本文件中的符号表(所有)
    //
    void scanObject(Linker & linker);
public:
    ObjectFile(const std::string & name);
};

class LibraryFile : public SrcFile
{
public:
    LibraryFile(const std::string & name);
};


class InputList
{
public:
    std::vector<SrcFile*>          fileList;
public:
    InputList();
    ~InputList();
    int addObject(const char * name);
    int addLibrary(const char * name);
};






