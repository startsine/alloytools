#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>

enum class FileType
{
    ELF_OBJECT,             // .o 文件
    STATIC_LIB,             // .a 文件
    ELF_DSO,                // .so 文件
    PE_DLL,                 // .dll 文件
    SYM_DEF,                // .dsosym 文件
};

class SrcFile
{
    friend class Linker;
private:
    FILE        * fp = nullptr;
    long long   fileStartOffset = 0;
    long long   fileTotalSize = 0;
    bool        isBigEndian = false;
public:
    std::string inputName;                  // 以参数输入时，它的文件名路径
    std::string fullPathName;               // 文件名全路径
    FileType fileType;
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






