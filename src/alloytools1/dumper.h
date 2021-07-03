#pragma once

#include <list>
#include <string>

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
	virtual void show();
};

class Elf32Dumper : public ElfDumper
{
private:
	
};

class Elf64Dumper : public ElfDumper
{
private:
	

	uint8_t			elfHeader[16];
};

class PECoffDumper : public ContentDumper
{
private:
	virtual int dumpContent(FILE* fp, size_t start, size_t size, const DumperParam& param);
};




