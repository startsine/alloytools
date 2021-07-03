#include <stdio.h>
#include <string.h>
#include <string>
#include <memory>
#include "dumper.h"
#include "utils.h"

int ElfDumper::dumpContent(FILE* fp, size_t start, size_t size, const DumperParam& param)
{
    int err;
    uint8_t ident[16];

    file_seek(fp, start, SEEK_SET);
    this->fp = fp;
    err = this->readElfHeader();
    if (err) {
        printf("格式错误，遇到不正常的文件结束\n");
        return -1;
    }
    if (header.ident[EI_POS_CLASS] == 1) {
        isElf64 = false;
    } if (header.ident[EI_POS_CLASS] == 2) {
        isElf64 = true;
    }
    this->showElfHeader();
    



    return 0;
}

int ElfDumper::readElfHeader()
{
    return -1;
}

int Elf32Dumper::readElfHeader()
{
    memset(header.ident, 0, sizeof(header.ident));
    fread(header.ident, 1, sizeof(header.ident), fp);
    header.type = fread_u16(fp);
    header.machine = fread_u16(fp);
    header.version = fread_u32(fp);
    header.entry = fread_u32(fp);
    header.phoff = fread_u32(fp);
    header.shoff = fread_u32(fp);
    header.flags = fread_u32(fp);
    header.ehsize = fread_u16(fp);
    header.phentsize = fread_u16(fp);
    header.phnum = fread_u16(fp);
    header.shentsize = fread_u16(fp);
    header.shnum = fread_u16(fp);
    header.shstrndx = fread_u16(fp);
    if (feof(fp))
        return -1;
    return 0;
}

int Elf64Dumper::readElfHeader()
{
    memset(header.ident, 0, sizeof(header.ident));
    fread(header.ident, 1, sizeof(header.ident), fp);
    header.type = fread_u16(fp);
    header.machine = fread_u16(fp);
    header.version = fread_u32(fp);
    header.entry = fread_u64(fp);
    header.phoff = fread_u64(fp);
    header.shoff = fread_u64(fp);
    header.flags = fread_u32(fp);
    header.ehsize = fread_u16(fp);
    header.phentsize = fread_u16(fp);
    header.phnum = fread_u16(fp);
    header.shentsize = fread_u16(fp);
    header.shnum = fread_u16(fp);
    header.shstrndx = fread_u16(fp);
    if (feof(fp))
        return -1;
    return 0;
}

void ElfDumper::showElfHeader()
{
    const char* text;
    char buffer[256];
    printf("基本信息：\n");
    printf("    ELF信息：           %s v%d, %s ABI(0x%02x) v%d, %s\n", 
        isElf64 ? "ELF64" : "ELF32", 
        header.ident[EI_POS_VERSION],
        abistr(header.ident[EI_POS_OSABI]),
        header.ident[EI_POS_OSABI],
        header.ident[EI_POS_ABIVERSION],
        header.ident[EI_POS_DATA] == 2 ? "大端" : "小端"
        );
    switch (header.type)
    {
    case 1:
        text = "0x0001 REL (可重定位目标文件)";
        break;
    case 2:
        text = "0x0002 EXEC (可执行文件)";
        break;
    case 3:
        text = "0x0003 DYN (动态库)";
        break;
    case 4:
        text = "0x0004 Core";
        break;
    default:
        sprintf(buffer, "0x%04x", header.type);
        text = buffer;
        break;
    }
    printf("    文件类型：           %s\n", text);
    printf("    机器架构：           0x%04x (%s)\n", header.machine, cpustr(header.machine));
    printf("    版本：              %d\n", header.version);
    if (header.entry > 0xffffffff)
        printf("    程序入口地址：           0x%016llx\n", header.entry);
    else
        printf("    程序入口地址：           0x%08x\n", (uint32_t)header.entry);
    if (header.phoff > 0xffffffff)
        printf("    程序头偏移：           0x%016llx\n", header.phoff);
    else
        printf("    程序头偏移：           0x%08x\n", (uint32_t)header.phoff);
    if (header.shoff > 0xffffffff)
        printf("    节表头偏移：           0x%016llx\n", header.shoff);
    else
        printf("    节表头偏移：           0x%08x\n", (uint32_t)header.shoff);

    printf("    ELF头部大小：           %d\n", header.ehsize);
    printf("    程序头项目大小：           %d\n", header.phentsize);
    printf("    程序头项目个数：           %d\n", header.phnum);

    printf("    节表头项目大小：           %d\n", header.shentsize);
    printf("    节表头项目个数：           %d\n", header.shnum);
    printf("    节名字符串节索引：           %d\n", header.shstrndx);
    /*
    
    uint16_t        ehsize;
    uint16_t        phentsize;
    uint16_t        phnum;
    uint16_t        shentsize;
    uint16_t        shnum;
    uint16_t        shstrndx;
    */
}

const char* ElfDumper::abistr(uint8_t abi)
{
    return "Linux";
}

const char* ElfDumper::cpustr(uint16_t cpu)
{
    return "AMD64";
}

