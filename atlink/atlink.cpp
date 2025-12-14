#include <stdio.h>
#include <memory>
#include <algorithm>
#ifdef _WIN32
#include <Windows.h>
#endif
#include "FileInfo.h"
#include "linker.h"
#include "at_elf.h"

void Linker::scanInputObjects()
{
    for (size_t i = 0; i < inputList.fileList.size(); i++) {
        if (inputList.fileList[i]->fileType == FileType::ELF_OBJECT) {
            ObjectFile * pObj = (ObjectFile*) inputList.fileList[i];
            scanObject(*pObj);
        }
        else if (inputList.fileList[i]->fileType == FileType::SYM_DEF) {
            DynamicModuleFile * pDef = dynamic_cast<DynamicModuleFile*>(inputList.fileList[i]);
            scanDef(*pDef);
        }
    }
}

void Linker::scanObject(ObjectFile & obj)
{
    obj.scanObject(*this);
}

void Linker::scanDef(DynamicModuleFile & def)
{
    def.scanDefTextFile(*this);
}

void Linker::resolveDependences()
{
    while (true)
    {
        std::string depSymbolName = symbolDepend.getSymbol();
        if (depSymbolName != "") {
            resolveSymbol(depSymbolName);
        }
        if (symbolDepend.isEmpty()) {
            break;
        }
    }
}

void Linker::resolveSymbol(const std::string & symName)
{
    auto symIndicesInfo = flatSymbols.finder.find(symName);
    if (symIndicesInfo != flatSymbols.finder.end()) {
        //// TO-DO 
        //  这个要加入多个同名符号，优先取其中的强符号
        auto symList = symIndicesInfo->second;
        uint64_t idx = symList[0];
        auto sym = flatSymbols.flatGlobalSymbols[idx];
        if (sym->external == 0) {
            // external 为 0 表示本执行模块内需要解决的符号
            auto flatSectionIndex = sym->flatSectionIndex;
            auto pSection = flatSections.sections[flatSectionIndex];
            if (!pSection->used) {
                resolveSection(pSection);
            }
        }
        else {
            // 这里处理依赖本执行模块外的符号 （在其它dll/so）
            resolveDynamicSymbol(symName, idx);
        }
        foundExternSymbols.addSymbol(symName);      // 添加已找到的外部符号列表 
    }
    else {
        //// TO-DO  报错
    }
}

void Linker::resolveDynamicSymbol(const std::string & symName, uint64_t index)
{
    linkedDynamicSymbolIndies.push_back(index);
    // 
    std::string extName;
    std::string extNameImpStub;
    if (0 == strncmp(symName.c_str(), "__imp_", 6)) {
        extNameImpStub = symName;
        extName = symName.substr(6);
    }
    else {
        extName = symName;
        extNameImpStub = std::string("__imp_") + symName;
    }
    foundExternSymbols.addSymbol(extName);
    foundExternSymbols.addSymbol(extNameImpStub);
}

void Linker::resolveSection(ElfSection * pSection)
{
    if (pSection->used) {
        return;
    }
    pSection->used = true;
    auto fileIndex = pSection->fileIndex;       // section 所在的文件在 InputList 中的索引
    auto localIndex = pSection->localIndex;     // section 在 object 文件中的原始索引
    auto file = inputList.fileList[fileIndex];
    // 把重定位信息挂载到 section 中
    pSection->relocs = std::make_shared<std::list<ElfRel> >();
    // 读取该section相关的全部重定位信息
    if (file->fileType == FileType::ELF_OBJECT) {
        auto obj = dynamic_cast<ObjectFile*>(file);
        auto startIndex = obj->startIndexOfFlatSections;
        auto totalSectionNum = obj->shnum;
        // 找出该 section 对应的 重定位信息 
        for (size_t idx = startIndex; idx < (startIndex + totalSectionNum); idx++) {
            auto sec = flatSections.sections[idx];
            if (sec->info == localIndex && (sec->type == ELF_SECTION_TYPE_RELA || sec->type == ELF_SECTION_TYPE_REL)) {
                // 加载该 section 的重定位信息 
                obj->loadRelocTable(*sec, pSection->relocs.get());
                //break;  // 注释掉break;可以加载多个重定位表(一般是只有一个)
            }
        }
        // 将该section的重定位表中的依赖的外部符号全部加入
        std::vector<uint32_t> needInnerSectionIndices;          // 本section依赖obj文件内部的section的索引列表
        for (auto it = pSection->relocs->begin(); it != pSection->relocs->end(); it++) {
            uint32_t symbolIndex = it->symbolIndex;
            if (symbolIndex < obj->objSymbols.symbols.size()) {
                auto & sym = obj->objSymbols.symbols[symbolIndex];
                if (sym.shndx == 0) {       // 节索引为0表示需要引用外部符号 
                    if (!foundExternSymbols.inList(sym.name)) {
                        symbolDepend.addSymbol(sym.name);       // 添加到符号依赖
                    }
                }
                else {                      // 节索引不为0表示需要引用本obj内的符号 
                    needInnerSectionIndices.push_back(sym.shndx);
                }
            }
        }
        // 如果需要依赖本obj的其他section，则这里添加依赖
        if (needInnerSectionIndices.size() != 0) {
            for (size_t j = 0; j < needInnerSectionIndices.size(); j++) {
                uint32_t localIdx = needInnerSectionIndices[j];
                auto startIndexOfFlatSections = obj->startIndexOfFlatSections;
                auto flatSectionIndex = startIndexOfFlatSections + localIdx;
                auto pDepSection = flatSections.sections[flatSectionIndex];
                if (!pDepSection->used) {
                    resolveSection(pDepSection);
                }
            }
        }
    }
    return;
}

void Linker::initNeedLinkedSections()
{
    for (size_t i = 0; i < flatSections.sections.size(); i++) {
        ElfSection * sec = flatSections.sections[i];
        if (sec->used) {
            needLinkedSections.push_back(sec);
        }
    }
}

void Linker::sortNeedLinkedSections()
{
    // 配列顺序:
    //   1. 带 EXEC 标志的 (带 EXEC 标志的不能带W标志，带有的会自动去掉)
    //   2. 不带 WRITE 标志的数据段(PROGBITS 类型 - 已初始化数据-只读)
    //   3. 不带 WRITE 标志的数据段(NOBITS 类型 - 未初始化数据-只读)
    //   4. 带 WRITE 标志的数据段(PROGBITS 类型 - 已初始化数据)
    //   5. 带 WRITE 标志的数据段(NOBITS 类型 - 未初始化数据)
    auto cmp = [](ElfSection* a, ElfSection* b) -> bool {
        int aValue = 0;
        int bValue = 0;
        // 可执行优先
        if (a->flags & ELF_SECTION_FLAG_EXEC) {
            aValue += 1000000;
        }
        if (b->flags & ELF_SECTION_FLAG_EXEC) {
            bValue += 1000000;
        }
        // 只读数据次级优先
        if (!(a->flags & ELF_SECTION_FLAG_WRITE)) {
            aValue += 10000;
        }
        if (!(b->flags & ELF_SECTION_FLAG_WRITE)) {
            bValue += 10000;
        }
        // 已初始化数据次次级优先 (PROGBITS 比 NOBITS 优先)
        if (a->type == ELF_SECTION_TYPE_PROGBITS) {
            aValue += 100;
        }
        if (b->type == ELF_SECTION_TYPE_PROGBITS) {
            bValue += 100;
        }
        // 自定义段属性
        //// TO-DO 这里检测如果是命令行参数列表自定义的段名,其value直接设置为负数(自定义段排最后)
        // 
        return (aValue > bValue);
    };
    std::sort(needLinkedSections.begin(), needLinkedSections.end(), cmp);
    //
    //for (size_t i = 0; i < needLinkedSections.size(); i++) {
    //    printf("haha: %s\n", needLinkedSections[i]->name.c_str());
    //}
}

void Linker::buildSegmentList()
{
    bool hasTextSegment = false;                // 是否存在 .text
    bool hasRDataSegment = false;               // 是否存在 .rdata
    bool hasDataSegment = false;                // 是否存在 .data
    //
    bool hasInsertIData = false;                // 是否已经插入 .idata
    // lambada函数, 检测是否有.idata，有的话添加到.rdata中去
    auto checkAndInsertIData = [&]() {
        if (!hasInsertIData && linkedDynamicSymbolIndies.size() != 0) {
            hasInsertIData = true;
            if (hasRDataSegment) {
                ImageSegmentData imageData;
                imageData.useSectionData = false;
                imageData.dataType = IMAGE_SEGMENT_DATA_TYPE_IDATA;
                imageSegments[rdataSegmentIndex].dataInfoList.push_back(imageData);
            }
            else {
                ImageSegment rdataSegment;
                rdataSegment.segmentName = ".rdata";
                ImageSegmentData imageData;
                imageData.useSectionData = false;
                imageData.dataType = IMAGE_SEGMENT_DATA_TYPE_IDATA;
                rdataSegment.dataInfoList.push_back(imageData);
                rdataSegmentIndex = imageSegments.size();
                imageSegments.push_back(rdataSegment);
                hasRDataSegment = true;
            }
        }
    };
    //
    for (size_t i = 0; i < needLinkedSections.size(); i++) {
        ElfSection * sec = needLinkedSections[i];
        uint64_t index = (uint64_t) i;
        if (sec->flags & ELF_SECTION_FLAG_EXEC) {
            if (hasTextSegment) {
                ImageSegmentData imageData;
                imageData.needSectionIndex = index;
                imageSegments[textSegmentIndex].dataInfoList.push_back(imageData);
            }
            else {
                ImageSegment textSegment;
                textSegment.segmentName = ".text";
                ImageSegmentData imageData;
                imageData.needSectionIndex = index;
                textSegment.dataInfoList.push_back(imageData);
                textSegmentIndex = imageSegments.size();
                imageSegments.push_back(textSegment);
                hasTextSegment = true;
            }
            continue;
        }
        // 只读
        if (!(sec->flags & ELF_SECTION_FLAG_WRITE)) {
            if (hasRDataSegment) {
                ImageSegmentData imageData;
                imageData.needSectionIndex = index;
                imageSegments[rdataSegmentIndex].dataInfoList.push_back(imageData);
            }
            else {
                ImageSegment rdataSegment;
                rdataSegment.segmentName = ".rdata";
                ImageSegmentData imageData;
                imageData.needSectionIndex = index;
                rdataSegment.dataInfoList.push_back(imageData);
                rdataSegmentIndex = imageSegments.size();
                imageSegments.push_back(rdataSegment);
                hasRDataSegment = true;
            }
            continue;
        }
        // 如果到这里还没有出现 .rdata 的话，检测是否有 .idata 和 .edata，放入 .rdata 中
        checkAndInsertIData();

        //数据段
        if (hasDataSegment) {
            ImageSegmentData imageData;
            imageData.needSectionIndex = index;
            imageSegments[dataSegmentIndex].dataInfoList.push_back(imageData);
        }
        else {
            ImageSegment dataSegment;
            dataSegment.segmentName = ".data";
            ImageSegmentData imageData;
            imageData.needSectionIndex = index;
            dataSegment.dataInfoList.push_back(imageData);
            dataSegmentIndex = imageSegments.size();
            imageSegments.push_back(dataSegment);
            hasDataSegment = true;
        }
    }
    // 再次检查 .idata
    checkAndInsertIData();

}

static int atlink_main(int argc, char ** argv)
{
    if (argc > 1) {
        //try {
        //    
        //}
        //catch (...) {
        //    //
        //}
    }
    // printf("size = %d", sizeof(std::shared_ptr<FileInfo>));
    Linker linker;
    linker.inputList.addObject("F:\\mywork\\alloytools\\x64\\Debug\\win1.o");
    linker.inputList.addLibrary("F:\\mywork\\alloytools\\x64\\Debug\\kernel32.dll.sym");
    linker.inputList.addLibrary("F:\\mywork\\alloytools\\x64\\Debug\\user32.dll.sym");

    linker.scanInputObjects();
    // 解决符号
    linker.symbolDepend.addSymbol("mystart");
    linker.resolveDependences();
    // 
    // 核心思想
    // 先把所有的需要依赖的section拿出来排列
    // 配列顺序:
    //   1. 带 EXEC 标志的 (带 EXEC 标志的不能带W标志，带有的会自动去掉)
    //   2. 不带 WRITE 标志的数据段(PROGBITS 类型 - 已初始化数据-只读)
    //   3. 不带 WRITE 标志的数据段(NOBITS 类型 - 未初始化数据-只读)
    //   4. 带 WRITE 标志的数据段(PROGBITS 类型 - 已初始化数据)
    //   5. 带 WRITE 标志的数据段(NOBITS 类型 - 未初始化数据)
    //
    //   a. 自己组织 .idata 嵌入到 .rdata
    //   b. 自己组织 .reloc
    linker.initNeedLinkedSections();                // 把需要链接进映像文件的section集合在一起
    linker.sortNeedLinkedSections();
    linker.buildSegmentList();

    return 0;
}



int main(int argc, char ** argv)
{
    int ret;
#ifdef _WIN32
    int i;
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    const wchar_t * cmdline = GetCommandLineW();
    int argsSize;
    wchar_t ** wargList = CommandLineToArgvW(cmdline, &argsSize);
    char ** byteArgList = new char *[argsSize + 1];
    for (i = 0; i < argsSize; i++) {
        size_t wlen = wcslen(wargList[i]);
        size_t maxByteLen = (wlen + 1) * 4;             // in UTF-8, max size of one character is 4
        char * byteArg = new char[maxByteLen];
        BOOL usedDefaultChar;
        WideCharToMultiByte(CP_UTF8, 0, wargList[i], (int)(wlen + 1), byteArg, (int)maxByteLen, "_", &usedDefaultChar);
        byteArgList[i] = byteArg;
    }
    byteArgList[i] = nullptr;
    LocalFree(wargList);
    //
    ret = atlink_main(argsSize, byteArgList);

    for (i = 0; i < argsSize; i++) {
        delete[] byteArgList[i];
    }
    delete[] byteArgList;

#else
    ret = atlink_main(argc, argv);
#endif

    return ret;
}


