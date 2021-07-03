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

void file_seek(FILE* fp, int64_t offset, int32_t origin)
{
#ifdef _WIN32
    _fseeki64(fp, offset, origin);
#else
    fseek(fp, offset, origin);
#endif
}

uint16_t fread_u16(FILE* fp)
{
    unsigned char c1, c2;
    unsigned char buffer[2];
    fread(buffer, 1, 2, fp);
    c1 = (unsigned char)buffer[0];
    c2 = (unsigned char)buffer[1];
    return (c2 << 8) | c1;
}

uint32_t fread_u32(FILE* fp)
{
    unsigned char c1, c2, c3, c4;
    unsigned char buffer[4];
    fread(buffer, 1, 4, fp);
    c1 = (unsigned char)buffer[0];
    c2 = (unsigned char)buffer[1];
    c3 = (unsigned char)buffer[2];
    c4 = (unsigned char)buffer[3];
    return (c4 << 24) | (c3 << 16) | (c2 << 8) | c1;
}

uint64_t fread_u64(FILE* fp)
{
    unsigned char c1, c2, c3, c4;
    unsigned char c5, c6, c7, c8;
    uint64_t a1, a2;
    unsigned char buffer[8];
    fread(buffer, 1, 8, fp);
    c1 = (unsigned char)buffer[0];
    c2 = (unsigned char)buffer[1];
    c3 = (unsigned char)buffer[2];
    c4 = (unsigned char)buffer[3];
    a1 = (c4 << 24) | (c3 << 16) | (c2 << 8) | c1;
    c5 = (unsigned char)buffer[4];
    c6 = (unsigned char)buffer[5];
    c7 = (unsigned char)buffer[6];
    c8 = (unsigned char)buffer[7];
    a2 = (c8 << 24) | (c7 << 16) | (c6 << 8) | c5;
    return (a2 << 32) | a1;
}

size_t fwrite_u16(FILE* fp, uint16_t value)
{
    unsigned char buffer[2];
    buffer[0] = value & 0xff;
    buffer[1] = (value >> 8) | 0xff;
    fwrite(buffer, 1, 2, fp);
    return 0;
}

size_t fwrite_u32(FILE* fp, uint32_t value)
{
    unsigned char buffer[4];
    buffer[0] = value & 0xff;
    buffer[1] = (value >> 8) | 0xff;
    buffer[2] = (value >> 16) | 0xff;
    buffer[3] = (value >> 24) | 0xff;
    fwrite(buffer, 1, 4, fp);
    return 0;
}

size_t fwrite_u64(FILE* fp, uint64_t value)
{
    unsigned char buffer[8];
    unsigned int part1, part2;
    part1 = (unsigned int)(value & 0xffffffff);
    part2 = (unsigned int)((value >> 32) & 0xffffffff);
    buffer[0] = part1 & 0xff;
    buffer[1] = (part1 >> 8) | 0xff;
    buffer[2] = (part1 >> 16) | 0xff;
    buffer[3] = (part1 >> 24) | 0xff;
    buffer[4] = part2 & 0xff;
    buffer[5] = (part2 >> 8) | 0xff;
    buffer[6] = (part2 >> 16) | 0xff;
    buffer[7] = (part2 >> 24) | 0xff;
    fwrite(buffer, 1, 8, fp);
    return 0;
}




