

#include <stdio.h>
#ifdef _WIN32
#include <Windows.h>
#endif

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
    return 0;
}



int main()
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
    ret = atlink_main(argsSize, byteArgList);
#endif

    return ret;
}


