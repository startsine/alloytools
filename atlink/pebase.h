#pragma once

#include <stdint.h>

struct COFFFileHeader 
{
    char        signature[4];
    uint16_t    machine;
    uint16_t    numberOfSections;
    uint32_t    timeDateStamp;
    uint32_t    pointerToSymbolTable;
    uint32_t    numberOfSymbols;
    uint16_t    sizeOfOptionalHeader;
    uint16_t    characteristics;
};

#define PE_FILE_RELOCS_STRIPPED           0x0001  // Relocation info stripped from file.
#define PE_FILE_EXECUTABLE_IMAGE          0x0002  // File is executable  (i.e. no unresolved external references).
#define PE_FILE_LINE_NUMS_STRIPPED        0x0004  // Line nunbers stripped from file.
#define PE_FILE_LOCAL_SYMS_STRIPPED       0x0008  // Local symbols stripped from file.
#define PE_FILE_AGGRESIVE_WS_TRIM         0x0010  // Aggressively trim working set
#define PE_FILE_LARGE_ADDRESS_AWARE       0x0020  // App can handle >2gb addresses
#define PE_FILE_BYTES_REVERSED_LO         0x0080  // Bytes of machine word are reversed.
#define PE_FILE_32BIT_MACHINE             0x0100  // 32 bit word machine.
#define PE_FILE_DEBUG_STRIPPED            0x0200  // Debugging info stripped from file in .DBG file
#define PE_FILE_REMOVABLE_RUN_FROM_SWAP   0x0400  // If Image is on removable media, copy and run from the swap file.
#define PE_FILE_NET_RUN_FROM_SWAP         0x0800  // If Image is on Net, copy and run from the swap file.
#define PE_FILE_SYSTEM                    0x1000  // System File.
#define PE_FILE_DLL                       0x2000  // File is a DLL.
#define PE_FILE_UP_SYSTEM_ONLY            0x4000  // File should only be run on a UP machine
#define PE_FILE_BYTES_REVERSED_HI         0x8000  // Bytes of machine word are reversed.

struct PEDataDirectory
{
    uint32_t    virtualAddress;
    uint32_t    size;
};

struct OptionalHeader64 {
    uint16_t        magic;
    uint8_t         majorLinkerVersion;
    uint8_t         minorLinkerVersion;
    uint32_t        sizeOfCode;
    uint32_t        sizeOfInitializedData;
    uint32_t        sizeOfUninitializedData;
    uint32_t        addressOfEntryPoint;
    uint32_t        baseOfCode;
    uint64_t        imageBase;
    uint32_t        sectionAlignment;
    uint32_t        fileAlignment;
    uint16_t        majorOperatingSystemVersion;
    uint16_t        minorOperatingSystemVersion;
    uint16_t        majorImageVersion;
    uint16_t        minorImageVersion;
    uint16_t        majorSubsystemVersion;
    uint16_t        minorSubsystemVersion;
    uint32_t        win32VersionValue;
    uint32_t        sizeOfImage;
    uint32_t        sizeOfHeaders;
    uint32_t        checkSum;
    uint16_t        subsystem;
    uint16_t        dllCharacteristics;
    uint64_t        sizeOfStackReserve;
    uint64_t        sizeOfStackCommit;
    uint64_t        sizeOfHeapReserve;
    uint64_t        sizeOfHeapCommit;
    uint32_t        loaderFlags;
    uint32_t        numberOfRvaAndSizes;
    PEDataDirectory dataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
};


