#pragma once

#include <list>
#include <string>

class DumperParam
{
public:
	DumperParam();
	~DumperParam();
	std::list<std::string>& getSrc();
private:
	std::list<std::string>  sources;
};

class Dumper
{
public:
	Dumper();
	~Dumper();
	void startDump();
private:
	DumperParam		params;
};



