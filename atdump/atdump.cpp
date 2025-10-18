
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <Windows.h>
#endif
#include "common_io.h"

#define FILE_TYPE_ELF           1

static int checkFileType(const char * filename)
{
    unsigned char b[4] = {0};
    FILE * srcFile = fopen_utf8(filename, "rb");
    if (srcFile != nullptr) {
        fread(b, 1, 4, srcFile);
        fclose(srcFile);
        if (0 == strncmp((char*)b, "\177ELF", 4))
            return FILE_TYPE_ELF;
    }
    return -1;
}

void dumpElfFile(const char * filename);

static int atdump_main(int argc, char ** argv)
{
    if (argc > 1) {
        const char * srcName = argv[1];
        switch (checkFileType(srcName))
        {
        case FILE_TYPE_ELF:
            dumpElfFile(srcName);
            break;
        default:
            break;
        }
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
        WideCharToMultiByte(CP_UTF8, 0, wargList[i], (int)(wlen + 1), byteArg, (int)maxByteLen, "_", &usedDefaultChar);
        byteArgList[i] = byteArg;
    }
    byteArgList[i] = nullptr;
    LocalFree(wargList);
    //
    ret = atdump_main(argsSize, byteArgList);

    for (i = 0; i < argsSize; i++) {
        delete[] byteArgList[i];
    }
    delete[] byteArgList;

#else
    ret = atdump_main(argc, argv);
#endif

    return ret;
}

