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

#define ELF_SEGMENT_FLAG_EXECUTABLE     (1 << 0)
#define ELF_SEGMENT_FLAG_WRITABLE       (1 << 1)
#define ELF_SEGMENT_FLAG_READABLE       (1 << 2)

#define ELF_SEGMENT_TYPE_NULL 0
#define ELF_SEGMENT_TYPE_LOAD 1
#define ELF_SEGMENT_TYPE_DYNAMIC 2
#define ELF_SEGMENT_TYPE_INTERP 3
#define ELF_SEGMENT_TYPE_NOTE 4
#define ELF_SEGMENT_TYPE_SHLIB 5
#define ELF_SEGMENT_TYPE_PHDR 6
#define ELF_SEGMENT_TYPE_TLS 7
#define ELF_SEGMENT_TYPE_NUM 8
#define ELF_SEGMENT_TYPE_LOOS		0x60000000	// OS-specific start id
// sun
#define ELF_SEGMENT_TYPE_SUNW_UNWIND  (ELF_SEGMENT_TYPE_LOOS + 0x464e550)
// gnu
#define ELF_SEGMENT_TYPE_GNU_EH_FRAME	(ELF_SEGMENT_TYPE_LOOS + 0x474e550) // Frame unwind information 
#define ELF_SEGMENT_TYPE_SUNW_EH_FRAME ELF_SEGMENT_TYPE_GNU_EH_FRAME      // Solaris uses the same value 
#define ELF_SEGMENT_TYPE_GNU_STACK	(ELF_SEGMENT_TYPE_LOOS + 0x474e551) // Stack flags 
#define ELF_SEGMENT_TYPE_GNU_RELRO	(ELF_SEGMENT_TYPE_LOOS + 0x474e552) // Read-only after relocation 
#define ELF_SEGMENT_TYPE_GNU_PROPERTY	(ELF_SEGMENT_TYPE_LOOS + 0x474e553) // GNU property 
#define ELF_SEGMENT_TYPE_GNU_SFRAME	(ELF_SEGMENT_TYPE_LOOS + 0x474e554) // SFrame stack trace information 
//OpenBSD
#define ELF_SEGMENT_TYPE_OPENBSD_MUTABLE   (ELF_SEGMENT_TYPE_LOOS + 0x5a3dbe5)  // Like bss, but not immutable
#define ELF_SEGMENT_TYPE_OPENBSD_RANDOMIZE (ELF_SEGMENT_TYPE_LOOS + 0x5a3dbe6)  // Fill with random data
#define ELF_SEGMENT_TYPE_OPENBSD_WXNEEDED  (ELF_SEGMENT_TYPE_LOOS + 0x5a3dbe7)  // Program does W^X violations
#define ELF_SEGMENT_TYPE_OPENBSD_NOBTCFI   (ELF_SEGMENT_TYPE_LOOS + 0x5a3dbe8)  // No branch target CFI
#define ELF_SEGMENT_TYPE_OPENBSD_SYSCALLS  (ELF_SEGMENT_TYPE_LOOS + 0x5a3dbe9)  // System call sites
#define ELF_SEGMENT_TYPE_OPENBSD_BOOTDATA  (ELF_SEGMENT_TYPE_LOOS + 0x5a41be6)  // Section for boot arguments
// sun
#define ELF_SEGMENT_TYPE_SUNWBSS	(ELF_SEGMENT_TYPE_LOOS + 0xffffffa)	        // Sun Specific segment
#define ELF_SEGMENT_TYPE_SUNWSTACK	(ELF_SEGMENT_TYPE_LOOS + 0xffffffb)	        // Stack segment.  
#define ELF_SEGMENT_TYPE_SUNWDTRACE   (ELF_SEGMENT_TYPE_LOOS + 0xffffffc)
#define ELF_SEGMENT_TYPE_SUNWCAP      (ELF_SEGMENT_TYPE_LOOS + 0xffffffd)


