#ifndef ASMX64_LINE_PROCESS_H
#define ASMX64_LINE_PROCESS_H 1

#include <string>
#include <vector>

class SourceLinePrePro
{
public:
    std::string                 rawLine;            // 源码行的原始内容
    std::vector<std::string>    tokens;             // token列表
};


#endif // ASMX64_LINE_PROCESS_H
