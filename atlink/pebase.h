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

#define PE_NUMBEROF_DIRECTORY_ENTRIES 16

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
    PEDataDirectory dataDirectory[PE_NUMBEROF_DIRECTORY_ENTRIES];
};

//      PE_LIBRARY_PROCESS_INIT            0x0001     // Reserved.
//      PE_LIBRARY_PROCESS_TERM            0x0002     // Reserved.
//      PE_LIBRARY_THREAD_INIT             0x0004     // Reserved.
//      PE_LIBRARY_THREAD_TERM             0x0008     // Reserved.
#define PE_DLLCHARACTERISTICS_HIGH_ENTROPY_VA    0x0020  // Image can handle a high entropy 64-bit virtual address space.
#define PE_DLLCHARACTERISTICS_DYNAMIC_BASE 0x0040     // DLL can move.
#define PE_DLLCHARACTERISTICS_FORCE_INTEGRITY    0x0080     // Code Integrity Image
#define PE_DLLCHARACTERISTICS_NX_COMPAT    0x0100     // Image is NX compatible
#define PE_DLLCHARACTERISTICS_NO_ISOLATION 0x0200     // Image understands isolation and doesn't want it
#define PE_DLLCHARACTERISTICS_NO_SEH       0x0400     // Image does not use SEH.  No SE handler may reside in this image
#define PE_DLLCHARACTERISTICS_NO_BIND      0x0800     // Do not bind this image.
#define PE_DLLCHARACTERISTICS_APPCONTAINER 0x1000     // Image should execute in an AppContainer
#define PE_DLLCHARACTERISTICS_WDM_DRIVER   0x2000     // Driver uses WDM model
#define PE_DLLCHARACTERISTICS_GUARD_CF     0x4000     // Image supports Control Flow Guard.
#define PE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE     0x8000

#define PE_DIRECTORY_ENTRY_EXPORT          0   // Export Directory
#define PE_DIRECTORY_ENTRY_IMPORT          1   // Import Directory
#define PE_DIRECTORY_ENTRY_RESOURCE        2   // Resource Directory
#define PE_DIRECTORY_ENTRY_EXCEPTION       3   // Exception Directory
#define PE_DIRECTORY_ENTRY_SECURITY        4   // Security Directory
#define PE_DIRECTORY_ENTRY_BASERELOC       5   // Base Relocation Table
#define PE_DIRECTORY_ENTRY_DEBUG           6   // Debug Directory
//      PE_DIRECTORY_ENTRY_COPYRIGHT       7   // (X86 usage)
#define PE_DIRECTORY_ENTRY_ARCHITECTURE    7   // Architecture Specific Data
#define PE_DIRECTORY_ENTRY_GLOBALPTR       8   // RVA of GP
#define PE_DIRECTORY_ENTRY_TLS             9   // TLS Directory
#define PE_DIRECTORY_ENTRY_LOAD_CONFIG    10   // Load Configuration Directory
#define PE_DIRECTORY_ENTRY_BOUND_IMPORT   11   // Bound Import Directory in headers
#define PE_DIRECTORY_ENTRY_IAT            12   // Import Address Table
#define PE_DIRECTORY_ENTRY_DELAY_IMPORT   13   // Delay Load Import Descriptors
#define PE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   // COM Runtime descriptor

struct PESection {
    char            name[8];
    union {
        uint32_t    physicalAddress;
        uint32_t    virtualSize;
    } Misc;
    uint32_t        virtualAddress;
    uint32_t        sizeOfRawData;
    uint32_t        pointerToRawData;
    uint32_t        pointerToRelocations;
    uint32_t        pointerToLinenumbers;
    uint16_t        numberOfRelocations;
    uint16_t        numberOfLinenumbers;
    uint32_t        characteristics;
};







