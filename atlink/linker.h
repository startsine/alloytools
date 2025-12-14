#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FileInfo.h"
#include "Section.h"
#include "Symbol.h"
#include "Dependence.h"

#define IMAGE_SEGMENT_DATA_TYPE_IDATA                   1
#define IMAGE_SEGMENT_DATA_TYPE_EDATA                   2

class ImageSegmentData
{
public:
    bool        useSectionData = true;
    int         dataType;                               // 
    uint64_t    needSectionIndex;                       // 在 needLinkedSections 中的索引
};

class ImageSegment
{
public:
    std::vector<ImageSegmentData>   dataInfoList;
    std::string                     segmentName;
};

class Linker
{
public:
    InputList           inputList;
    SectionList         flatSections;
    GlobalSymbolList    flatSymbols;
    SymbolDepend        symbolDepend;                   // 外部符号依赖队列
    FoundExternSymbolList foundExternSymbols;           // 已找到的外部符号列表
    std::vector<uint64_t> linkedDynamicSymbolIndies;    // 需要链接的外部动态库的符号，在全局符号表中的索引 
    std::vector<ElfSection*> needLinkedSections;        // 需要链接的section(仅指针复制，不需做释放)
    std::vector<ImageSegment> imageSegments;            // 
    size_t textSegmentIndex = 0;
    size_t rdataSegmentIndex = 0;
    size_t dataSegmentIndex = 0;

    void scanInputObjects();
    void scanObject(ObjectFile & obj);
    void scanDef(DynamicModuleFile & def);
    void resolveDependences();
    void resolveSymbol(const std::string & symName);
    void resolveSection(ElfSection * pSection);
    void resolveDynamicSymbol(const std::string & symName, uint64_t index);
    // 把需要链接进映像文件的section集合在一起,
    void initNeedLinkedSections();
    // 把需要链接进映像文件的section排序
    void sortNeedLinkedSections();
    // 组合成segment
    void buildSegmentList();
};

