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

class FileInfo
{
public:
    std::string inputName;                  // 以参数输入时，它的文件名路径
    std::string fullPathName;               // 文件名全路径
    FileType fileType;
    FileInfo(std::string name, bool libraryFlag);
};

class InputList
{
private:
    std::vector<FileInfo*>          fileList;
public:
    InputList();
    ~InputList();
};






