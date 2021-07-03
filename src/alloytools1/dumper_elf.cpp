#include <stdio.h>
#include <string.h>
#include <string>
#include <memory>
#include "dumper.h"
#include "utils.h"

int ElfDumper::dumpContent(FILE* fp, size_t start, size_t size, const DumperParam& param)
{
    uint8_t ident[16];

    file_seek(fp, start, SEEK_SET);
    memset(ident, 0, sizeof(ident));
    fread(ident, 1, sizeof(ident), fp);



    return 0;
}


