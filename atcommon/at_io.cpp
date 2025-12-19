
#include <string.h>
#include <stdio.h>
#ifdef _WIN32
#include <io.h>
#endif
#include "at_io.h"

FILE * fopen_utf8(const char * filename, const char * mode)
{
#ifdef _WIN32
    return fopen(filename, mode);
#else
#endif
}

int fseek_long(FILE* fp, long long offset, int origin)
{
#ifdef _WIN32
    return _fseeki64(fp, offset, origin);
#else
    return fseek(fp, offset, origin);
#endif
}

long long get_stdio_file_size(FILE * fp)
{
    return _filelengthi64(_fileno(fp));
}

long long get_file_size(const char * filename) 
{
#ifdef _WIN32
    FILE * fp = fopen(filename, "rb");
    if (fp != nullptr) {
        long long size = get_stdio_file_size(fp);
        fclose(fp);
        return size;
    }
    else {
        return 0;
    }
#else
    ////
#endif
}

long long get_file_curr_pointer(FILE * fp)
{
#ifdef _WIN32
    return _ftelli64(fp);
#else
    return ftell(fp);
#endif
}

static uint32_t testValue = 0x12345678;

uint16_t to_le16(uint16_t a)
{
    uint32_t * p32 = &testValue;
    uint8_t * p8 = (uint8_t*) p32;
    if (*p8 == 0x78) {
        return a;
    }
    uint16_t a0 = a & 0xff;
    uint16_t a1 = a >> 8;
    return (a0 << 8) | a1;
}

uint32_t to_le32(uint32_t a)
{
    uint32_t * p32 = &testValue;
    uint8_t * p8 = (uint8_t*)p32;
    if (*p8 == 0x78) {
        return a;
    }
    uint32_t a0 = a & 0xff;
    uint32_t a1 = (a >> 8) & 0xff;
    uint32_t a2 = (a >> 16) & 0xff;
    uint32_t a3 = (a >> 24) & 0xff;
    return (a0 << 24) | (a1 << 16) | (a2 << 8) | a3;
}

uint64_t to_le64(uint64_t a)
{
    uint32_t * p32 = &testValue;
    uint8_t * p8 = (uint8_t*)p32;
    if (*p8 == 0x78) {
        return a;
    }
    uint64_t a0 = a & 0xff;
    uint64_t a1 = (a >> 8) & 0xff;
    uint64_t a2 = (a >> 16) & 0xff;
    uint64_t a3 = (a >> 24) & 0xff;
    uint64_t a4 = (a >> 32) & 0xff;
    uint64_t a5 = (a >> 40) & 0xff;
    uint64_t a6 = (a >> 48) & 0xff;
    uint64_t a7 = (a >> 56) & 0xff;
    return (a0 << 56) | (a1 << 48) | (a2 << 40) | (a3 << 32) | (a4 << 24) | (a5 << 16) | (a6 << 8) | a7;
}

uint16_t get_value_from_le16(uint8_t * p)
{
    return ((*(p + 1)) << 8) | (*p);
}

uint32_t get_value_from_le32(uint8_t * p)
{
    return ((*(p + 3)) << 24) | ((*(p + 2)) << 16) | ((*(p + 1)) << 8) | (*p);
}

uint64_t get_value_from_le64(uint8_t * p)
{
    return ((*(p + 7)) << 56) | ((*(p + 6)) << 48) | ((*(p + 5)) << 40) | ((*(p + 4)) << 32) | ((*(p + 3)) << 24) | ((*(p + 2)) << 16) | ((*(p + 1)) << 8) | (*p);
}

void put_value_to_le16(uint8_t * p, uint16_t v)
{
    *(p + 0) = v & 0xff;
    *(p + 1) = (v >> 8) & 0xff;
}

void put_value_to_le32(uint8_t * p, uint32_t v)
{
    *(p + 0) = v & 0xff;
    *(p + 1) = (v >> 8) & 0xff;
    *(p + 2) = (v >> 16) & 0xff;
    *(p + 3) = (v >> 24) & 0xff;
}

void put_value_to_le64(uint8_t * p, uint64_t v)
{
    *(p + 0) = v & 0xff;
    *(p + 1) = (v >> 8) & 0xff;
    *(p + 2) = (v >> 16) & 0xff;
    *(p + 3) = (v >> 24) & 0xff;
    *(p + 4) = (v >> 32) & 0xff;
    *(p + 5) = (v >> 40) & 0xff;
    *(p + 6) = (v >> 48) & 0xff;
    *(p + 7) = (v >> 56) & 0xff;
}





