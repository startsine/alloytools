#pragma once

#include <stdint.h>
#include <string>
#include <vector>

class ElfSection
{
public:
    std::string name;
    uint64_t    offset = 0;
    uint64_t    size = 0;
    uint64_t    addr = 0;
    uint64_t    entsize = 0;
    uint64_t    addralign = 0;
    uint32_t    link = 0;
    uint32_t    info = 0;
    uint32_t    flags = 0;
    uint32_t    type = 0;
    //
    int64_t     fileIndex = -1;             // section 所在的文件在 InputList 中的索引
    int64_t     localIndex = -1;            // section 在 object 文件中的原始索引
};

class SectionList
{
public:
    std::vector<ElfSection*>        sections;
public :
    SectionList();
    ~SectionList();
};




