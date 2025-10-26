#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <memory>
#include "at_io.h"
#include "at_elf.h"
#include "at_util.h"

class DynEntryShowInfo {
public:
    std::string  showText;
    enum {
        SHOW_INT,
        SHOW_HEX32,
        SHOW_HEX64,
        SHOW_HEX,
        SHOW_BYTE_SIZE,
        SHOW_CUSTOM,
    } showType;
};

class ElfParser {
private:
    FILE * elf;
    const long long elfStartOffset;
    const long long elfTotalSize;
    bool isBigEndian;                           
    bool isElf64;
    int elfType;
    unsigned short stringTableSectionIndex = 0;
    uint64_t sectionTableOffset = 0;
    uint16_t sectionEntryCount = 0;
    uint64_t programTableOffset = 0;
    uint16_t programEntryCount = 0;
    std::vector<Elf64SectionEntry> sectionTable64;
    std::vector<const char * > sectionNameList;
    char * shstrtab = nullptr;
    uint64_t shStrTabSize = 0;
    std::vector<Elf64ProgramEntry> programTable64;
    std::vector<Elf64DynEntry> dynEntries64;
    char * dynstr = nullptr;
    uint64_t dynStrSize = 0;

private:

    uint64_t addrToFileOffset(uint64_t addr) {
        for (size_t i = 0; i < programTable64.size(); i++) {
            const Elf64ProgramEntry & entry = programTable64[i];
            if (ELF_SEGMENT_TYPE_LOAD == entry.pro_type) {
                if (addr >= entry.pro_vaddr && addr < (entry.pro_vaddr + entry.pro_memsz)) {
                    size_t delta = addr - entry.pro_vaddr;
                    size_t newOffset = delta + entry.pro_offset;
                    if (newOffset < (entry.pro_offset + entry.pro_filesz)) {
                        return newOffset;
                    }
                }
            }
        }
        //
        for (size_t i = 0; i < programTable64.size(); i++) {
            const Elf64ProgramEntry & entry = programTable64[i];
            if (addr >= entry.pro_vaddr && addr < (entry.pro_vaddr + entry.pro_memsz)) {
                size_t delta = addr - entry.pro_vaddr;
                size_t newOffset = delta + entry.pro_offset;
                if (newOffset < (entry.pro_offset + entry.pro_filesz)) {
                    return newOffset;
                }
            }
        }
        return 0;
    }

    size_t fread(void* buffer, size_t eSize, size_t eCount) {
        long long curr = get_file_curr_pointer(elf);
        if (curr + eSize * eCount > elfStartOffset + elfTotalSize) {
            throw(std::string("out of file"));
        }
        size_t n = ::fread(buffer, eSize, eCount, elf);
        return n;
    }

    unsigned short read_u16() {
        long long curr = get_file_curr_pointer(elf);
        if (curr + 2 > elfStartOffset + elfTotalSize) {
            throw(std::string("out of file"));
        }
        unsigned int a0 = fgetc(elf);
        unsigned int a1 = fgetc(elf);
        if (isBigEndian)
            return static_cast<unsigned short>((a0 << 8) | a1);
        return static_cast<unsigned short>((a1 << 8) | a0);
    }

    unsigned int read_u32() {
        long long curr = get_file_curr_pointer(elf);
        if (curr + 4 > elfStartOffset + elfTotalSize) {
            throw(std::string("out of file"));
        }
        unsigned int a0 = fgetc(elf);
        unsigned int a1 = fgetc(elf);
        unsigned int a2 = fgetc(elf);
        unsigned int a3 = fgetc(elf);
        if (isBigEndian)
            return ((a0 << 24) | (a1 << 16) | (a2 << 8) | a3);
        return ((a3 << 24) | (a2 << 16) | (a1 << 8) | a0);
    }

    unsigned long long read_u64() {
        long long curr = get_file_curr_pointer(elf);
        if (curr + 8 > elfStartOffset + elfTotalSize) {
            throw(std::string("out of file"));
        }
        unsigned long long a0 = fgetc(elf);
        unsigned long long a1 = fgetc(elf);
        unsigned long long a2 = fgetc(elf);
        unsigned long long a3 = fgetc(elf);
        unsigned long long a4 = fgetc(elf);
        unsigned long long a5 = fgetc(elf);
        unsigned long long a6 = fgetc(elf);
        unsigned long long a7 = fgetc(elf);
        if (isBigEndian)
            return ((a0 << 56) | (a1 << 48) | (a2 << 40) | (a3 << 32) | (a4 << 24) | (a5 << 16) | (a6 << 8) | a7);
        return ((a7 << 56) | (a6 << 48) | (a5 << 40) | (a4 << 32) | (a3 << 24) | (a2 << 16) | (a1 << 8) | a0);
    }

    std::string readString() {
        std::vector<char> s;
        char ch;
        do {
            ch = (char) fgetc(elf);
            s.push_back(ch);
        } while (ch != 0);
        return std::string(s.data(), s.size());
    }

    int elfSeek(long long offset, int origin) {
        int v = fseek_long(elf, offset, origin);
        if (feof(elf)) {
            throw(std::string("out of file"));
        }
        long long curr = get_file_curr_pointer(elf);
        if (curr > elfStartOffset + elfTotalSize) {
            throw(std::string("out of file"));
        }
        return v;
    }

    const char * getSectionString(uint64_t offset) {
        if (shstrtab == nullptr)  return "";
        if (offset >= shStrTabSize) return "";
        const char * str = &shstrtab[offset];
        return str;
    }

    const char * getSectionTypeDesc(uint32_t type) {
        static char unknownText[32];
        if (type < 0x60000000) {
            switch (type)
            {
            case 0:
                return "NULL";
            case 1:
                return "PROGBITS";
            case 2:
                return "SYMTAB";
            case 3:
                return "STRTAB";
            case 4:
                return "RELA";
            case 5:
                return "HASH";
            case 6:
                return "DYNAMIC";
            case 7:
                return "NOTE";
            case 8:
                return "NOBITS";
            case 9:
                return "REL";
            case 10:
                return "SHLIB";
            case 11:
                return "DYNSYM";
            case 14:
                return "INIT_ARRAY";
            case 15:
                return "FINI_ARRAY";
            case 16:
                return "PREINIT_ARRAY";
            case 17:
                return "GROUP";
            case 18:
                return "SYMTAB_SHNDX";
            case 19:
                return "RELR";
            default:
                snprintf(unknownText, sizeof(unknownText), "unknown-0x%08x", type);
                return unknownText;
            }
        }
        else if (type >= 0x60000000 && type <= 0x60000000) {
            snprintf(unknownText, sizeof(unknownText), "OS-0x%08x", type);
            return unknownText;
        }
        else if (type >= 0x70000000 && type <= 0x7FFFFFFF) {
            snprintf(unknownText, sizeof(unknownText), "PROC-0x%08x", type);
            return unknownText;
        }
        snprintf(unknownText, sizeof(unknownText), "unknown-0x%08x", type);
        return unknownText;
    }

    const char * getProgramTypeDesc(uint32_t type) {
        static char unknownText[32];
        if (type < 0x60000000) {
            switch (type)
            {
            case ELF_SEGMENT_TYPE_NULL:
                return "NULL";
            case ELF_SEGMENT_TYPE_LOAD:
                return "LOAD";
            case ELF_SEGMENT_TYPE_DYNAMIC:
                return "DYNAMIC";
            case ELF_SEGMENT_TYPE_INTERP:
                return "INTERP";
            case ELF_SEGMENT_TYPE_NOTE:
                return "NOTE";
            case ELF_SEGMENT_TYPE_SHLIB:
                return "SHLIB";
            case ELF_SEGMENT_TYPE_PHDR:
                return "PHDR";
            case ELF_SEGMENT_TYPE_TLS:
                return "TLS";
            case ELF_SEGMENT_TYPE_NUM:
                return "NUM";                   //umber of defined types
            default:
                snprintf(unknownText, sizeof(unknownText), "unknown-0x%08x", type);
                return unknownText;
            }
        }
        else if (type >= 0x60000000 && type <= 0x6FFFFFFF) {
            switch (type) {
            case ELF_SEGMENT_TYPE_SUNW_UNWIND:
                return "SUNW_UNWIND";
            case ELF_SEGMENT_TYPE_GNU_EH_FRAME:
                return "EH_FRAME";
            case ELF_SEGMENT_TYPE_GNU_STACK:
                return "GNU_STACK";
            case ELF_SEGMENT_TYPE_GNU_RELRO:
                return "GNU_RELRO";
            case ELF_SEGMENT_TYPE_GNU_PROPERTY:
                return "GNU_PROPERTY";
            case ELF_SEGMENT_TYPE_GNU_SFRAME:
                return "GNU_SFRAME";
            case ELF_SEGMENT_TYPE_OPENBSD_MUTABLE:
                return "OPENBSD_MUTABLE";
            case ELF_SEGMENT_TYPE_OPENBSD_RANDOMIZE:
                return "OPENBSD_RANDOMIZE";
            case ELF_SEGMENT_TYPE_OPENBSD_WXNEEDED:
                return "OPENBSD_WXNEEDED";
            case ELF_SEGMENT_TYPE_OPENBSD_NOBTCFI:
                return "OPENBSD_NOBTCFI";
            case ELF_SEGMENT_TYPE_OPENBSD_SYSCALLS:
                return "OPENBSD_SYSCALLS";
            case ELF_SEGMENT_TYPE_OPENBSD_BOOTDATA:
                return "OPENBSD_BOOTDATA";
            case ELF_SEGMENT_TYPE_SUNWBSS:
                return "SUNWBSS";
            case ELF_SEGMENT_TYPE_SUNWSTACK:
                return "SUNWSTACK";
            case ELF_SEGMENT_TYPE_SUNWDTRACE:
                return "SUNWDTRACE";
            case ELF_SEGMENT_TYPE_SUNWCAP:
                return "SUNWCAP";
            default:
                snprintf(unknownText, sizeof(unknownText), "OS-0x%08x", type);
                return unknownText;
            }
        }
        else if (type >= 0x70000000 && type <= 0x7FFFFFFF) {
            snprintf(unknownText, sizeof(unknownText), "PROC-0x%08x", type);
            return unknownText;
        }
        snprintf(unknownText, sizeof(unknownText), "unknown-0x%08x", type);
        return unknownText;
    }

    void dumpElfHeader() {
        unsigned char ident[16];
        fread(ident, 1, 16);
        // EI_CLASS == 4
        const char * bitSize = "未知";
        isElf64 = true;
        if (ident[4] == 1) {
            bitSize = "32位";
            isElf64 = false;
        }
        else if (ident[4] == 2) {
            bitSize = "64位";
        }
        printf("CPU架构位数:         %s\n", bitSize);
        // EI_DATA == 5
        const char * endianDesc = "未知";
        if (ident[5] == 1) {
            endianDesc = "little-endian";
            isBigEndian = false;
        }
        else if (ident[5] == 2) {
            endianDesc = "big-endian";
            isBigEndian = true;
        }
        printf("CPU数据排列:         %s\n", endianDesc);
        // EI_VERSION == 6
        printf("ELF 版本:         %d\n", ident[6]);
        // EI_OSABI == 7
        const char * abiType = "未知";
        switch (ident[7]) {
        case 0:
            abiType = "System V";
            break;
        case 1:
            abiType = "HP-UX";
            break;
        }
        printf("ABI:         %s\n", abiType);
        // EI_OSABI == 8
        printf("ABI 版本:         %d\n", ident[8]);

        printf("----\n");

        // 文件类型: e_type
        unsigned short e_type = read_u16();
        const char * eTypeStr = "未知";
        switch (e_type) {
        case 1:
            eTypeStr = "object";
            break;
        case 2:
            eTypeStr = "可执行文件";
            break;
        case 3:
            eTypeStr = "动态库";
            break;
        case 4:
            eTypeStr = "Core File";
            break;
        }
        printf("文件类型:         %s\n", eTypeStr);
        elfType = e_type;

        // CPU类型: e_machine
        unsigned short e_machine = read_u16();
        const char * eCpuTypeStr = "未知";
        switch (e_machine) {
        case 1:
            eCpuTypeStr = "xxxx";
            break;
        }
        printf("CPU类型:         %s\n", eCpuTypeStr);

        // 目标文件版本: e_version
        unsigned int e_version = read_u32();
        printf("目标文件版本:         %u\n", e_version);

        // 程序入口点 : e_entry
        unsigned long long e_entry = read_u64();
        printf("程序入口地址:         0x%016llx\n", e_entry);

        // 程序头表位置: e_phoff
        unsigned long long e_phoff = read_u64();
        printf("程序头表位置:         0x%016llx\n", e_phoff);
        programTableOffset = e_phoff;

        // 节表头位置: e_shoff
        unsigned long long e_shoff = read_u64();
        printf("节表头位置:         0x%016llx\n", e_shoff);
        sectionTableOffset = e_shoff;

        // 处理器特殊标志: e_flags
        unsigned int e_flags = read_u32();
        printf("处理器特殊标志:         0x%08u\n", e_flags);

        // ELF头大小: e_ehsize
        unsigned short e_ehsize = read_u16();
        printf("ELF 头大小:         %d\n", (int)e_ehsize);

        // 程序头表项大小: e_phentsize
        unsigned short e_phentsize = read_u16();
        printf("程序头表项大小:         %d\n", (int)e_phentsize);

        // 程序头表项个数: e_phnum
        unsigned short e_phnum = read_u16();
        printf("程序头表项个数:         %d\n", (int)e_phnum);
        programEntryCount = e_phnum;

        // 节表项大小: e_shentsize
        unsigned short e_shentsize = read_u16();
        printf("节表项大小:         %d\n", (int)e_shentsize);

        // 节表项个数: e_shnum
        unsigned short e_shnum = read_u16();
        printf("节表项个数:         %d\n", (int)e_shnum);
        sectionEntryCount = e_shnum;

        // 字符串表节的索引: e_shstrndx
        unsigned short e_shstrndx = read_u16();
        printf("字符串表节的索引:         %d\n", (int)e_shstrndx);
        stringTableSectionIndex = e_shstrndx;
    }

    void dumpSectionHeader() {
        if (sectionEntryCount == 0 || sectionTableOffset == 0) return;
        elfSeek((int64_t)sectionTableOffset, SEEK_SET);
        if (isElf64) {
            for (uint32_t i = 0; i < sectionEntryCount; i++) {
                Elf64SectionEntry section;
                section.section_name = read_u32();
                section.section_type = read_u32();
                section.section_flags = read_u64();
                section.section_addr = read_u64();
                section.section_offset = read_u64();
                section.section_size = read_u64();
                section.section_link = read_u32();
                section.section_info = read_u32();
                section.section_addralign = read_u64();
                section.section_entsize = read_u64();
                sectionTable64.push_back(section);
            }
            // 读取字符串表
            if (stringTableSectionIndex != 0 && stringTableSectionIndex < sectionEntryCount) {
                Elf64SectionEntry & section = sectionTable64[stringTableSectionIndex];
                shStrTabSize = section.section_size;
                shstrtab = new char[shStrTabSize + 4];
                elfSeek((int64_t)section.section_offset, SEEK_SET);
                fread(shstrtab, 1, (size_t)shStrTabSize);
            }
            // 读取所有section名
            sectionNameList.clear();
            for (uint32_t i = 0; i < sectionTable64.size(); i++) {
                const char * str = getSectionString(sectionTable64[i].section_name);
                sectionNameList.push_back(str);
            }
            //
            int maxSectionIdDigits = 0;         // section 序号 最大占多少位10进制数字
            size_t maxNameLength = 0;           // section名最大长度
            size_t maxTypeLength = 0;           // section type 最大长度
            bool secAddrUse64 = false;          // section addr 是否用64位
            bool secOffsetUse64 = false;        // section offset 是否用64位
            int maxSizeDigits = 0;              // section size 最大占多少位10进制数字
            int maxLinkDigits = 0;              // section link 最大占多少位10进制数字
            int maxInfoDigits = 0;              // section info 最大占多少位10进制数字
            bool secAddrAlignUse64 = false;     // section addralign 是否用64位
            int maxEntSizeDigits = 0;           // section entsize 最大占多少位10进制数字
            
            for (uint32_t i = 1; i < sectionTable64.size(); i++) {
                Elf64SectionEntry & section = sectionTable64[i];
                //
                if (get_number_digits((int64_t) i) > maxSectionIdDigits) {
                    maxSectionIdDigits = get_number_digits((int64_t) i);
                }
                if (strlen(sectionNameList[i]) > maxNameLength) {
                    maxNameLength = strlen(sectionNameList[i]);
                }
                const char * typeSesc = getSectionTypeDesc(section.section_type);
                if (strlen(typeSesc) > maxTypeLength) {
                    maxTypeLength = strlen(typeSesc);
                }
                if (section.section_addr > 0xffffffff) {
                    secAddrUse64 = true;
                }
                if (section.section_offset > 0xffffffff) {
                    secOffsetUse64 = true;
                }
                if (get_number_digits(section.section_size) > maxSizeDigits) {
                    maxSizeDigits = get_number_digits(section.section_size);
                }
                if (get_number_digits((uint64_t)section.section_link) > maxLinkDigits) {
                    maxLinkDigits = get_number_digits((uint64_t)section.section_link);
                }
                if (get_number_digits((uint64_t)section.section_info) > maxInfoDigits) {
                    maxInfoDigits = get_number_digits((uint64_t)section.section_info);
                }
                if (section.section_addralign > 0xffffffff) {
                    secAddrAlignUse64 = true;
                }
                if (get_number_digits(section.section_entsize) > maxEntSizeDigits) {
                    maxEntSizeDigits = get_number_digits(section.section_entsize);
                }
            }
            //
            if (maxSizeDigits < 4)  maxSizeDigits = 4;
            if (maxLinkDigits < 4)  maxLinkDigits = 4;
            if (maxInfoDigits < 4)  maxInfoDigits = 4;
            if (maxEntSizeDigits < 7) maxEntSizeDigits = 7;
            //
            char format[128];
            putchar('\n');
            printf("节表:\n");
            for (int i = 0; i < maxSectionIdDigits; i++)
                putchar(' ');
            putchar(' ');
            // section name
            snprintf(format, sizeof(format), "%%-%us ", (uint32_t)maxNameLength);
            printf(format, "name");
            // section type
            snprintf(format, sizeof(format), "%%-%us ", (uint32_t)maxTypeLength);
            printf(format, "type");
            // section addr
            if (secAddrUse64) {
                printf("%-18s ", "addr");
            }
            else {
                printf("%-10s ", "addr");
            }
            // section offset
            if (secOffsetUse64) {
                printf("%-18s ", "offset");
            }
            else {
                printf("%-10s ", "offset");
            }
            // section size
            snprintf(format, sizeof(format), "%%%ds ", maxSizeDigits);
            printf(format, "size");
            // section link
            snprintf(format, sizeof(format), "%%%ds ", maxLinkDigits);
            printf(format, "link");
            // section info
            snprintf(format, sizeof(format), "%%%ds ", maxInfoDigits);
            printf(format, "info");
            // section addralign
            if (secAddrAlignUse64) {
                printf("%-18s ", "addralign");
            }
            else {
                printf("%-10s ", "addralign");
            }
            // section entsize
            snprintf(format, sizeof(format), "%%%ds ", maxEntSizeDigits);
            printf(format, "entsize");
            printf("flag");
            putchar('\n');
            //
            for (uint32_t i = 1; i < sectionTable64.size(); i++) {
                Elf64SectionEntry & section = sectionTable64[i];
                // id
                snprintf(format, sizeof(format), "%%%dd ", maxSectionIdDigits);
                printf(format, (int) i);
                // section name
                snprintf(format, sizeof(format), "%%-%us ", (uint32_t)maxNameLength);
                printf(format, sectionNameList[i]);
                // section type
                const char * typeSesc = getSectionTypeDesc(section.section_type);
                snprintf(format, sizeof(format), "%%-%us ", (uint32_t)maxTypeLength);
                printf(format, typeSesc);
                // section addr
                if (secAddrUse64) {
                    printf("0x%016X ", section.section_addr);
                } else {
                    printf("0x%08X ", (uint32_t) section.section_addr);
                }
                // section offset
                if (secOffsetUse64) {
                    printf("0x%016X ", section.section_offset);
                }
                else {
                    printf("0x%08X ", (uint32_t)section.section_offset);
                }
                // section size
                snprintf(format, sizeof(format), "%%%dd ", maxSizeDigits);
                printf(format, section.section_size);
                // section link
                snprintf(format, sizeof(format), "%%%dd ", maxLinkDigits);
                printf(format, section.section_link);
                // section info
                snprintf(format, sizeof(format), "%%%dd ", maxInfoDigits);
                printf(format, section.section_info);
                // section addralign
                if (secAddrAlignUse64) {
                    printf("0x%016X ", section.section_addralign);
                }
                else {
                    printf("0x%08X ", (uint32_t)section.section_addralign);
                }
                // section entsize
                snprintf(format, sizeof(format), "%%%dd ", maxEntSizeDigits);
                printf(format, section.section_entsize);
                // flag
                char flagStr[64] = {0};
                int flagStrCnt = 0;
                if (section.section_flags & ELF_SECTION_FLAG_ALLOC) flagStr[flagStrCnt++] = 'A';
                if (section.section_flags & ELF_SECTION_FLAG_WRITE) flagStr[flagStrCnt++] = 'W';
                if (section.section_flags & ELF_SECTION_FLAG_EXEC) flagStr[flagStrCnt++] = 'X';
                if (section.section_flags & ELF_SECTION_FLAG_MERGE) flagStr[flagStrCnt++] = 'M';
                if (section.section_flags & ELF_SECTION_FLAG_STRING) flagStr[flagStrCnt++] = 'S';
                if (section.section_flags & ELF_SECTION_GROUP) flagStr[flagStrCnt++] = 'G';
                if (section.section_flags & ELF_SECTION_TLS) flagStr[flagStrCnt++] = 'T';

                if (section.section_flags & ELF_SECTION_FLAG_INFO_LINK) flagStr[flagStrCnt++] = 'I';
                if (section.section_flags & ELF_SECTION_FLAG_LINK_ORDER) flagStr[flagStrCnt++] = 'L';
                if (section.section_flags & ELF_SECTION_COMPRESSED) flagStr[flagStrCnt++] = 'C';
                if (section.section_flags & ELF_SECTION_GNU_MBIND) flagStr[flagStrCnt++] = 'D';
                //if (section.section_flags & ELF_SECTION_EXCLUDE) flagStr[flagStrCnt++] = 'E';
                /*
                    Key to Flags:
                      O (extra OS processing required),
                      C (compressed), x (unknown), o (OS specific),
                      l (large), p (processor specific)
                    */

                if (flagStr[0] != 0)
                    printf("[%s]", flagStr);
                printf("\n");
            }
            
        }
        //
        


    }

    void dumpProgramHeader() {
        if (programEntryCount == 0 || programTableOffset == 0) return;
        elfSeek((int64_t)programTableOffset, SEEK_SET);
        if (isElf64) {
            for (uint32_t i = 0; i < programEntryCount; i++) {
                Elf64ProgramEntry entry;
                entry.pro_type = read_u32();
                entry.pro_flags = read_u32();
                entry.pro_offset = read_u64();
                entry.pro_vaddr = read_u64();
                entry.pro_paddr = read_u64();
                entry.pro_filesz = read_u64();
                entry.pro_memsz = read_u64();
                entry.pro_align = read_u64();
                programTable64.push_back(entry);
            }
            int maxProgramIdDigits = 0;         // program header 序号 最大占多少位10进制数字
            size_t maxTypeLength = 0;           // program type 最大长度
            bool vAddrUse64 = false;            // vaddr 是否用64位
            bool offsetUse64 = false;           // offset 是否用64位
            int maxMemSizeDigits = 0;           // memsz
            int maxFileSizeDigits = 0;          // filesz
            int maxAlignDigits = 0;             // align
            for (uint32_t i = 0; i < programTable64.size(); i++) {                      // program header 从0条目开始都是实质内容，不留空
                Elf64ProgramEntry & programEntry = programTable64[i];
                //
                if (get_number_digits((int64_t)i) > maxProgramIdDigits) {
                    maxProgramIdDigits = get_number_digits((int64_t)i);
                }
                const char * typeSesc = getProgramTypeDesc(programEntry.pro_type);
                if (strlen(typeSesc) > maxTypeLength) {
                    maxTypeLength = strlen(typeSesc);
                }
                if (programEntry.pro_vaddr > 0xffffffff) {
                    vAddrUse64 = true;
                }
                if (programEntry.pro_offset > 0xffffffff) {
                    vAddrUse64 = true;
                }
                if (get_number_digits(programEntry.pro_memsz) > maxMemSizeDigits) {
                    maxMemSizeDigits = get_number_digits(programEntry.pro_memsz);
                }
                if (get_number_digits(programEntry.pro_filesz) > maxFileSizeDigits) {
                    maxFileSizeDigits = get_number_digits(programEntry.pro_filesz);
                }
                if (get_number_digits(programEntry.pro_align) > maxAlignDigits) {
                    maxAlignDigits = get_number_digits(programEntry.pro_align);
                }
            }
            char format[128];
            putchar('\n');
            printf("程序头表:\n");
            for (int i = 0; i < maxProgramIdDigits; i++)
                putchar(' ');
            putchar(' ');
            // type
            snprintf(format, sizeof(format), "%%-%us ", (uint32_t)maxTypeLength);
            printf(format, "type");
            putchar('\n');
            //
            for (uint32_t i = 0; i < programTable64.size(); i++) {
                Elf64ProgramEntry & programEntry = programTable64[i];
                // id
                snprintf(format, sizeof(format), "%%%dd ", maxProgramIdDigits);
                printf(format, (int)i);
                // type
                const char * typeSesc = getProgramTypeDesc(programEntry.pro_type);
                snprintf(format, sizeof(format), "%%-%us ", (uint32_t)maxTypeLength);
                printf(format, typeSesc);
                // vaddr
                if (vAddrUse64) {
                    printf("0x%016X ", programEntry.pro_vaddr);
                }
                else {
                    printf("0x%08X ", (uint32_t)programEntry.pro_vaddr);
                }
                // vaddr
                if (offsetUse64) {
                    printf("0x%016X ", programEntry.pro_offset);
                }
                else {
                    printf("0x%08X ", (uint32_t)programEntry.pro_offset);
                }
                // mem size
                snprintf(format, sizeof(format), "%%%dlld ", maxMemSizeDigits);
                printf(format, programEntry.pro_memsz);
                // file size
                snprintf(format, sizeof(format), "%%%dlld ", maxFileSizeDigits);
                printf(format, programEntry.pro_filesz);
                // align
                snprintf(format, sizeof(format), "%%%dlld ", maxAlignDigits);
                printf(format, programEntry.pro_align);
                // flag
                char flagStr[64] = { 0 };
                int flagStrCnt = 0;
                if (ELF_SEGMENT_FLAG_READABLE & programEntry.pro_flags) {
                    flagStr[flagStrCnt++] = 'R';
                }
                if (ELF_SEGMENT_FLAG_WRITABLE & programEntry.pro_flags) {
                    flagStr[flagStrCnt++] = 'W';
                }
                if (ELF_SEGMENT_FLAG_EXECUTABLE & programEntry.pro_flags) {
                    flagStr[flagStrCnt++] = 'X';
                }
                if (flagStr[0] != 0)
                    printf("[%s]", flagStr);
                // 显示动态连接器
                if (ELF_SEGMENT_TYPE_INTERP == programEntry.pro_type) {
                    char loaderName[256];
                    elfSeek(programEntry.pro_offset, SEEK_SET);
                    memset(loaderName, 0, sizeof(loaderName));
                    int readSize = programEntry.pro_filesz;
                    if (readSize > sizeof(loaderName)) {
                        readSize = sizeof(loaderName);
                    }
                    fread(loaderName, 1, readSize);
                    printf("  加载器: %s", loaderName);
                }
                //
                putchar('\n');
            }
        }
    }

    DynEntryShowInfo getDynTypeDesc(uint64_t type) {
        DynEntryShowInfo info;
        switch (type) {
        case DYN_TYPE_NULL:
            info.showText = "NULL";
            info.showType = info.SHOW_INT;
            break;
        case DYN_TYPE_NEEDED:
            info.showText = "NEEDED";
            info.showType = info.SHOW_CUSTOM;
            break;
        case DYN_TYPE_PLTRELSZ:
            info.showText = "PLTRELSZ";
            info.showType = info.SHOW_BYTE_SIZE;
            break;
        case DYN_TYPE_PLTGOT:
            info.showText = "PLTGOT";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_HASH:
            info.showText = "HASH";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_STRTAB:
            info.showText = "STRTAB";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_SYMTAB:
            info.showText = "SYMTAB";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_RELA:
            info.showText = "RELA";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_RELASZ:
            info.showText = "RELASZ";
            info.showType = info.SHOW_BYTE_SIZE;
            break;
        case DYN_TYPE_RELAENT:
            info.showText = "RELAENT";
            info.showType = info.SHOW_BYTE_SIZE;
            break;
        case DYN_TYPE_STRSZ:
            info.showText = "STRSZ";
            info.showType = info.SHOW_BYTE_SIZE;
            break;
        case DYN_TYPE_SYMENT:
            info.showText = "SYMENT";
            info.showType = info.SHOW_BYTE_SIZE;
            break;
        case DYN_TYPE_INIT:
            info.showText = "INIT";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_FINI:
            info.showText = "FINI";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_SONAME:
            info.showText = "SONAME";
            info.showType = info.SHOW_CUSTOM;
            break;
        case DYN_TYPE_RPATH:
            info.showText = "RPATH";
            info.showType = info.SHOW_CUSTOM;
            break;
        case DYN_TYPE_SYMBOLIC:
            info.showText = "SYMBOLIC";
            info.showType = info.SHOW_INT;
            break;
        case DYN_TYPE_REL:
            info.showText = "REL";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_RELSZ:
            info.showText = "RELSZ";
            info.showType = info.SHOW_BYTE_SIZE;
            break;
        case DYN_TYPE_RELENT:
            info.showText = "RELENT";
            info.showType = info.SHOW_BYTE_SIZE;
            break;
        case DYN_TYPE_PLTREL:
            info.showText = "PLTREL";
            info.showType = info.SHOW_CUSTOM;
            break;
        case DYN_TYPE_DEBUG:
            info.showText = "DEBUG";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_TEXTREL:
            info.showText = "TEXTREL";
            info.showType = info.SHOW_INT;
            break;
        case DYN_TYPE_JMPREL:
            info.showText = "JMPREL";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_BIND_NOW:
            info.showText = "BIND_NOW";
            info.showType = info.SHOW_INT;
            break;
        case DYN_TYPE_INIT_ARRAY:
            info.showText = "INIT_ARRAY";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_FINI_ARRAY:
            info.showText = "FINI_ARRAY";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_INIT_ARRAYSZ:
            info.showText = "INIT_ARRAYSZ";
            info.showType = info.SHOW_BYTE_SIZE;
            break;
        case DYN_TYPE_FINI_ARRAYSZ:
            info.showText = "FINI_ARRAYSZ";
            info.showType = info.SHOW_BYTE_SIZE;
            break;
        case DYN_TYPE_RUNPATH:
            info.showText = "RUNPATH";
            info.showType = info.SHOW_CUSTOM;
            break;
        case DYN_TYPE_FLAGS:
            info.showText = "FLAGS";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_PREINIT_ARRAY:
            info.showText = "PREINIT_ARRAY";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_PREINIT_ARRAYSZ:
            info.showText = "PREINIT_ARRAYSZ";
            info.showType = info.SHOW_BYTE_SIZE;
            break;
        case DYN_TYPE_SYMTAB_SHNDX:
            info.showText = "SYMTAB_SHNDX";
            info.showType = info.SHOW_INT;
            break;
        case DYN_TYPE_RELRSZ:
            info.showText = "RELRSZ";
            info.showType = info.SHOW_BYTE_SIZE;
            break;
        case DYN_TYPE_RELR:
            info.showText = "RELR";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_RELRENT:
            info.showText = "RELRENT";
            info.showType = info.SHOW_BYTE_SIZE;
            break;
        case DYN_TYPE_GNU_HASH:
            info.showText = "GNU_HASH";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_FLAGS_1:
            info.showText = "FLAGS_1";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_VERNEED:
            info.showText = "VERNEED";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_VERNEEDNUM:
            info.showText = "VERNEEDNUM";
            info.showType = info.SHOW_INT;
            break;
        case DYN_TYPE_VERSYM:
            info.showText = "VERSYM";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_RELACOUNT:
            info.showText = "RELACOUNT";
            info.showType = info.SHOW_INT;
            break;
        case DYN_TYPE_VERDEF:
            info.showText = "VERDEF";
            info.showType = info.SHOW_HEX;
            break;
        case DYN_TYPE_VERDEFNUM:
            info.showText = "VERDEFNUM";
            info.showType = info.SHOW_INT;
            break;
        default: {
            char tmpstr[256];
            snprintf(tmpstr, sizeof(tmpstr), "unknown-0x%llx", type);
            info.showText = tmpstr;
            info.showType = info.SHOW_HEX;
        }
            break;
        }
        return info;
    }

    void dumpDynamicSegment() {
        bool found = false;
        uint64_t size = 0;
        uint64_t offset = 0;
        for (size_t i = 0; i < programTable64.size(); i++) {
            if (programTable64[i].pro_type == ELF_SEGMENT_TYPE_DYNAMIC) {
                found = true;
                offset = programTable64[i].pro_offset;
                size = programTable64[i].pro_filesz;
                break;
            }
        }
        if (!found)   return;
        printf("\n");
        
        if (offset > 0xffffffff)
            printf("\n动态链接信息, 文件偏移: 0x%016llx, 数据大小: %lld\n", offset, size);
        else 
            printf("\n动态链接信息, 文件偏移: 0x%08x, 数据大小: %lld\n", (uint32_t)offset, size);
        printf("-----------------\n");
        //
        elfSeek(offset, SEEK_SET);
        uint64_t value1, value2;
        while (size > 0) {
            value1 = read_u64();
            value2 = read_u64();
            size -= 16;
            Elf64DynEntry entry;
            entry.d_tag = (int64_t) value1;
            entry.d_un.d_val = value2;
            dynEntries64.push_back(entry);
            if (value1 == 0 && value2 == 0)
                break;
        }
        // 读取动态链接所需要的字符串表
        uint64_t strTabAddr = 0;
        uint64_t strTabSize = 0;
        for (size_t i = 0; i < dynEntries64.size(); i++) {
            const Elf64DynEntry & entry = dynEntries64[i]; 
            if (entry.d_tag == DYN_TYPE_STRTAB) {
                strTabAddr = entry.d_un.d_ptr;
            }
            if (entry.d_tag == DYN_TYPE_STRSZ) {
                strTabSize = entry.d_un.d_val;
            }
        }
        uint64_t strTabOffset = addrToFileOffset(strTabAddr);
        char * mem = new char[strTabSize + 4];
        elfSeek(strTabOffset, SEEK_SET);
        fread(mem, 1, strTabSize);
        dynstr = mem;
        dynStrSize = strTabSize;
        //
        size_t maxTypeLength = 0;
        for (size_t i = 0; i < dynEntries64.size(); i++) {
            DynEntryShowInfo info = getDynTypeDesc(dynEntries64[i].d_tag);
            if (info.showText.length() > maxTypeLength) {
                maxTypeLength = info.showText.length();
            }
        }
        //
        char format[128];
        for (size_t i = 0; i < dynEntries64.size(); i++) {
            const Elf64DynEntry & entry = dynEntries64[i];
            DynEntryShowInfo info = getDynTypeDesc(entry.d_tag);
            // type
            snprintf(format, sizeof(format), "%%-%us ", (uint32_t)maxTypeLength);
            printf(format, info.showText.c_str());
            // value
            if (info.showType == info.SHOW_INT) {
                printf("%lld", (uint64_t)entry.d_un.d_val);
            }
            else if (info.showType == info.SHOW_HEX) {
                if (entry.d_un.d_val > 0xffffffff) {
                    printf("0x%016llx", (uint64_t)entry.d_un.d_val);
                }
                else {
                    printf("0x%08x", (uint32_t)entry.d_un.d_val);
                }
            }
            else if (info.showType == info.SHOW_BYTE_SIZE) {
                printf("%lld 字节", (uint64_t)entry.d_un.d_val);
            }
            else if (info.showType == info.SHOW_CUSTOM) {
                if (DYN_TYPE_NEEDED == entry.d_tag) {
                    const char * libName;
                    if (entry.d_un.d_val < strTabSize) {
                        libName = & dynstr[entry.d_un.d_val];
                    }
                    else {
                        libName = "";
                    }
                    printf("依赖:  %s", libName);
                }
                else if (DYN_TYPE_SONAME == entry.d_tag) {
                    const char * soName;
                    if (entry.d_un.d_val < strTabSize) {
                        soName = &dynstr[entry.d_un.d_val];
                    }
                    else {
                        soName = "";
                    }
                    printf("本库名:  %s", soName);
                }
                else if (DYN_TYPE_PLTREL == entry.d_tag) {
                    printf("%lld", entry.d_un.d_val);
                    if (entry.d_un.d_val == DYN_TYPE_RELA) {
                        printf(" => RELA");
                    }
                    else if (entry.d_un.d_val == DYN_TYPE_REL) {
                        printf(" => REL");
                    }
                    else if (entry.d_un.d_val == DYN_TYPE_RELR) {
                        printf(" => RELR");
                    }
                }
            }
            putchar('\n');
        }
    }

    const char * relTypeDesc(uint32_t relType) {
        return "R_X86_64_RELATIVE";
    }

    void showRelData(const std::vector<Elf64RelAEntry> & relList, bool isRelA, uint64_t symbolTabOffset, uint64_t symbolEntrySize, const char * strtab) {
        bool offsetUse64 = false;
        int maxIdDigits = 0;                      // id 最大位数
        int maxSymIndexDigits = 0;                // 符号索引的最大位数
        int maxAddendDigits = 0;                  // Addend的最大位数
        size_t maxLengthTypeStr = 0;              // 重定位类型的字符串最大长度
        //
        maxIdDigits = get_number_digits((uint64_t) relList.size());
        for (size_t i = 0; i < relList.size(); i++) {
            uint32_t symIndex;
            uint32_t relType;
            if (relList[i].offset > 0xffffffff) {
                offsetUse64 = true;
            }
            if (isElf64) {
                symIndex = (uint32_t)(relList[i].info >> 32);
                relType = (uint32_t)(relList[i].info & 0xffffffff);
            }
            else {
                symIndex = ((uint32_t)relList[i].info) >> 8;
                relType = ((uint32_t)relList[i].info) & 0xff;
            }
            int curSymIndexDigits = get_number_digits((uint64_t)symIndex);
            if (curSymIndexDigits > maxSymIndexDigits) {
                maxSymIndexDigits = curSymIndexDigits;
            }
            int curAddendDigits = get_number_digits((int64_t)relList[i].addend);
            if (curAddendDigits > maxAddendDigits) {
                maxAddendDigits = curAddendDigits;
            }
        }
        //
        char format[128];
        for (size_t i = 0; i < relList.size(); i++) {
            const Elf64RelAEntry & rel = relList[i];
            uint32_t symIndex;
            uint32_t relType;
            if (isElf64) {
                symIndex = (uint32_t)(relList[i].info >> 32);
                relType = (uint32_t)(relList[i].info & 0xffffffff);
            }
            else {
                symIndex = ((uint32_t)relList[i].info) >> 8;
                relType = ((uint32_t)relList[i].info) & 0xff;
            }
            // id
            snprintf(format, sizeof(format), "%%%du: ", maxIdDigits);
            printf(format, (uint64_t)i);
            // offset
            if (offsetUse64) {
                printf("0x%016llx ", rel.offset);
            }
            else {
                printf("0x%08x ", (uint32_t) rel.offset);
            }
            // type name
            printf(" %s ", relTypeDesc(relType));
            // addend
            if (isRelA) {
                snprintf(format, sizeof(format), " %%%dlld ", maxAddendDigits);
                printf(format, rel.addend);
            }
            // symbol index
            snprintf(format, sizeof(format), " %%%du ", maxSymIndexDigits);
            printf(format, symIndex);
            // symbol name
            if (symbolTabOffset != 0 && symIndex != 0) {
                elfSeek(symbolTabOffset + symbolEntrySize * symIndex, SEEK_SET);
                uint32_t nameOffset = read_u32();
                if (nameOffset != 0 && strtab != nullptr) {
                    const char * name = & strtab[nameOffset];
                    printf(" %s", name);
                }
            }

            putchar('\n');
        }
    }

    void dumpDynamicReloc() {
        // 符号表信息
        uint64_t dynSymAddr = 0;
        uint64_t dynSymEnt = 0;
        // RELA
        uint64_t relAAddr = 0;
        uint64_t relASize = 0;
        uint64_t relAEnt = 0;
        // REL
        uint64_t relAddr = 0;
        uint64_t relSize = 0;
        uint64_t relEnt = 0;
        // 
        for (size_t i = 0; i < dynEntries64.size(); i++) {
            const Elf64DynEntry & dyn = dynEntries64[i];
            if (dyn.d_tag == DYN_TYPE_RELA) {                   // RELA 地址
                relAAddr = dyn.d_un.d_ptr;
            }
            else if (dyn.d_tag == DYN_TYPE_RELASZ) {            // RELA 大小
                relASize = dyn.d_un.d_val;
            }
            else if (dyn.d_tag == DYN_TYPE_RELAENT) {           // RELA 条目
                relAEnt = dyn.d_un.d_val;
            }
            else if (dyn.d_tag == DYN_TYPE_SYMTAB) {            // 符号表地址
                dynSymAddr = dyn.d_un.d_ptr;
            }
            else if (dyn.d_tag == DYN_TYPE_SYMENT) {            // 符号表项目
                dynSymEnt = dyn.d_un.d_val;
            }
            else if (dyn.d_tag == DYN_TYPE_REL) {
                relAddr = dyn.d_un.d_ptr;
            }
            else if (dyn.d_tag == DYN_TYPE_RELSZ) {
                relSize = dyn.d_un.d_val;
            }
            else if (dyn.d_tag == DYN_TYPE_RELENT) {
                relEnt = dyn.d_un.d_val;
            }
        }
        if (relAEnt == 0) {
            relAEnt = isElf64 ? 24 : 12;
        }
        if (relEnt == 0) {
            relEnt = isElf64 ? 16 : 8;
        }
        // 如果存在 RELA 则展示
        if (relAAddr != 0 && relASize != 0) {
            uint64_t relAOffset = addrToFileOffset(relAAddr);
            uint64_t dynSymTabOffset = addrToFileOffset(dynSymAddr);
            if (relAOffset != 0) {
                uint64_t relAEntryCount = relASize / relAEnt;
                printf("\n动态链接重定位表, RELA 格式, 文件偏移: ");
                if (relAOffset > 0xffffffff)
                    printf("0x%016llx", relAOffset);
                else 
                    printf("0x%08x", (uint32_t)relAOffset);
                printf(", 内存地址: ");
                if (relAAddr > 0xffffffff)
                    printf("0x%016llx", relAAddr);
                else
                    printf("0x%08x", (uint32_t)relAAddr);
                printf(", 重定位条目数: %llu", relAEntryCount);
                putchar('\n');
                //
                std::vector<Elf64RelAEntry> relaData;
                elfSeek(relAOffset, SEEK_SET);
                for (uint64_t i = 0; i < relAEntryCount; i++) {
                    Elf64RelAEntry relaUnit;
                    relaUnit.offset = read_u64();
                    relaUnit.info = read_u64();
                    relaUnit.addend = (int64_t) read_u64();
                    relaData.push_back(relaUnit);
                }
                //
                showRelData(relaData, true, dynSymTabOffset, dynSymEnt, dynstr);
            }
        }
        // 如果存在 REL 则展示
        if (relAddr != 0 && relSize != 0) {
            uint64_t relOffset = addrToFileOffset(relAddr);
            uint64_t dynSymTabOffset = addrToFileOffset(dynSymAddr);
            if (relOffset != 0) {
                uint64_t relEntryCount = relSize / relEnt;
                printf("\n动态链接重定位表, REL 格式, 文件偏移: ");
                if (relOffset > 0xffffffff)
                    printf("0x%016llx", relOffset);
                else
                    printf("0x%08x", (uint32_t)relOffset);
                printf(", 内存地址: ");
                if (relAddr > 0xffffffff)
                    printf("0x%016llx", relAddr);
                else
                    printf("0x%08x", (uint32_t)relAddr);
                printf(", 重定位条目数: %llu", relEntryCount);
                putchar('\n');
                //
                std::vector<Elf64RelAEntry> relaData;
                elfSeek(relOffset, SEEK_SET);
                for (uint64_t i = 0; i < relEntryCount; i++) {
                    Elf64RelAEntry relaUnit;
                    relaUnit.offset = read_u64();
                    relaUnit.info = read_u64();
                    relaUnit.addend = 0;
                    relaData.push_back(relaUnit);
                }
                //
                showRelData(relaData, false, dynSymTabOffset, dynSymEnt, dynstr);
            }
        }
        // JMP Rel
        uint64_t jmpRelTab = 0;
        uint64_t pltRelSize = 0;
        uint64_t pltRelType = 0;
        uint64_t pltRelEnt = 0;
        bool pltRelUseA = false;
        for (size_t i = 0; i < dynEntries64.size(); i++) {
            const Elf64DynEntry & dyn = dynEntries64[i];
            if (dyn.d_tag == DYN_TYPE_JMPREL) {                 // PLT REL 表
                jmpRelTab = dyn.d_un.d_ptr;
            }
            else if (dyn.d_tag == DYN_TYPE_PLTRELSZ) {          // PLT REL 表大小
                pltRelSize = dyn.d_un.d_val;
            }
            else if (dyn.d_tag == DYN_TYPE_PLTREL) {            // PLT REL 的类型
                pltRelType = dyn.d_un.d_val;
                if (pltRelType == DYN_TYPE_RELA) {
                    pltRelEnt = isElf64 ? 24 : 12;
                    pltRelUseA = true;
                }
                else if (pltRelType == DYN_TYPE_REL) {
                    pltRelEnt = isElf64 ? 12 : 8;
                }
            }
        }
        if (jmpRelTab != 0 && pltRelSize != 0 && pltRelEnt != 0) {
            uint64_t pltRelOffset = addrToFileOffset(jmpRelTab);
            uint64_t dynSymTabOffset = addrToFileOffset(dynSymAddr);
            if (pltRelOffset != 0) {
                uint64_t pltRelEntryCount = pltRelSize / pltRelEnt;
                printf("\n动态链接 PLT 重定位表, %s 格式, 文件偏移: ", pltRelUseA ? "RELA" : "REL");
                if (pltRelOffset > 0xffffffff)
                    printf("0x%016llx", pltRelOffset);
                else
                    printf("0x%08x", (uint32_t)pltRelOffset);
                printf(", 内存地址: ");
                if (jmpRelTab > 0xffffffff)
                    printf("0x%016llx", jmpRelTab);
                else
                    printf("0x%08x", (uint32_t)jmpRelTab);
                printf(", 重定位条目数: %llu", pltRelEntryCount);
                putchar('\n');
                //
                std::vector<Elf64RelAEntry> relaData;
                elfSeek(pltRelOffset, SEEK_SET);
                for (uint64_t i = 0; i < pltRelEntryCount; i++) {
                    Elf64RelAEntry relaUnit;
                    relaUnit.offset = read_u64();
                    relaUnit.info = read_u64();
                    if (pltRelUseA)
                        relaUnit.addend = read_u64();
                    else
                        relaUnit.addend = 0;
                    relaData.push_back(relaUnit);
                }
                //
                showRelData(relaData, pltRelUseA, dynSymTabOffset, dynSymEnt, dynstr);
            }
        }
    }

    const char * symbolBindingsToStr(uint32_t binding) {
        switch (binding)
        {
        case SYMBOL_BINDINGS_LOCAL:
            return "LOCAL";
        case SYMBOL_BINDINGS_GLOBAL:
            return "GLOBAL";
        case SYMBOL_BINDINGS_WEAK:
            return "WEAK";
        case SYMBOL_BINDINGS_GNU_UNIQUE:
            return "GNU_UNIQUE";
        default:
            break;
        }
        return "";
    }

    const char * symbolTypeToStr(uint32_t type) {
        switch (type)
        {
        case SYMBOL_TYPE_NOTYPE:
            return "NOTYPE";
        case SYMBOL_TYPE_OBJECT:
            return "OBJECT";
        case SYMBOL_TYPE_FUNC:
            return "FUNC";
        case SYMBOL_TYPE_SECTION:
            return "SECTION";
        case SYMBOL_TYPE_FILE:
            return "FILE";
        case SYMBOL_TYPE_COMMON:
            return "COMMON";
        case SYMBOL_TYPE_TLS:
            return "TLS";
        case SYMBOL_TYPE_RELC:
            return "RELC";
        case SYMBOL_TYPE_SRELC:
            return "SRELC";
        case SYMBOL_TYPE_GNU_IFUNC:
            return "GNU_IFUNC";
        default:
            break;
        }
        return "";
    }

    const char * symbolVisibilityToStr(uint32_t visibility) {
        switch (visibility)
        {
        case SYMBOL_VISIBILITY_DEFAULT:
            return "DEFAULT";
        case SYMBOL_VISIBILITY_INTERNAL:
            return "INTERNAL";
        case SYMBOL_VISIBILITY_HIDDEN:
            return "HIDDEN";
        case SYMBOL_VISIBILITY_PROTECTED:
            return "PROTECTED";
        default:
            break;
        }
        return "";
    }

    void showSymbolData(const std::vector<Elf64Symbol> & symbols, const char * strtab) {
        //
        int maxIdDigits = 0;
        bool valueUse64 = false;
        int maxSecIndexDigits = 0;
        int maxSizeDigits = 0;
        size_t maxBindingLength = 0;
        size_t maxTypeLength = 0;
        size_t maxVisibilityLength = 0;
        maxIdDigits = get_number_digits(symbols.size());
        for (size_t i = 0; i < symbols.size(); i++) {
            const Elf64Symbol & symbol = symbols[i];
            if (symbol.value > 0xffffffff)
                valueUse64 = true;
            int curSecIndexDigits = get_number_digits((uint64_t)symbol.shndx);
            if (curSecIndexDigits > maxSecIndexDigits)
                maxSecIndexDigits = curSecIndexDigits;
            int curSizeDigits = get_number_digits((uint64_t)symbol.size);
            if (curSizeDigits > maxSizeDigits)
                maxSizeDigits = curSizeDigits;
            uint8_t binding = symbol.info >> 4;
            uint8_t type = symbol.info & 0x0f;
            uint8_t visibility = symbol.other & 0x0f;
            if (strlen(symbolBindingsToStr(binding)) > maxBindingLength) {
                maxBindingLength = strlen(symbolBindingsToStr(binding));
            }
            if (strlen(symbolTypeToStr(type)) > maxTypeLength) {
                maxTypeLength = strlen(symbolTypeToStr(type));
            }
            if (strlen(symbolVisibilityToStr(visibility)) > maxVisibilityLength) {
                maxVisibilityLength = strlen(symbolVisibilityToStr(visibility));
            }
        }

        char format[128];
        for (size_t i = 0; i < symbols.size(); i++) {
            const Elf64Symbol & symbol = symbols[i];
            // id
            snprintf(format, sizeof(format), "%%%dllu: ", maxIdDigits);
            printf(format, (uint64_t)i);
            // section index
            snprintf(format, sizeof(format), "%%%du ", maxSecIndexDigits);
            printf(format, (uint32_t)symbol.shndx);
            // size
            snprintf(format, sizeof(format), "%%%dllu ", maxSizeDigits);
            printf(format, (uint64_t)symbol.size);
            // value
            if (valueUse64) {
                printf("0x%016llx ", symbol.value);
            }
            else {
                printf("0x%08x ", (uint32_t)symbol.value);
            }
            //
            uint8_t binding = symbol.info >> 4;
            snprintf(format, sizeof(format), "%%-%us ", (uint32_t)maxBindingLength);
            printf(format, symbolBindingsToStr(binding));
            //
            uint8_t type = symbol.info & 0x0f;
            snprintf(format, sizeof(format), "%%-%us ", (uint32_t)maxTypeLength);
            printf(format, symbolTypeToStr(type));
            //
            uint8_t visibility = symbol.other & 0x0f;
            snprintf(format, sizeof(format), "%%-%us ", (uint32_t)maxVisibilityLength);
            printf(format, symbolVisibilityToStr(visibility));
            // name 
            if (symbol.name != 0) {
                printf(" %s", &strtab[symbol.name]);
            }
            putchar('\n');
        }
    }

    void dumpDynamicSymbol() {
        uint64_t dynSymAddr = 0;
        uint64_t dynSymSize = 0;        // 好像没有这项, 需要从 sction 表中找 
        uint64_t dynSymEnt = 0;

        for (size_t i = 0; i < dynEntries64.size(); i++) {
            const Elf64DynEntry & dyn = dynEntries64[i];
            if (dyn.d_tag == DYN_TYPE_SYMTAB) {
                dynSymAddr = dyn.d_un.d_ptr;
            }
            //else if (dyn.d_tag == DYN_TYPE_RELASZ) {
            //    relASize = dyn.d_un.d_val;
            //}
            else if (dyn.d_tag == DYN_TYPE_SYMENT) {
                dynSymEnt = dyn.d_un.d_val;
            }
        }
        if (dynSymEnt == 0) {
            dynSymEnt = isElf64 ? 24 : 16;
        }
        if (dynSymAddr == 0 || dynSymEnt == 0) {
            return;
        }
        //
        bool foundSection = false;
        for (size_t i = 0; i < sectionTable64.size(); i++) {
            const Elf64SectionEntry & section = sectionTable64[i];
            if (dynSymAddr == section.section_addr) {
                foundSection = true;
                dynSymSize = section.section_size;
            }
        }
        //
        if (!foundSection)   return;
        //
        uint64_t symbolTabOffset = addrToFileOffset(dynSymAddr);
        uint64_t symTotal = dynSymSize / dynSymEnt;
        if (symbolTabOffset == 0)   return;
        if (symTotal == 0)   return;
        elfSeek(symbolTabOffset, SEEK_SET);
        std::vector<Elf64Symbol> symbols;
        for (size_t i = 0; i < symTotal; i++) {
            Elf64Symbol symbol;
            symbol.name = read_u32();
            symbol.info = (uint8_t) fgetc(elf);
            symbol.other = (uint8_t) fgetc(elf);
            symbol.shndx = read_u16();
            symbol.value = read_u64();
            symbol.size = read_u64();
            symbols.push_back(symbol);
        }
        printf("\n动态链接符号表, 文件偏移: ");
        if (symbolTabOffset > 0xffffffff)
            printf("0x%016llx", symbolTabOffset);
        else
            printf("0x%08x", (uint32_t)symbolTabOffset);
        printf(", 内存地址: ");
        if (dynSymAddr > 0xffffffff)
            printf("0x%016llx", dynSymAddr);
        else
            printf("0x%08x", (uint32_t)dynSymAddr);
        printf(", 符号条目数: %llu", symTotal);
        putchar('\n');
        showSymbolData(symbols, dynstr);
    }


    void parse2() {
        if (elf == nullptr) return;
        try {
            dumpElfHeader();
            dumpSectionHeader();
            dumpProgramHeader();
            dumpDynamicSegment();
            dumpDynamicReloc();
            dumpDynamicSymbol();
        }
        catch (std::string s) {

        }
    }

public:
    ElfParser(const char * filename, long long start, long long elfSize): elfStartOffset(start), elfTotalSize(elfSize) {
        elf = fopen_utf8(filename, "rb");
    }

    ~ElfParser() {
        if (elf)    fclose(elf);
        elf = nullptr;
        if (shstrtab != nullptr)  delete[] shstrtab;
        shstrtab = nullptr;
        if (dynstr != nullptr) delete[] dynstr;
        dynstr = nullptr;
    }

    void parse() {
        parse2();
    }
};


void dumpElfContent(const char * filename, long long start, long long elfSize)
{
    ElfParser elfParser(filename, start, elfSize);
    elfParser.parse();
}

void dumpElfFile(const char * filename)
{
    long long fileSzie = get_file_size(filename);
    dumpElfContent(filename, 0, fileSzie);
}












