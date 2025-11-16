#include <stdio.h>
#include <memory>
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
    bool dependSymbolsIsEmpty;
    while (true)
    {
        std::string depSymbolName = symbolDepend.getSymbol(dependSymbolsIsEmpty);
        if (depSymbolName != "") {
            resolveSymbol(depSymbolName);
        }
        if (dependSymbolsIsEmpty) {
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
        auto flatSectionIndex = sym->flatSectionIndex;
        auto pSection = flatSections.sections[flatSectionIndex];
        if (!pSection->used) {

        }
    }
    else {
        //// TO-DO  报错
    }
}

void Linker::resolveSection(ElfSection * pSection)
{
    pSection->used = true;
    auto fileIndex = pSection->fileIndex;       // section 所在的文件在 InputList 中的索引
    auto localIndex = pSection->localIndex;     // section 在 object 文件中的原始索引
    auto file = inputList.fileList[fileIndex];
    if (file->fileType == FileType::ELF_OBJECT) {
        auto obj = dynamic_cast<ObjectFile*>(file);
        auto startIndex = obj->startIndexOfFlatSections;
        auto totalSectionNum = obj->shnum;
        // 找出该 section 对应的 重定位信息 
        for (size_t idx = startIndex; idx < (startIndex + totalSectionNum); idx++) {
            auto sec = flatSections.sections[idx];
            if (sec->info == localIndex && (sec->type == ELF_SECTION_TYPE_RELA || sec->type == ELF_SECTION_TYPE_REL)) {
                pSection->relocs = std::make_shared<std::list<ElfRel> > ();
                obj->loadRelocTable(*sec, pSection->relocs.get());
                break;
            }
        }
    }
    

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
    //
    linker.symbolDepend.addSymbol("mystart");
    linker.resolveDependences();
    

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


