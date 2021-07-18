#pragma once

#include <list>
#include <vector>
#include <string>
#include <memory>
#include "elf.h"

class DumperParam
{
public:
	DumperParam();
	~DumperParam();
	std::list<std::string>& getSrc();
	int parseParam(int argc, char** argv);
private:
	std::list<std::string>  sources;
};

class Dumper
{
public:
	Dumper();
	~Dumper();
	void startDump(int argc, char** argv);
private:
	DumperParam		params;
};

class ContentDumper
{
public:
	virtual int dumpContent(FILE * fp, uint64_t start, uint64_t contentSize, const DumperParam & param) = 0;
protected:
	uint64_t			startPosition;
	uint64_t			contentSize;
	FILE*				fp;
	const DumperParam*	param;
};

class ElfDumper : public ContentDumper
{
protected:
	virtual int dumpContent(FILE* fp, uint64_t start, uint64_t contentSize, const DumperParam& param);
	virtual int readElfHeader();
	virtual int readProgramHeader();
	virtual int readSectionHeader();
	virtual int readDynamicInfo(const ELF64ProgramHeader& proHeader);
	virtual int readDynamicSymTab(const ELF64ProgramHeader& proHeader);
	void showElfHeader();
	void showProgramHeader();
	void showSectionHeader();
	void showProgramHeaderMapSection();
	bool existDynamicProgramHeader();
	void showDynamicInfo(int dynIndex);
	void showDynamicSymTab(int dynIndex);
	uint64_t getDynamicValue(bool & exist, uint64_t tag);
	uint64_t programHeaderAddrToFileOffset(bool & error, uint64_t vaddr);
	static const char* abistr(uint8_t abi);
	static const char* cpustr(uint16_t cpu);
	static const char* programHeaderTypeStr(uint32_t type);
	static const char* sectionTypeStr(uint32_t type);
	static const char* dynamicTagToStr(uint64_t tag);
protected:
	bool								isElf64;
	ELF64Header							header;
	std::vector<ELF64ProgramHeader>		programHeaders;
	std::vector<Elf64SectionHeader>		sectionHeaders;
	std::shared_ptr<char>				secNameStrTab;
	std::vector<Elf64DynEntry>			dynInfos;
	int64_t								dynStrTabSize;
	std::shared_ptr<char>				dynStrTab;
};

class Elf32Dumper : public ElfDumper
{
private:
	virtual int readElfHeader();
	virtual int readProgramHeader();
	virtual int readSectionHeader();
	virtual int readDynamicInfo(const ELF64ProgramHeader& proHeader);
	virtual int readDynamicSymTab(const ELF64ProgramHeader& proHeader);
};

class Elf64Dumper : public ElfDumper
{
private:
	virtual int readElfHeader();
	virtual int readProgramHeader();
	virtual int readSectionHeader();
	virtual int readDynamicInfo(const ELF64ProgramHeader& proHeader);
	virtual int readDynamicSymTab(const ELF64ProgramHeader& proHeader);
};


/// <summary>
/// //////////////////////////////////////////////
/// </summary>
class PECoffDumper : public ContentDumper
{
private:
	virtual int dumpContent(FILE* fp, uint64_t start, uint64_t contentSize, const DumperParam& param);
};




