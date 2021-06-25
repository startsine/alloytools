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



