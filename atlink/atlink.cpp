#include <stdio.h>
#include <memory>
#ifdef _WIN32
#include <Windows.h>
#endif
#include "FileInfo.h"

class Linker 
{
public:
    InputList       inputList;

    void scanInputObjects();
    void scanObject(SrcFile & fileInfo);
};

void Linker::scanInputObjects()
{
    for (size_t i = 0; i < inputList.fileList.size(); i++) {
        if (inputList.fileList[i]->fileType == FileType::ELF_OBJECT) {
            scanObject(*inputList.fileList[i]);
        }
    }
}

void Linker::scanObject(SrcFile & obj)
{
    char magic[4];
    uint8_t elfClass, elfData;
    obj.open();
    obj.seek(0, SEEK_SET);
    obj.fread(magic, 1, 4);
    elfClass = obj.read_u8();
    elfData = obj.read_u8();
    obj.isBigEndian = (elfData == 2);


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


