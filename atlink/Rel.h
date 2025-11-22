#pragma once

#include <stdint.h>
#include <string>
#include <vector>

class ElfRel
{
public:
    uint64_t        offset;
    int64_t         addend;
    uint32_t        symbolIndex;
    uint32_t        type;
};





