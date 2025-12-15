#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <list>
#include <memory>
#include "Rel.h"

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
    int64_t     flatindex = -1;             // section 在 flat-sections 表中的索引
    bool        used = false;               // 该 section 是否被引用，会被链接成可执行文件 
    std::shared_ptr<std::list<ElfRel> > relocs = nullptr;       // 指向该section的重定位表信息
    // 链接时
    uint64_t    startImageAddress = 0;      // 该section在映像文件中的RVA地址(对齐后)
    uint64_t    offsetInSegment = 0;        // 该section在segment中的偏移(对齐后)
};

class SectionList
{
public:
    std::vector<ElfSection*>        sections;
public :
    SectionList();
    ~SectionList();
    size_t getSize();
    void add(ElfSection*);
};




