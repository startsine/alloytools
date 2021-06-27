#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include "utils.h"

FILE* fopen_utf8_filename(const char* path, const char* mode)
{
#ifdef _WIN32
    FILE* fp;
    size_t i;
    size_t blen;
    size_t wlen;
    WCHAR* wBuffer;
    FILE * handle;
    WCHAR wmode[32];

    blen = strlen(path);
    wlen = (blen + 4) * sizeof(WCHAR);
    wBuffer = (WCHAR*)malloc(wlen * sizeof(WCHAR));

    MultiByteToWideChar(CP_UTF8, 0, path, -1, (LPWSTR)wBuffer, (int)wlen);

    for (i = 0; i < sizeof(wmode) / sizeof(WCHAR) - 1; i++) {
        wmode[i] = mode[i];
        if (mode[i] == 0)
            break;
    }
    wmode[sizeof(wmode) / sizeof(WCHAR) - 1] = 0;
    
    handle = _wfopen((wchar_t*)wBuffer, (wchar_t*)wmode);
    free(wBuffer);

    return handle;
#else
    return fopen(path, mode);
#endif // _WIN32
}








