#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include "FileInfo.h"
#include "at_io.h"

// libraryFlag 标志表示命令行加了 -l 
SrcFile::SrcFile(const std::string & name, bool libraryFlag) {
    inputName = name;
}

void SrcFile::open()
{
    if (fp == nullptr) {
        long long fileSize = get_file_size(inputName.c_str());
        fileStartOffset = 0;
        fileTotalSize = fileSize;
        fp = fopen_utf8(inputName.c_str(), "rb");
    }
}

void SrcFile::close()
{
    if (fp != nullptr) {
        fclose(fp);
        fp = nullptr;
    }
}

int SrcFile::seek(long long offset, int origin)
{
    int v = fseek_long(fp, offset, origin);
    if (feof(fp)) {
        throw(std::string("out of file"));
    }
    long long curr = get_file_curr_pointer(fp);
    if (curr > fileStartOffset + fileTotalSize) {
        throw(std::string("out of file"));
    }
    return v;
}

size_t SrcFile::fread(void* buffer, size_t eSize, size_t eCount)
{
    long long curr = get_file_curr_pointer(fp);
    if (curr + eSize * eCount > fileStartOffset + fileTotalSize) {
        throw(std::string("out of file"));
    }
    size_t n = ::fread(buffer, eSize, eCount, fp);
    return n;
}

uint8_t SrcFile::read_u8()
{
    long long curr = get_file_curr_pointer(fp);
    if (curr + 1 > fileStartOffset + fileTotalSize) {
        throw(std::string("out of file"));
    }
    unsigned int a0 = fgetc(fp);
    return (uint8_t) a0;
}

uint16_t SrcFile::read_u16()
{
    long long curr = get_file_curr_pointer(fp);
    if (curr + 2 > fileStartOffset + fileTotalSize) {
        throw(std::string("out of file"));
    }
    unsigned int a0 = fgetc(fp);
    unsigned int a1 = fgetc(fp);
    if (isBigEndian)
        return static_cast<unsigned short>((a0 << 8) | a1);
    return static_cast<unsigned short>((a1 << 8) | a0);
}

uint32_t SrcFile::read_u32()
{
    long long curr = get_file_curr_pointer(fp);
    if (curr + 4 > fileStartOffset + fileTotalSize) {
        throw(std::string("out of file"));
    }
    unsigned int a0 = fgetc(fp);
    unsigned int a1 = fgetc(fp);
    unsigned int a2 = fgetc(fp);
    unsigned int a3 = fgetc(fp);
    if (isBigEndian)
        return ((a0 << 24) | (a1 << 16) | (a2 << 8) | a3);
    return ((a3 << 24) | (a2 << 16) | (a1 << 8) | a0);
}

uint64_t SrcFile::read_u64()
{
    long long curr = get_file_curr_pointer(fp);
    if (curr + 8 > fileStartOffset + fileTotalSize) {
        throw(std::string("out of file"));
    }
    unsigned long long a0 = fgetc(fp);
    unsigned long long a1 = fgetc(fp);
    unsigned long long a2 = fgetc(fp);
    unsigned long long a3 = fgetc(fp);
    unsigned long long a4 = fgetc(fp);
    unsigned long long a5 = fgetc(fp);
    unsigned long long a6 = fgetc(fp);
    unsigned long long a7 = fgetc(fp);
    if (isBigEndian)
        return ((a0 << 56) | (a1 << 48) | (a2 << 40) | (a3 << 32) | (a4 << 24) | (a5 << 16) | (a6 << 8) | a7);
    return ((a7 << 56) | (a6 << 48) | (a5 << 40) | (a4 << 32) | (a3 << 24) | (a2 << 16) | (a1 << 8) | a0);
}


InputList::InputList() {

}

InputList::~InputList() {
    if (fileList.size() != 0) {
        for (size_t i = 0; i < fileList.size(); i++) {
            if (fileList[i] != nullptr) {
                delete fileList[i];
                fileList[i] = nullptr;
            }
        }
    }

}

ObjectFile::ObjectFile(const std::string & name) :
    SrcFile(name, false)
{
}

void ObjectFile::scanObject()
{
    char magic[4];
    uint8_t elfClass, elfData;
    open();
    seek(0, SEEK_SET);
    fread(magic, 1, 4);
    elfClass = read_u8();
    isElf64 = (elfClass == 2);
    elfData = read_u8();
    isBigEndian = (elfData == 2);
    seek(16, SEEK_SET);
    if (isElf64) {
        type = read_u16();
        machine = read_u16();
        version = read_u32();
        entry = read_u64();
        phoff = read_u64();
        shoff = read_u64();
        flags = read_u32();
        ehsize = read_u16();
        phentsize = read_u16();
        phnum = read_u16();
        shentsize = read_u16();
        shnum = read_u16();
        shstrndx = read_u16();
    }
    else {
        type = read_u16();
        machine = read_u16();
        version = read_u32();
        entry = read_u32();
        phoff = read_u32();
        shoff = read_u32();
        flags = read_u32();
        ehsize = read_u16();
        phentsize = read_u16();
        phnum = read_u16();
        shentsize = read_u16();
        shnum = read_u16();
        shstrndx = read_u16();
    }
}

LibraryFile::LibraryFile(const std::string & name) :
    SrcFile(name, true)
{

}

int InputList::addObject(const char * name) 
{
    ObjectFile * obj = new ObjectFile(name);
    obj->fileType = FileType::ELF_OBJECT;
    fileList.push_back(obj);
    return 0;
}


int InputList::addLibrary(const char * name) {
    LibraryFile * lib = new LibraryFile(name);
    lib->fileType = FileType::SYM_DEF;
    fileList.push_back(lib);
    return 0;
}

