#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "at_io.h"
#include "at_elf.h"

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
    char * strtab = nullptr;
    uint64_t strtabSize = 0;
    std::vector<const char * > sectionNameList;

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
        printf("ABI 版本:         %d\n", ident[6]);

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
        printf("程序入口地址:         0x%016llu\n", e_entry);

        // 程序头表位置: e_phoff
        unsigned long long e_phoff = read_u64();
        printf("程序头表位置:         0x%016llu\n", e_phoff);
        programTableOffset = e_phoff;

        // 节表头位置: e_shoff
        unsigned long long e_shoff = read_u64();
        printf("节表头位置:         0x%016llu\n", e_shoff);
        sectionTableOffset = e_shoff;

        // 处理器特殊标志: e_flags
        unsigned int e_flags = read_u32();
        printf("处理器特殊标志:         0x%08u\n", e_flags);

        // ELF头大小: e_ehsize
        unsigned short e_ehsize = read_u16();
        printf("ELF 头大小:         0x%d\n", (int)e_ehsize);

        // 程序头表项大小: e_phentsize
        unsigned short e_phentsize = read_u16();
        printf("程序头表项大小:         0x%d\n", (int)e_phentsize);

        // 程序头表项个数: e_phnum
        unsigned short e_phnum = read_u16();
        printf("程序头表项个数:         0x%d\n", (int)e_phnum);
        programEntryCount = e_phnum;

        // 节表项大小: e_shentsize
        unsigned short e_shentsize = read_u16();
        printf("节表项大小:         0x%d\n", (int)e_shentsize);

        // 节表项个数: e_shnum
        unsigned short e_shnum = read_u16();
        printf("节表项个数:         0x%d\n", (int)e_shnum);
        sectionEntryCount = e_shnum;

        // 字符串表节的索引: e_shstrndx
        unsigned short e_shstrndx = read_u16();
        printf("字符串表节的索引:         0x%d\n", (int)e_shstrndx);
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

            for (uint32_t i = 1; i < sectionTable64.size(); i++) {
                printf("name: %s\n", sectionNameList[i]);
            }
            //
        }
        //
        


    }



    void parse2() {
        if (elf == nullptr) return;
        try {
            dumpElfHeader();
            dumpSectionHeader();
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












