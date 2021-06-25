#include <stdio.h>
#include <string.h>
#include <string>
#include <memory>
#include "dumper.h"

extern "C" 
#ifdef _WIN32
__declspec(dllexport) 
#endif
int dumper_v1(int argc, char** argv)
{
    auto dumper = std::make_shared<Dumper>();
    try {
        dumper->startDump(argc, argv);
    }
    catch(...) {

    }
    return 0;
}

std::list<std::string>& DumperParam::getSrc()
{
    return sources;
}

Dumper::Dumper()
{
    printf("dumper()\n");
}

Dumper::~Dumper()
{
    printf("~dumper()\n");
}

void Dumper::startDump(int argc, char** argv)
{
    int ret;
    ret = this->params.parseParam(argc, argv);

}

DumperParam::DumperParam()
{
}

DumperParam::~DumperParam()
{
}

int DumperParam::parseParam(int argc, char** argv)
{
    if (argc == 1)
        return 0;
    for (int i = 1; i < argc; i++) {
        const auto argstr = argv[1];
        if (argstr[0] == '-') {
            // param
        } else {
            this->sources.push_back(argstr);
        }
    }
    return 0;
}


