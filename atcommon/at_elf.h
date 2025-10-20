#pragma once

#include <stdint.h>

typedef struct
{
    uint32_t section_name;          // Section name 
    uint32_t section_type;          // Section type 
    uint64_t section_flags;         // Section attributes 
    uint64_t section_addr;          // Virtual address in memory 
    uint64_t section_offset;        // Offset in file 
    uint64_t section_size;          // Size of section 
    uint32_t section_link;          // Link to other section 
    uint32_t section_info;          // Miscellaneous information 
    uint64_t section_addralign;     // Address alignment boundary 
    uint64_t section_entsize;       // Size of entries, if section has table 
} Elf64SectionEntry;



