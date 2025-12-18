#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "FileInfo.h"
#include "Section.h"
#include "Symbol.h"
#include "Dependence.h"
#include "pebase.h"

#define IMAGE_SEGMENT_DATA_TYPE_EDATA                   1
#define IMAGE_SEGMENT_DATA_TYPE_IDATA                   2
#define IMAGE_SEGMENT_DATA_TYPE_JMPSLOT                 3    // 导入调用的插桩



class ImageSegmentData
{
public:
    bool        useSectionData = true;
    int         dataType;                               // 
    uint64_t    needSectionIndex;                       // 在 needLinkedSections 中的索引
    uint64_t    dataStartRVA = 0;                       // 数据的起始RVA
    uint64_t    dataFileSize = 0;                       // 数据占用文件空间大小
    uint64_t    dataMemSize = 0;                        // 数据占用内存空间大小
    uint64_t    offsetInSegment = 0;                    // 该数据在segment中的偏移
};

class ImageSegment
{
public:
    std::vector<ImageSegmentData>   dataInfoList;
    std::string                     segmentName;
    uint64_t                        segmentStartRVA = 0;
    uint64_t                        segmentFileSize = 0;
    uint64_t                        segmentMemSize = 0;
    uint64_t                        segmentFilePos = 0;
    std::shared_ptr<uint8_t>        segmentData = nullptr;
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
    uint32_t  segmentFileAlign = 512;
    uint32_t  segmentMemAlign = 4096;
    uint32_t  firstSegmentStartRva = 0x1000;
    uint64_t  idataAddress = 0;                         // .idata的地址(RVA)
    uint64_t  idataSize = 0;                            // .idata的大小
    uint64_t  iatAddress = 0;                           // IAT 的地址(RVA)
    uint64_t  iatSize = 0;                              // IAT 的大小
    std::shared_ptr<uint8_t> idataRawData = nullptr;    // .idata的数据
    uint64_t  jmpSlotAddress = 0;                       // jmpSlot的地址(RVA)
    uint64_t  jmpSlotByteSize = 0;                      // jmpSlot的大小(字节)
    uint64_t  jmpSlotItemCount = 0;                     // jmpSlot的项数
    std::shared_ptr<uint8_t> jmpSlotRawData = nullptr;  // jmpSlot的数据
    // PE信息
    COFFFileHeader coffHeader;                          // coff头
    uint32_t    coffHeaderFilePos;                      // coff头文件偏移
    OptionalHeader64 pe64OptHeader;                     // 可选头(包含数据目录)
    uint32_t    peOptHeaderFilePos;                     // 可选头文件偏移
    std::vector<PESection> peSections;                  // 节表
    uint32_t    peSectionHeaderFilePos;                 // 节表文件偏移
    uint32_t    firstPESectionDataFilePos;              // PE首节数据的文件偏移

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
    // 加载合成segment数据MAP
    void buildSegmentDataMap();
    // 构建segmnet的完整数据
    void buildAndFixupSegmentFullData();
    // 
    void buildImageFile();

    void buildPEImportTable(uint64_t addr);
    void updateExternModuleSymbolValue(const std::string symbolName, uint64_t value);
};

