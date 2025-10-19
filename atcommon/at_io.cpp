
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






