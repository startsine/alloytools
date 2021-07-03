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
    unknown, elf32, elf64, pecoff, arch, omf
};

void Dumper::startDump(int argc, char** argv)
{
    int ret;
    int i;
    ret = this->params.parseParam(argc, argv);

    auto sources = this->params.getSrc();

    for (auto it = sources.begin(); it != sources.end(); it++) {
        DetectType type = DetectType::unknown;
        auto& src = *it;
        FILE* fp;
        uint8_t flag[12];

        fp = fopen_utf8_filename(src.c_str(), "rb");
        if (fp == nullptr) {
            continue;
        }

        for (i = 0; i < sizeof(flag); i++)
            flag[i] = 0;
        fread(flag, 1, 8, fp);
        fseek(fp, 0, SEEK_SET);
        
        if (flag[0] == 0x7f && flag[1] == 'E' && flag[2] == 'L' && flag[3] == 'F') {
            if (flag[4] == 2)
                type = DetectType::elf64;
            else
                type = DetectType::elf32;
        }
        else if (flag[0] == 'M' && flag[1] == 'Z') {
            type = DetectType::pecoff;
        }
        else if (0 == strcmp((char*)flag, "!<arch>\n")) {
            type = DetectType::arch;
        }

        //
        std::shared_ptr<ContentDumper> dumper;
        switch (type)
        {
        case DetectType::elf32:
            dumper = std::make_shared<Elf32Dumper>();
            break;
        case DetectType::elf64:
            dumper = std::make_shared<Elf64Dumper>();
            break;
        case DetectType::pecoff:
            dumper = std::make_shared<PECoffDumper>();
            break;
        case DetectType::arch:
            dumper = std::make_shared<ElfDumper>();
            break;
        default:
            break;
        }
        if (dumper) {
            printf("查看的文件：%s\n", it->c_str());
            dumper->dumpContent(fp, 0, 1000, this->params);
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


