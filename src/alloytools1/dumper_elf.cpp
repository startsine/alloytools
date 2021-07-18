#include <stdio.h>
#include <string.h>
#include <string>
#include <memory>
#include "dumper.h"
#include "utils.h"

int ElfDumper::dumpContent(FILE* fp, uint64_t start, uint64_t contentSize, const DumperParam& param)
{
    int err;
    uint8_t ident[16];

    this->startPosition = start;
    this->contentSize = contentSize;
    this->fp = fp;
    this->param = &param;

    file_seek(fp, start, SEEK_SET);
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
    printf("\n");
    
    // program header
    if (header.phnum) {
        if (header.phnum * (uint64_t)header.phentsize + header.phoff > contentSize) {
            printf("格式错误，遇到不正常的文件结束\n");
            return -1;
        }
        file_seek(fp, start + header.phoff, SEEK_SET);
        this->readProgramHeader();
        this->showProgramHeader();
        printf("\n");
    }

    // section table header
    if (header.shnum) {
        if (header.shnum * (uint64_t)header.shentsize + header.shoff > contentSize) {
            printf("格式错误，遇到不正常的文件结束\n");
            return -1;
        }
        file_seek(fp, start + header.shoff, SEEK_SET);
        this->readSectionHeader();
        this->showSectionHeader();
        printf("\n");
    }

    // map
    if (header.phnum && header.shnum) {
        this->showProgramHeaderMapSection();
        printf("\n");
    }

    // exe/so dynamic info
    if (existDynamicProgramHeader()) {
        int dynCount = 0;
        for (auto itp = programHeaders.begin(); itp != programHeaders.end(); itp++) {
            if (itp->type == ELF_PROGRAM_TYPE_DYNAMIC) {
                if (itp->offset + itp->filesz > contentSize) {
                    printf("格式错误，遇到不正常的文件结束\n");
                    continue;
                }
                //
                file_seek(fp, start + itp->offset, SEEK_SET);
                this->readDynamicInfo(*itp);
                this->showDynamicInfo(dynCount);
                //
                this->readDynamicSymTab(*itp);
                this->showDynamicSymTab(dynCount);

            }
        }
    }
    else {
        printf("不存在动态链接信息 \n");
    }


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

int ElfDumper::readProgramHeader()
{
    return -1;
}

int Elf32Dumper::readProgramHeader()
{
    ELF64ProgramHeader prgmHeader;
    for (uint16_t i = 0; i < header.phnum; i++) {
        prgmHeader.type = fread_u32(fp);
        prgmHeader.offset = fread_u32(fp);
        prgmHeader.vaddr = fread_u32(fp);
        prgmHeader.paddr = fread_u32(fp);
        prgmHeader.filesz = fread_u32(fp);
        prgmHeader.memsz = fread_u32(fp);
        prgmHeader.flags = fread_u32(fp);
        prgmHeader.align = fread_u32(fp);
        this->programHeaders.push_back(prgmHeader);
    }
    if (feof(fp))
        return -1;
    return 0;
}

int Elf64Dumper::readProgramHeader()
{
    ELF64ProgramHeader prgmHeader;
    for (uint16_t i = 0; i < header.phnum; i++) {
        prgmHeader.type = fread_u32(fp);
        prgmHeader.flags = fread_u32(fp);
        prgmHeader.offset = fread_u64(fp);
        prgmHeader.vaddr = fread_u64(fp);
        prgmHeader.paddr = fread_u64(fp);
        prgmHeader.filesz = fread_u64(fp);
        prgmHeader.memsz = fread_u64(fp);
        prgmHeader.align = fread_u64(fp);
        this->programHeaders.push_back(prgmHeader);
    }
    if (feof(fp))
        return -1;
    return 0;
}

void ElfDumper::showProgramHeader()
{
    bool largeOffset = false;
    bool largeVirAddr = false;
    bool largePhyAddr = false;
    bool largeFileSize = false;
    bool largeMemSize = false;
    int  programHeaderId;
    char alignText[32];
    char flagsText[128];

    for (auto it = programHeaders.begin(); it != programHeaders.end(); it++) {
        if (it->offset > 0xffffffff) {
            largeOffset = true;
            break;
        }
    }

    for (auto it = programHeaders.begin(); it != programHeaders.end(); it++) {
        if (it->vaddr > 0xffffffff) {
            largeVirAddr = true;
            break;
        }
    }

    for (auto it = programHeaders.begin(); it != programHeaders.end(); it++) {
        if (it->paddr > 0xffffffff) {
            largePhyAddr = true;
            break;
        }
    }

    for (auto it = programHeaders.begin(); it != programHeaders.end(); it++) {
        if (it->filesz > 0xffffffff) {
            largeFileSize = true;
            break;
        }
    }

    for (auto it = programHeaders.begin(); it != programHeaders.end(); it++) {
        if (it->memsz > 0xffffffff) {
            largeMemSize = true;
            break;
        }
    }

    if (header.phnum == 0)
        return;
    printf("程序头表: \n");

    printf("  #  类型           文件偏移   虚拟地址   物理地址   文件大小   内存大小    对齐 属性\n");
    programHeaderId = 0;
    for (auto it = programHeaders.begin(); it != programHeaders.end(); it++) {
        if (header.phnum < 10)
            printf("  %d ", programHeaderId);
        else if (header.phnum < 100)
            printf("  %02d ", programHeaderId);
        else if (header.phnum < 1000)
            printf("  %03d ", programHeaderId);
        else if (header.phnum < 10000)
            printf("  %04d ", programHeaderId);
        else
            printf("  %05d ", programHeaderId);

        printf("%-14s ", programHeaderTypeStr(it->type));
        if (largeOffset)
            printf("0x%016llx ", it->offset);
        else
            printf("0x%08x ", (uint32_t)it->offset);
        if (largeVirAddr)
            printf("0x%016llx ", it->vaddr);
        else
            printf("0x%08x ", (uint32_t)it->vaddr);
        if (largePhyAddr)
            printf("0x%016llx ", it->paddr);
        else
            printf("0x%08x ", (uint32_t)it->paddr);
        if (largeFileSize)
            printf("0x%016llx ", it->filesz);
        else
            printf("0x%08x ", (uint32_t)it->filesz);
        if (largeMemSize)
            printf("0x%016llx ", it->memsz);
        else
            printf("0x%08x ", (uint32_t)it->memsz);
        if (it->align > 0xffffffff)
            sprintf(alignText, "%llu", it->align);
        else
            sprintf(alignText, "%u", (uint32_t)it->align);
        printf("%5s ", alignText);
        flagsText[0] = 0;
        if (it->flags & 0x4)
            strcat(flagsText, "READ ");
        if (it->flags & 0x2)
            strcat(flagsText, "WRITE ");
        if (it->flags & 0x1)
            strcat(flagsText, "EXEC ");
        printf("%s  ", flagsText);
        // loader
        if (it->type == 3) {        // PT_INTERP
            std::shared_ptr<char> interp(new char[it->filesz + 4](), std::default_delete<char[]>());
            file_seek(fp, this->startPosition + it->offset, SEEK_SET);
            fread(interp.get(), 1, it->filesz, this->fp);
            printf("(%s)", interp.get());
        }
        printf(" \n");
        programHeaderId++;
    }


    
}

const char* ElfDumper::programHeaderTypeStr(uint32_t type)
{
    static char typeName[12];
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
    case 0x6474e550:
        return "GNU_EH_FRAME";
    case 0x6474e551:
        return "GNU_STACK";
    case 0x6474e552:
        return "GNU_RELRO";
    default:
        sprintf(typeName, "%08x", type);
        return typeName;
    }
}

int ElfDumper::readSectionHeader()
{
    return -1;
}

int Elf32Dumper::readSectionHeader()
{
    Elf64SectionHeader section;
    for (uint16_t i = 0; i < header.shnum; i++) {
        section.name = fread_u32(fp);
        section.type = fread_u32(fp);
        section.flags = fread_u32(fp);
        section.addr = fread_u32(fp);
        section.offset = fread_u32(fp);
        section.size = fread_u32(fp);
        section.link = fread_u32(fp);
        section.info = fread_u32(fp);
        section.addralign = fread_u32(fp);
        section.entsize = fread_u32(fp);
        this->sectionHeaders.push_back(section);
    }
    if (feof(fp))
        return -1;
    return 0;
}

int Elf64Dumper::readSectionHeader()
{
    Elf64SectionHeader section;
    for (uint16_t i = 0; i < header.shnum; i++) {
        section.name = fread_u32(fp);
        section.type = fread_u32(fp);
        section.flags = fread_u64(fp);
        section.addr = fread_u64(fp);
        section.offset = fread_u64(fp);
        section.size = fread_u64(fp);
        section.link = fread_u32(fp);
        section.info = fread_u32(fp);
        section.addralign = fread_u64(fp);
        section.entsize = fread_u64(fp);
        this->sectionHeaders.push_back(section);
    }
    if (feof(fp))
        return -1;
    return 0;
}

void ElfDumper::showSectionHeader()
{
    int sectionId;
    bool largeOffset = false;
    bool largeAddr = false;
    bool largeSize = false;
    char flagsText[16];
    
    for (auto it = sectionHeaders.begin(); it != sectionHeaders.end(); it++) {
        if (it->offset > 0xffffffff) {
            largeOffset = true;
            break;
        }
    }

    for (auto it = sectionHeaders.begin(); it != sectionHeaders.end(); it++) {
        if (it->addr > 0xffffffff) {
            largeAddr = true;
            break;
        }
    }

    for (auto it = sectionHeaders.begin(); it != sectionHeaders.end(); it++) {
        if (it->size > 0xffffffff) {
            largeSize = true;
            break;
        }
    }

    if (header.shnum == 0)
        return;
    printf("节表: \n");
    printf("  #  类型     虚拟地址   文件偏移   数据大小     项大小  link  对齐  属性  info  节名\n");

    // load section name strtab
    if (header.shstrndx < header.shnum) {
        auto & sec = sectionHeaders[header.shstrndx];
        auto size = sec.size;
        if (sec.offset + this->startPosition + size <= this->contentSize) {
            secNameStrTab = std::shared_ptr<char> (new char[size + 4](), std::default_delete<char[]>());
            file_seek(fp, sec.offset + this->startPosition, SEEK_SET);
            fread(secNameStrTab.get(), 1, size, fp);
        }
    }

    sectionId = 0;
    for (auto it = sectionHeaders.begin(); it != sectionHeaders.end(); it++) {
        printf("  ");
        if (header.shnum < 10)
            printf("%d ", sectionId);
        else if (header.shnum < 100)
            printf("%02d ", sectionId);
        else if (header.shnum < 1000)
            printf("%03d ", sectionId);
        else if (header.shnum < 10000)
            printf("%04d ", sectionId);
        else
            printf("%05d ", sectionId);
        printf("%8s ", sectionTypeStr(it->type));
        if (largeAddr)
            printf("0x%016llx ", it->addr);
        else
            printf("0x%08x ", (uint32_t)it->addr);
        if (largeOffset)
            printf("0x%016llx ", it->offset);
        else
            printf("0x%08x ", (uint32_t)it->offset);
        if (largeSize)
            printf("0x%016llx ", it->size);
        else
            printf("0x%08x ", (uint32_t)it->size);
        printf("%8llu ", it->entsize);
        printf("%5d ", it->link);
        printf("%5lld ", it->addralign);
        flagsText[0] = 0;
        if (it->flags & 0x0001)
            strcat(flagsText, "W");
        if (it->flags & 0x0002)
            strcat(flagsText, "A");
        if (it->flags & 0x0004)
            strcat(flagsText, "E");
        printf("%5s ", flagsText);
        printf("%5d ", it->info);

        if (secNameStrTab) {
            auto p = secNameStrTab.get();
            printf(" %s ", &p[it->name]);
        }

        sectionId++;
        printf("\n");
    }

}

const char* ElfDumper::sectionTypeStr(uint32_t type)
{
    static char typeName[12];
    switch (type)
    {
    case 0x00:
        return "NULL";
    case 0x01:
        return "PROGBITS";
    case 0x02:
        return "SYMTAB";
    case 0x03:
        return "STRTAB";
    case 0x04:
        return "RELA";
    case 0x05:
        return "HASH";
    case 0x06:
        return "DYNAMIC";
    case 0x07:
        return "NOTE";
    case 0x08:
        return "NOBITS";
    case 0x09:
        return "REL";
    case 0x0A:
        return "SHLIB";
    case 0x0B:
        return "DYNSYM";
    default:
        sprintf(typeName, "%08x", type);
        return typeName;
    }
}

void ElfDumper::showProgramHeaderMapSection()
{
    size_t maxTypeLen = 0;
    char maxTypeLenSF[16];
    int programHeaderId;

    for (auto it = programHeaders.begin(); it != programHeaders.end(); it++) {
        auto str = programHeaderTypeStr(it->type);
        auto curLen = strlen(str);
        if (maxTypeLen < curLen) {
            maxTypeLen = curLen;
        }
    }
    maxTypeLen++;
    sprintf(maxTypeLenSF, "%%-%ds ", (int32_t)maxTypeLen);

    printf("映射表: \n");

    programHeaderId = 0;
    for (auto itp = programHeaders.begin(); itp != programHeaders.end(); itp++) {
        std::vector<std::string> secList;
        for (auto its = sectionHeaders.begin(); its != sectionHeaders.end(); its++) {
            if (its->type == ELF_SECTION_TYPE_NULL)
                continue;
            if (its->offset >= itp->offset && (its->offset + its->size) <= (itp->offset + itp->filesz)) {
                if (secNameStrTab) {
                    auto p = secNameStrTab.get();
                    secList.push_back(&p[its->name]);
                }
            }
            else if (its->type == ELF_SECTION_TYPE_NOBITS && itp->type == ELF_PROGRAM_TYPE_LOAD &&
                (its->addr >= itp->vaddr && (its->addr + its->size) <= (itp->vaddr + itp->memsz))) {
                if (secNameStrTab) {
                    auto p = secNameStrTab.get();
                    secList.push_back(&p[its->name]);
                }
            }
        }

        if (secList.size() == 0) {
            programHeaderId++;
            continue;
        }

        if (header.phnum < 10)
            printf("  %d ", programHeaderId);
        else if (header.phnum < 100)
            printf("  %02d ", programHeaderId);
        else if (header.phnum < 1000)
            printf("  %03d ", programHeaderId);
        else if (header.phnum < 10000)
            printf("  %04d ", programHeaderId);
        else
            printf("  %05d ", programHeaderId);

        printf("%-14s ", programHeaderTypeStr(itp->type));
        printf("( ");
        for (auto it3 = secList.begin(); it3 != secList.end(); it3++) {
            printf("%s ", it3->c_str());
        }
        printf(")");

        programHeaderId++;
        printf("\n");
    }
}

bool ElfDumper::existDynamicProgramHeader()
{
    bool exist = false;
    for (auto itp = programHeaders.begin(); itp != programHeaders.end(); itp++) {
        if (itp->type == ELF_PROGRAM_TYPE_DYNAMIC) {
            exist = true;
            break;
        }
    }
    return exist;
}

int ElfDumper::readDynamicInfo(const ELF64ProgramHeader& proHeader)
{
    return -1;
}

int Elf32Dumper::readDynamicInfo(const ELF64ProgramHeader& proHeader)
{
    dynInfos.clear();
    uint64_t datasize = proHeader.filesz;
    uint64_t entryCount = datasize / sizeof(Elf32DynEntry);
    for (uint64_t i = 0; i < entryCount; i++) {
        Elf64DynEntry entry;
        entry.tag = fread_u32(fp);
        entry.un.val = fread_u32(fp);
        dynInfos.push_back(entry);
    }
    if (feof(fp))
        return -1;
    return 0;
}

int Elf64Dumper::readDynamicInfo(const ELF64ProgramHeader& proHeader)
{
    dynInfos.clear();
    uint64_t datasize = proHeader.filesz;
    uint64_t entryCount = datasize / sizeof(Elf64DynEntry);
    for (uint64_t i = 0; i < entryCount; i++) {
        Elf64DynEntry entry;
        entry.tag = fread_u64(fp);
        entry.un.val = fread_u64(fp);
        dynInfos.push_back(entry);
    }
    if (feof(fp))
        return -1;
    return 0;
}

void ElfDumper::showDynamicInfo(int dynIndex)
{
    bool error;
    bool existStrSize = false;
    bool existStrTab = false;
    uint64_t valueStrTab = 0;
    uint64_t offsetStrTab = 0;

    dynStrTabSize = getDynamicValue(existStrSize, ELF_DT_STRSZ);
    valueStrTab = getDynamicValue(existStrTab, ELF_DT_STRTAB);
    offsetStrTab = programHeaderAddrToFileOffset(error, valueStrTab);
    if (error) {
        printf("格式错误，无法定位虚拟地址0x%08llx到文件偏移\n", valueStrTab);
        return;
    }
    dynStrTab = std::shared_ptr<char>(new char[dynStrTabSize + 4](), std::default_delete<char[]>());
    file_seek(fp, startPosition + offsetStrTab, SEEK_SET);
    fread(dynStrTab.get(), 1, dynStrTabSize, fp);

    printf("动态链接信息  #%d:\n", dynIndex);
    printf("    TAG         类型             值 \n");
    for (auto it = dynInfos.begin(); it != dynInfos.end(); it++) {
        if (it->tag == 0)
            continue;
        printf("    0x%08llx  %-12s ", it->tag, dynamicTagToStr(it->tag));
        if (it->tag == ELF_DT_NEEDED) {
            printf("0x%08llx  ", it->un.val);
            if (dynStrTab) {
                printf("(%s)", &(dynStrTab.get()[it->un.val]));
            }
        }
        else if (it->tag == ELF_DT_INIT_ARRAYSZ || it->tag == ELF_DT_FINI_ARRAYSZ || it->tag == ELF_DT_STRSZ || it->tag == ELF_DT_PLTRELSZ ||
            it->tag == ELF_DT_SYMENT) {
            printf("0x%08llx  ", it->un.val);
            printf("(%lld 字节)", it->un.val);
        }
        else {
            printf("0x%08llx", it->un.val);
        }
        printf("\n");
    }
}

uint64_t ElfDumper::getDynamicValue(bool& exist, uint64_t tag)
{
    for (auto it = dynInfos.begin(); it != dynInfos.end(); it++) {
        if (it->tag == tag) {
            exist = true;
            return it->un.val;
        }
    }
    exist = false;
    return 0;
}

const char* ElfDumper::dynamicTagToStr(uint64_t tag) {
    static char tagName[24];
    switch (tag) {
    case ELF_DT_NULL:
        return "NULL";
    case ELF_DT_NEEDED:
        return "NEEDED";
    case ELF_DT_PLTRELSZ:
        return "PLTRELSZ";
    case ELF_DT_PLTGOT:
        return "PLTGOT";
    case ELF_DT_HASH:
        return "HASH";
    case ELF_DT_STRTAB:
        return "STRTAB";
    case ELF_DT_SYMTAB:
        return "SYMTAB";
    case ELF_DT_RELA:
        return "RELA";
    case ELF_DT_RELASZ:
        return "RELASZ";
    case ELF_DT_RELAENT:
        return "RELAENT";
    case ELF_DT_STRSZ:
        return "STRSZ";
    case ELF_DT_SYMENT:
        return "SYMENT";
    case ELF_DT_INIT:
        return "INIT";
    case ELF_DT_FINI:
        return "FINI";
    case ELF_DT_SONAME:
        return "SONAME";
    case ELF_DT_RPATH:
        return "RPATH";
    case ELF_DT_SYMBOLIC:
        return "SYMBOLIC";
    case ELF_DT_REL:
        return "REL";
    case ELF_DT_RELSZ:
        return "RELSZ";
    case ELF_DT_RELENT:
        return "RELENT";
    case ELF_DT_PLTREL:
        return "PLTREL";
    case ELF_DT_DEBUG:
        return "DEBUG";
    case ELF_DT_TEXTREL:
        return "TEXTREL";
    case ELF_DT_JMPREL:
        return "JMPREL";
    case ELF_DT_BIND_NOW:
        return "BIND_NOW";
    case ELF_DT_INIT_ARRAY:
        return "INIT_ARRAY";
    case ELF_DT_FINI_ARRAY:
        return "FINI_ARRAY";
    case ELF_DT_INIT_ARRAYSZ:
        return "INIT_ARRAYSZ";
    case ELF_DT_FINI_ARRAYSZ:
        return "FINI_ARRAYSZ";
    case ELF_DT_LOOS:
        return "LOOS";
    case ELF_DT_GNU_HASH:
        return "GNU_HASH";
    case ELF_DT_HIOS:
        return "HIOS";
    case ELF_DT_LOPROC:
        return "LOPROC";
    case ELF_DT_HIPROC:
        return "HIPROC";
    default:
        sprintf(tagName, "0x%08llx", tag);
        return tagName;
    }
}

uint64_t ElfDumper::programHeaderAddrToFileOffset(bool& error, uint64_t vaddr)
{
    for (auto it = programHeaders.begin(); it != programHeaders.end(); it++) {
        if (vaddr >= it->vaddr && vaddr <= (it->vaddr + it->memsz)) {
            error = false;
            return it->offset + (vaddr - it->vaddr);
        }
    }
    error = true;
    return 0;
}

int ElfDumper::readDynamicSymTab(const ELF64ProgramHeader& proHeader)
{
    return -1;
}

int Elf32Dumper::readDynamicSymTab(const ELF64ProgramHeader& proHeader)
{
    bool error;
    bool existSynSymTab = false;
    bool existSynSymEntrySize = false;
    uint64_t valueDynSymTab;
    uint64_t valueDynSymEntrySize;
    uint64_t offsetDynSymTab;
    valueDynSymTab = getDynamicValue(existSynSymTab, ELF_DT_SYMTAB);
    valueDynSymEntrySize = getDynamicValue(existSynSymEntrySize, ELF_DT_SYMENT);
    offsetDynSymTab = programHeaderAddrToFileOffset(error, valueDynSymTab);

    dynInfos.clear();
    uint64_t datasize = proHeader.filesz;
    uint64_t entryCount = datasize / sizeof(Elf32DynEntry);
    for (uint64_t i = 0; i < entryCount; i++) {
        Elf64DynEntry entry;
        entry.tag = fread_u32(fp);
        entry.un.val = fread_u32(fp);
        dynInfos.push_back(entry);
    }
    if (feof(fp))
        return -1;
    return 0;

    //return -1;
}

int Elf64Dumper::readDynamicSymTab(const ELF64ProgramHeader& proHeader)
{
    return -1;
}

void ElfDumper::showDynamicSymTab(int dynIndex)
{

}
