#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "at_io.h"
#include "at_elf.h"
#include "at_util.h"

class ElfParser {
private:
    FILE * elf;
    const long long elfStartOffset;
    const long long elfTotalSize;
    bool isBigEndian;                           
    bool isElf64;
    unsigned short stringTableSectionIndex = 0;
    uint64_t sectionTableOffset = 0;
    uint16_t sectionEntryCount = 0;
    uint64_t programTableOffset = 0;
    uint16_t programEntryCount = 0;
    std::vector<Elf64SectionEntry> sectionTable64;
    std::vector<const char * > sectionNameList;
    char * strtab = nullptr;
    uint64_t strtabSize = 0;
    std::vector<Elf64ProgramEntry> programTable64;

private:

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

    const char * getString(uint64_t offset) {
        if (strtab == nullptr)  return "";
        if (offset >= strtabSize) return "";
        const char * str = &strtab[offset];
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
            case 0:
                return "NULL";
            case 1:
                return "LOAD";
            case 2:
                return "DYNAMIC";
            case 3:
                return "INTERP";
            case 4:
                return "NOTE";
            case 5:
                return "SHLIB";
            case 6:
                return "PHDR";
            case 7:
                return "TLS";
            case 8:
                return "NUM";                   //umber of defined types
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
                strtabSize = section.section_size;
                strtab = new char[strtabSize + 4];
                elfSeek((int64_t)section.section_offset, SEEK_SET);
                fread(strtab, 1, (size_t)strtabSize);
            }
            // 读取所有section名
            sectionNameList.clear();
            for (uint32_t i = 0; i < sectionTable64.size(); i++) {
                const char * str = getString(sectionTable64[i].section_name);
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

                putchar('\n');
            }
        }
    }


    void parse2() {
        if (elf == nullptr) return;
        try {
            dumpElfHeader();
            dumpSectionHeader();
            dumpProgramHeader();
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
        if (strtab != nullptr)  delete[] strtab;
        strtab = nullptr;
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












