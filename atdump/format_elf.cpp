#include <stdio.h>
#include "common_io.h"


static FILE * elf = nullptr;
long long elfStartOffset = 0;
long long elfTotalSize = 0;


static void dumpElfContent()
{

}


void dumpElfFile(const char * filename)
{
    printf("哈哈\n");
    dumpElfContent();
}

void dumpElfContent(const char * filename, long long start, long long elfSize)
{
    FILE * fp = fopen_utf8(filename, "rb");
    if (fp != nullptr) {
        elf = fp;
        elfStartOffset = start;
        elfTotalSize = elfSize;
        dumpElfContent();
        fclose(fp);
    }
    else {

    }
}














