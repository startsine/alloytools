#pragma once

#include <list>
#include <string>
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
	virtual int dumpContent(FILE * fp, size_t start, size_t size, const DumperParam & param) = 0;
};

class ElfDumper : public ContentDumper
{
private:
	virtual int dumpContent(FILE* fp, size_t start, size_t size, const DumperParam& param);
	virtual int readElfHeader();
	void showElfHeader();
	static const char* abistr(uint8_t abi);
	static const char* cpustr(uint16_t cpu);
protected:
	bool				isElf64;
	FILE*				fp;
	ELF64Header			header;
};

class Elf32Dumper : public ElfDumper
{
private:
	virtual int readElfHeader();
};

class Elf64Dumper : public ElfDumper
{
private:
	virtual int readElfHeader();
};


/// <summary>
/// //////////////////////////////////////////////
/// </summary>
class PECoffDumper : public ContentDumper
{
private:
	virtual int dumpContent(FILE* fp, size_t start, size_t size, const DumperParam& param);
};




