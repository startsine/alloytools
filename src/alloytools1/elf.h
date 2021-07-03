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


