#ifndef ASMX64_LINE_PROCESS_H
#define ASMX64_LINE_PROCESS_H 1

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <list>


class SourceLoader
{
private:
    static constexpr int BUFF_SIZE = 4096;
    std::vector<std::string> serachPathList;
    FILE * fileStream;
    char buffer[BUFF_SIZE + 2];
    bool endOfFile;
    int endCursor;                              // 有效数据结尾指针
    int curCursor;                              // 当前读取指针
public:
    SourceLoader();
    SourceLoader(const std::vector<std::string> & paths);
    ~SourceLoader();
    void addSearchPath(const std::string & path);
    void loadFile(const std::string & filepath);
    uint8_t getByte(bool * pEOF);
    uint8_t preGet();
    uint8_t preGetNext();
};


class SourceLinePrePro
{
public:
    std::string                 rawLine;            // 源码行的原始内容
    std::vector<std::string>    tokens;             // token列表
    void clear();
};

class SourceLine
{
public:
    std::string rawContent;                          // 源码行的原始内容
    std::string bincode;                             // 改行对应的二进制代码
    std::vector<X64Expression> expressions;     // 当前行的表达式列表(指令后面的表达式)
    bool hasInsn = false;                        // 该行是否拥有指令(包括指令与伪指令、虚拟指令等)
    bool hasLabel = false;                       // 该行是否拥有标签符号
    bool hasInsnPrefix = false;                  // 是否拥有指令前缀
    std::string labelStr = "";                        // 标签符号字符串
    std::string insnStr = "";                         // 指令字符串
    int64_t sectionIndex = -1;            // 当前行产生的指令或者数据属于哪一个 section
    uint32_t prefixs = (uint32_t) CpuInsnPrefixID::None;  // 指令前缀列表
    bool keepFollowingToken = false;             // 标志：是否保留了指令后面的token
    std::vector<std::string>     followingTokens;   // 紧跟指令后面的原始token, keepFollowingToken为true时有效
    std::vector<uint8_t> code;                // 本行产生的代码
    std::vector<RelocInfo> relocInfos;                  // 本行中的重定位信息

    //public bool isCpuInsn = false;                      // 如果该行拥有指令，该指令是否CPU真实指令，如果为false则表示是伪指令
    //public uint bytesize = 0;                           // 该行产生的机器代码的字节大小（如果是虚拟指令或者Jcc指令，这里先存放它的可能最长的大小,后面再扫描修正）
};

class SourceParser
{
private:
    std::list<SourceLinePrePro> * lines;
    SourceLoader * loader;
    bool isIdentifierStart(uint8_t ch);
    bool isIdentifierNext(uint8_t ch);
    bool isNumericStart(uint8_t ch);
    bool isNumericNext(uint8_t ch);
    bool testTokenBuffIsAllDecimal(uint8_t * buffer, int size);
public:
    SourceParser(std::list<SourceLinePrePro> * lines, SourceLoader * loader);
    bool parse();
};


#endif // ASMX64_LINE_PROCESS_H
