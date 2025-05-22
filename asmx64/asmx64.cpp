#include <stdio.h>
#ifdef _WIN32
#include <Windows.h>
#endif
#include <memory>
#include <string>
#include "asmx64.h"

using namespace std;

static int asm_main(int argc, char ** argv) 
{
    if (argc > 0) {
        unique_ptr<X64Assembler> asm1 = make_unique<X64Assembler>();
        asm1->assemble(argv[0]);
    }
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
        WideCharToMultiByte(CP_UTF8, 0, wargList[i], wlen + 1, byteArg, maxByteLen, "_", &usedDefaultChar);
        byteArgList[i] = byteArg;
    }
    byteArgList[i] = nullptr;
    LocalFree(wargList);
    //
    ret = asm_main(argsSize, byteArgList);
    
    for (i = 0; i < argsSize; i++) {
        delete[] byteArgList[i];
    }
    delete[] byteArgList;
    
#else
    ret = asm_main(argsSize, byteArgList);
#endif

    return ret;
}
