#include <stdio.h>
#include <string.h>
#include <string>
#include <memory>
#include "dumper.h"
#include "utils.h"

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

enum class DetectType
{
    unknown, elf, pecoff, msarch, arch, omf
};

void Dumper::startDump(int argc, char** argv)
{
    int ret;
    ret = this->params.parseParam(argc, argv);

    auto sources = this->params.getSrc();

    for (auto it = sources.begin(); it != sources.end(); it++) {
        DetectType type = DetectType::unknown;
        auto& src = *it;
        FILE* fp;
        uint8_t flag[4];

        fp = fopen_utf8_filename(src.c_str(), "rb");
        if (fp == nullptr) {
            continue;
        }

        flag[0] = flag[1] = flag[2] = flag[3] = 0;
        fread(flag, 1, 4, fp);
        
        if (flag[0] == 0x7f && flag[1] == 'E' && flag[2] == 'L' && flag[3] == 'F') {
            type = DetectType::elf;
        }
        else if (flag[0] == 'M' && flag[1] == 'Z') {
            type = DetectType::pecoff;
        }

        fclose(fp);
    }

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


