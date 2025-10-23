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

#define ELF_SECTION_FLAG_WRITE          (1 << 0)
#define ELF_SECTION_FLAG_ALLOC          (1 << 1)
#define ELF_SECTION_FLAG_EXEC           (1 << 2)
#define ELF_SECTION_FLAG_MERGE          (1 << 4)
#define ELF_SECTION_FLAG_STRING         (1 << 5)
#define ELF_SECTION_FLAG_INFO_LINK      (1 << 6)
#define ELF_SECTION_FLAG_LINK_ORDER     (1 << 7)
#define ELF_SECTION_OS_SPECIFIC         (1 << 8)
#define ELF_SECTION_GROUP               (1 << 9)
#define ELF_SECTION_TLS                 (1 << 10)
#define ELF_SECTION_COMPRESSED          (1 << 11)
//#define ELF_SECTION_EXCLUDE             (1 << 18)
#define ELF_SECTION_GNU_RETAIN          (1 << 21)
#define ELF_SECTION_GNU_MBIND           (1 << 24)

typedef struct
{
    uint32_t pro_type;              // Type of segment 
    uint32_t pro_flags;             // Segment attributes 
    uint64_t pro_offset;            // Offset in file 
    uint64_t pro_vaddr;             // Virtual address in memory 
    uint64_t pro_paddr;             // Reserved 
    uint64_t pro_filesz;            // Size of segment in file 
    uint64_t pro_memsz;             // Size of segment in memory 
    uint64_t pro_align;             // Alignment of segment 
} Elf64ProgramEntry;




