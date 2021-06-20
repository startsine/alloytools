#include <stdio.h>
#include <string.h>
#include <string>
#include "dumper.h"

extern "C" 
#ifdef _WIN32
__declspec(dllexport) 
#endif
int dumper_v1(int argc, char** argv)
{
    printf("我测试看呀");
    return 0;
}

static int parse_param()
{

}

DumperParam::DumperParam()
{
}

DumperParam::~DumperParam()
{
}

std::list<std::string>& DumperParam::getSrc()
{
    return sources;
}

Dumper::Dumper()
{
}

Dumper::~Dumper()
{
}

void Dumper::startDump()
{
}
