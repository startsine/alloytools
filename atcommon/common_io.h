
#ifndef COMMON_IO_H_B96DA686CA40FC17
#define COMMON_IO_H_B96DA686CA40FC17

#include <stdio.h>

FILE * fopen_utf8(const char * filename, const char * mode);
int fseek_long(FILE* fp, long long offset, int origin);

#endif // COMMON_IO_H_B96DA686CA40FC17
