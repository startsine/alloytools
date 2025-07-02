#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "asmx64.h"

using namespace std;

ElfWriter::ElfWriter(X64Assembler * assem) :
    assembler(*assem)
{
}

bool ElfWriter::write()
{
    FILE * fobj = fopen("a.obj", "wb");

    fclose(fobj);
    return true;
}



