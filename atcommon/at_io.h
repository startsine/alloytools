
#ifndef COMMON_IO_H_B96DA686CA40FC17
#define COMMON_IO_H_B96DA686CA40FC17

#include <stdio.h>

FILE * fopen_utf8(const char * filename, const char * mode);
int fseek_long(FILE* fp, long long offset, int origin);
long long get_stdio_file_size(FILE * fp);
long long get_file_size(const char * filename);
long long get_file_curr_pointer(FILE * fp);

#endif // COMMON_IO_H_B96DA686CA40FC17
