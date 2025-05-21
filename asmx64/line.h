#ifndef ASMX64_LINE_PROCESS_H
#define ASMX64_LINE_PROCESS_H 1

#include <stdio.h>
#include <string>
#include <vector>

class SourceLoader
{
private:
    const int BUFF_SIZE = 65536;
    std::vector<std::string> serachPathList;
    FILE * fileStream;
    char * buffer;
    bool endOfFile;
    int endCursor;                              // 有效数据结尾指针
    int curCursor;                              // 当前读取指针
public:
    SourceLoader();
    SourceLoader(const std::vector<std::string> & paths);
    void addSearchPath(const std::string & path);
    void loadFile(const std::string & filepath);
    unsigned char getByte(bool * pEOF);
};


class SourceLinePrePro
{
public:
    std::string                 rawLine;            // 源码行的原始内容
    std::vector<std::string>    tokens;             // token列表
};




#endif // ASMX64_LINE_PROCESS_H
