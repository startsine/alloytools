
#include <string.h>
#include <stdio.h>
#include "common_io.h"

FILE * fopen_utf8(const char * filename, const char * mode)
{
    return fopen(filename, mode);
}

int fseek_long(FILE* fp, long long offset, int origin)
{
#ifdef _WIN32
    return _fseeki64(fp, offset, origin);
#else
    return fseek(fp, offset, origin);
#endif
}



