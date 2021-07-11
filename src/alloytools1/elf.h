#pragma once
#include <stdint.h>

#define EI_POS_CLASS 4
#define EI_POS_DATA 5
#define EI_POS_VERSION 6
#define EI_POS_OSABI 7
#define EI_POS_ABIVERSION 8

typedef struct {
    uint8_t         ident[16];
    uint16_t        type;
    uint16_t        machine;
    uint32_t        version;
    uint32_t        entry;
    uint32_t        phoff;
    uint32_t        shoff;
    uint32_t        flags;
    uint16_t        ehsize;
    uint16_t        phentsize;
    uint16_t        phnum;
    uint16_t        shentsize;
    uint16_t        shnum;
    uint16_t        shstrndx;
} ELF32Header;

typedef struct
{
    uint8_t         ident[16];          // ELF identification
    uint16_t        type;               // Object file type
    uint16_t        machine;            // Machine type
    uint32_t        version;            // Object file version
    uint64_t        entry;              // Entry point address
    uint64_t        phoff;              // Program header offset
    uint64_t        shoff;              // Section header offset
    uint32_t        flags;              // Processor-specific flags
    uint16_t        ehsize;             // ELF header size
    uint16_t        phentsize;          // Size of program header entry
    uint16_t        phnum;              // Number of program header entries
    uint16_t        shentsize;          // Size of section header entry
    uint16_t        shnum;              // Number of section header entries
    uint16_t        shstrndx;           // Section name string table index
} ELF64Header;

typedef struct {
    uint32_t        type;
    uint32_t        offset;
    uint32_t        vaddr;
    uint32_t        paddr;
    uint32_t        filesz;
    uint32_t        memsz;
    uint32_t        flags;
    uint32_t        align;
} ELF32ProgramHeader;

typedef struct
{
    uint32_t        type;               // Type of segment 
    uint32_t        flags;              // Segment attributes 
    uint64_t        offset;             // Offset in file 
    uint64_t        vaddr;              // Virtual address in memory 
    uint64_t        paddr;              // Reserved 
    uint64_t        filesz;             // Size of segment in file 
    uint64_t        memsz;              // Size of segment in memory 
    uint64_t        align;              // Alignment of segment 
} ELF64ProgramHeader;

#define ELF_PROGRAM_TYPE_LOAD           0x01
#define ELF_PROGRAM_TYPE_DYNAMIC        0x02

typedef struct {
    uint32_t        name;
    uint32_t        type;
    uint32_t        flags;
    uint32_t        addr;
    uint32_t        offset;
    uint32_t        size;
    uint32_t        link;
    uint32_t        info;
    uint32_t        addralign;
    uint32_t        entsize;
} Elf32SectionHeader;

typedef struct
{
    uint32_t        name;               // Section name 
    uint32_t        type;               // Section type 
    uint64_t        flags;              // Section attributes 
    uint64_t        addr;               // Virtual address in memory 
    uint64_t        offset;             // Offset in file 
    uint64_t        size;               // Size of section 
    uint32_t        link;               // Link to other section 
    uint32_t        info;               // Miscellaneous information 
    uint64_t        addralign;          // Address alignment boundary 
    uint64_t        entsize;            // Size of entries, if section has table 
} Elf64SectionHeader;

#define ELF_SECTION_TYPE_NULL           0x00
#define ELF_SECTION_TYPE_NOBITS         0x08

typedef struct {
    uint32_t        name;
    uint32_t        value;
    uint32_t        size;
    unsigned char   info;
    unsigned char   other;
    uint16_t        shndx;
} Elf32SymEntry;

typedef struct
{
    uint32_t        name;               // Symbol name 
    unsigned char   info;               // Type and Binding attributes 
    unsigned char   other;              // Reserved 
    uint16_t        shndx;              // Section table index 
    uint64_t        value;              // Symbol value 
    uint64_t        size;               // Size of object (e.g., common) 
} Elf64SymEntry;

typedef struct {
    int32_t         tag;
    union {
        uint32_t    val;
        uint32_t    ptr;
    } un;
} Elf32DynEntry;

typedef struct
{
    int64_t         tag;
    union {
        uint64_t    val;
        uint64_t    ptr;
    } un;
} Elf64DynEntry;

#define ELF_DT_NULL         0
#define ELF_DT_NEEDED       1
#define ELF_DT_PLTRELSZ     2
#define ELF_DT_PLTGOT       3
#define ELF_DT_HASH         4
#define ELF_DT_STRTAB       5
#define ELF_DT_SYMTAB       6
#define ELF_DT_RELA         7
#define ELF_DT_RELASZ       8
#define ELF_DT_RELAENT      9
#define ELF_DT_STRSZ        10
#define ELF_DT_SYMENT       11
#define ELF_DT_INIT         12
#define ELF_DT_FINI         13
#define ELF_DT_SONAME       14
#define ELF_DT_RPATH        15
#define ELF_DT_SYMBOLIC     16
#define ELF_DT_REL          17
#define ELF_DT_RELSZ        18
#define ELF_DT_RELENT       19
#define ELF_DT_PLTREL       20
#define ELF_DT_DEBUG        21
#define ELF_DT_TEXTREL      22
#define ELF_DT_JMPREL       23
#define ELF_DT_BIND_NOW     24
#define ELF_DT_INIT_ARRAY   25
#define ELF_DT_FINI_ARRAY   26
#define ELF_DT_INIT_ARRAYSZ 27
#define ELF_DT_FINI_ARRAYSZ 28
#define ELF_DT_LOOS         0x60000000
#define ELF_DT_GNU_HASH     0x6ffffef5
#define ELF_DT_HIOS         0x6FFFFFFF
#define ELF_DT_LOPROC       0x70000000
#define ELF_DT_HIPROC       0x7FFFFFFF


