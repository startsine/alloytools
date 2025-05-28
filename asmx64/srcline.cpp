#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#ifdef _WIN32
#include "Windows.h"
#endif
#include "asmx64.h"

using namespace std;

enum class CurrTokenStartType   // 当前词法分析的Token起始类型
{
    None,
    Identifier,                     // 标识符开始 (非数字开头，允许字母、数字、$、?、@、_)
    Numeric,                        // 数字
    StringSingleQuote,              // 单引号开始的字符串
    StringDoubleQuote,              // 双引号开始的字符串
    Comment,                        // 注释
};

SourceLoader::SourceLoader(): fileStream(nullptr), endOfFile(false), endCursor(0), curCursor(0)
{
    memset(buffer, 0, sizeof(buffer));
}

SourceLoader::~SourceLoader()
{
    if (fileStream) {
        fclose(fileStream);
    }
}

void SourceLoader::loadFile(const std::string & filepath)
{
#ifdef _WIN32
    int slen = filepath.length();
    shared_ptr<wchar_t> pw(new wchar_t[slen + 2], [](wchar_t * p) { delete[] p; } );
    int number = MultiByteToWideChar(CP_UTF8, 0, filepath.c_str(), -1, pw.get(), slen + 2);
    if (number == 0) {
        //// 错误处理   打印错误信息  
        return;
    }
    fileStream = _wfopen(pw.get(), L"rb");
#else
    fileStream = fopen(filepath.c_str(), "rb");
#endif
    if (fileStream == nullptr) {
        //// 错误处理   打印错误信息  
        return;
    }
    // 预读 2 字节
    size_t readSize = fread(buffer, 1, 2, fileStream);
    if (readSize != 0) {
        endOfFile = false;
        curCursor = 2;
        endCursor = 2 + readSize;
    } else {
        endOfFile = true;
    }
    
    buffer[2] = 0;
    printf("%s\n", buffer);
}

uint8_t SourceLoader::getByte(bool * pEOF)
{
    uint8_t ret;
    *pEOF = false;
    if (curCursor >= endCursor) {
        *pEOF = true;
        return 0;
    }
    ret = buffer[curCursor];
    curCursor++;
    //
    if (curCursor + 2 >= endCursor) {       // 如果只剩下2个或者2个以下字节大小，把剩余字节搬到数组前面，然后再从文件中读取一些数据添加到后面补充  
        if (!endOfFile) {
            int movSize = endCursor - curCursor;
            if (movSize > 0) {
                for (int i = 0; i < movSize; i++) {         // 把剩余的字节移到数组头部  
                    buffer[i] = buffer[curCursor + i];
                }
                curCursor = 0;
                endCursor = movSize;
                if (fileStream != nullptr) {
                    int n = fread(&buffer[movSize], 1, BUFF_SIZE, fileStream);
                    if (n > 0) {
                        endOfFile = false;
                        endCursor += n;
                    }
                    else {
                        endOfFile = true;
                        fclose(fileStream);
                        fileStream = nullptr;
                    }
                }
            }
        }
    }
    return ret;
}

// 预读取下一个字节
uint8_t SourceLoader::preGet()
{
    if (curCursor >= endCursor) {
        return 0;
    }
    return buffer[curCursor];
}

// 预读取下一个的下一个字节，不移动文件指针，到了文件尽头返回0
uint8_t SourceLoader::preGetNext()
{
    if (curCursor + 1 >= endCursor) {
        return 0;
    }
    return buffer[curCursor + 1];
}

void SourceLinePrePro::clear()
{
    rawLine = "";
    tokens.clear();
}

SourceParser::SourceParser(std::list<SourceLinePrePro> * lines, SourceLoader * loader)
{
    this->lines = lines;
    this->loader = loader;
}

// 是否是符号首字符
bool SourceParser::isIdentifierStart(uint8_t ch)
{
    if (ch >= 128) return true;                             // 大于128的 Unicode 字符
    if (ch >= 0x41 && ch <= 0x5A) return true;              // A-Z
    if (ch >= 0x61 && ch <= 0x7A) return true;              // a-z
    if (ch == '_') return true;                             // _
    if (ch == '$') return true;                             // $
    if (ch == '?') return true;                             // ?
    if (ch == '@') return true;                             // @
    if (ch == '.') return true;                             // .
    return false;
}

// 是否是符号后续字符
bool SourceParser::isIdentifierNext(uint8_t ch)
{
    if (ch >= 0x30 && ch <= 0x39) return true;              // 0-9
    return isIdentifierStart(ch);
}

// 是否数字起始字符
bool SourceParser::isNumericStart(uint8_t ch)
{
    if (ch >= 0x30 && ch <= 0x39) return true;              // 0-9
    return false;
}

// 是否是数字后续字符
bool SourceParser::isNumericNext(uint8_t ch)
{
    if (ch >= 0x30 && ch <= 0x39) return true;              // 0-9
    if (isIdentifierNext(ch)) return true;
    return false;
}

// 测试指定tiken缓冲区是否全是10进制数字表示式（0-9加上小数点）
bool SourceParser::testTokenBuffIsAllDecimal(uint8_t * buffer, int size) 
{
    for (int i = 0; i < size; i++) {
        if (buffer[i] >= '0' && buffer[i] <= '9' || buffer[i] == '.') {
            continue;
        }
        break;
    }
    return true;
}

bool SourceParser::parse()
{
    const int LINE_MAX_SIZE = 256 * 1024;
    const int TOKEN_MAX_SIZE = 64 * 1024;
    uint8_t ch, ch_next, ch_next2;
    bool eof = false;
    shared_ptr<uint8_t> lineBuffPtr(new uint8_t[LINE_MAX_SIZE], [](uint8_t * p) { delete[] p; } );
    uint8_t * lineBuff = lineBuffPtr.get();
    shared_ptr<uint8_t> tokenBuffPtr(new uint8_t[TOKEN_MAX_SIZE], [](uint8_t * p) { delete[] p; } );
    uint8_t * tokenBuff = tokenBuffPtr.get();
    int linePtr = 0;
    int tokenPtr = 0;
    CurrTokenStartType tokenType = CurrTokenStartType::None;
    SourceLinePrePro curLine;
    
    auto add_curr_token = [&]() {
        if (tokenPtr <= 0)
            return;
        std::string preProToken((char*)tokenBuff, tokenPtr);
        curLine.tokens.push_back(preProToken);
        //
        tokenPtr = 0;
        tokenType = CurrTokenStartType::None;
    };
    
    while (true) {
        ch = loader->getByte(&eof);
        if (ch == 0 && eof) {
            break;
        }
        // 判断行是否超长，或者token是否超长   
        if (linePtr >= LINE_MAX_SIZE) {
        }
        if (tokenPtr >= TOKEN_MAX_SIZE) {
        }
        // 只要不是换行和回车,把字符添加到行缓冲区(/n==0x0A，/r==0x0D, 0x0C==换页)    
        if (ch != 0x0A && ch != 0x0D && ch != 0x0C && ch != 0)
            lineBuff[linePtr++] = ch;
        //
        ch_next = loader->preGet();
        ch_next2 = loader->preGetNext();

        switch (tokenType) {
            case CurrTokenStartType::None: {
                    // 标识符起始字符
                    if (ch == 0x20 || ch == '\t') {
                        // 空格不做任何处理
                    } else if (isIdentifierStart(ch)) {
                        tokenType = CurrTokenStartType::Identifier;
                        tokenBuff[tokenPtr++] = ch;
                        // 如果下一个字符不是标识符后继字符
                        if (!isIdentifierNext(ch_next)) {
                            add_curr_token();
                        }
                    } else if (isNumericStart(ch)) {
                        tokenType = CurrTokenStartType::Numeric;
                        tokenBuff[tokenPtr++] = ch;
                        // 如果下一个字符不是标识符后继字符
                        if (!isNumericNext(ch_next)) {
                            add_curr_token();
                        }
                    } else if (ch == '\n' || ch == '\r' || ch == 0x0C || ch == 0) {  // 换行、回车、换页
                        if (ch == '\r' && ch_next == '\n') {    // windows下是\r\n
                            ch = loader->getByte(&eof);
                        }
                        //
                        if (linePtr > 0) {
                            curLine.rawLine = std::string((char*)lineBuff, linePtr);
                        }
                        lines->push_back(curLine);
                        linePtr = 0;
                        curLine.clear();        // 当前行清除，相当于设置当前行为新行                           
                    } else if (ch == ';') {     // 分号开始是注释，直到换一行 
                        tokenType = CurrTokenStartType::Comment;
                    } else if (ch == '\'' || ch == '\"') {
                        tokenPtr = 0;
                        tokenBuff[tokenPtr++] = ch;
                        add_curr_token();
                        tokenPtr = 0;
                        if (ch == '\'')
                            tokenType = CurrTokenStartType::StringSingleQuote;
                        else
                            tokenType = CurrTokenStartType::StringDoubleQuote;
                    } else if (ch == '&') {
                        tokenPtr = 0;
                        tokenBuff[tokenPtr++] = ch;
                        if (ch_next == '&') {
                            ch = loader->getByte(&eof);
                            tokenBuff[tokenPtr++] = ch;
                            lineBuff[linePtr++] = ch;
                        }
                        add_curr_token();
                    } else if (ch == '|') {
                        tokenPtr = 0;
                        tokenBuff[tokenPtr++] = ch;
                        if (ch_next == '|') {
                            ch = loader->getByte(&eof);
                            tokenBuff[tokenPtr++] = ch;
                            lineBuff[linePtr++] = ch;
                        }
                        add_curr_token();
                    }
                    else if (ch == '=') {
                        tokenPtr = 0;
                        tokenBuff[tokenPtr++] = ch;
                        if (ch_next == '=') {
                            ch = loader->getByte(&eof);
                            tokenBuff[tokenPtr++] = ch;
                            lineBuff[linePtr++] = ch;
                        }
                        add_curr_token();
                    }
                    else if (ch == ',' || ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '+' || ch == '-' || ch == '*' || ch == '/' ||
                        ch == ':' || ch == '!' || ch == '#' || ch == '%' || ch == '<' || ch == '>' || ch == '\\' || ch == '^' || ch == '`' ||
                        ch == '{' || ch == '}' || ch == '~') {          // 单字符符号
                        tokenPtr = 0;
                        tokenBuff[tokenPtr++] = ch;
                        add_curr_token();
                    }
                    else {
                        // 当空格处理，忽略
                    }
                }
                break;
            case CurrTokenStartType::Identifier: {
                    // 标识符后继字符
                    if (isIdentifierNext(ch)) {
                        tokenBuff[tokenPtr++] = ch;
                    }
                    // 如果下一个字符不是标识符后继字符
                    if (! isIdentifierNext(ch_next)) {
                        add_curr_token();
                    }
                }
                break;
            case CurrTokenStartType::Numeric: {
                    if (ch == (uint8_t)('e') || ch == (uint8_t)('E')) {                               // 匹配例如 3.14E+48, 5.9687e-102 这样的浮点数
                        if (ch_next == (uint8_t)('+') || ch_next == (uint8_t)('-')) {
                            if (ch_next2 >= '0' && ch_next2 <= '9') {
                                if (testTokenBuffIsAllDecimal(tokenBuff, tokenPtr)) {
                                    tokenBuff[tokenPtr++] = ch;
                                    ch = loader->getByte(&eof);
                                    tokenBuff[tokenPtr++] = ch;
                                    lineBuff[linePtr++] = ch;
                                    continue;
                                }
                            }
                        }
                    }
                    // 标识符后继字符
                    if (isNumericNext(ch)) {
                        tokenBuff[tokenPtr++] = ch;
                    }
                    // 如果下一个字符不是标识符后继字符
                    if (!isNumericNext(ch_next)) {
                        add_curr_token();
                    }
                }
                break;
            case CurrTokenStartType::Comment: {
                    if (ch == '\n' || ch == '\r' || ch == 0x0C || ch == 0) {  // 注释以 换行、回车、换页结束 (这里处理的是分号紧跟回车的情况)
                        if (ch == '\r' && ch_next == '\n') {
                            ch = loader->getByte(&eof);
                        }
                        //
                        if (linePtr > 0) {
                            curLine.rawLine = std::string((char*)lineBuff, linePtr);
                        }
                        lines->push_back(curLine);
                        linePtr = 0;
                        curLine.clear();                                                    // 设置当前行为新行                        
                        tokenType = CurrTokenStartType::None;
                        continue;
                    }
                    //
                    if (ch_next == '\n' || ch_next == '\r' || ch_next == 0x0C || ch_next == 0) {
                        tokenType = CurrTokenStartType::None;
                    }
                }
                break;
            case CurrTokenStartType::StringSingleQuote:
            case CurrTokenStartType::StringDoubleQuote: {
                    if ((ch == '\'' && tokenType == CurrTokenStartType::StringSingleQuote) ||
                        (ch == '\"' && tokenType == CurrTokenStartType::StringDoubleQuote)) { // 这里处理的是引号跟紧引号的情况
                        tokenPtr = 0;
                        tokenBuff[tokenPtr++] = ch;
                        add_curr_token();
                        tokenPtr = 0;
                        tokenType = CurrTokenStartType::None;
                        continue;
                    }
                    else if (ch == '\n' || ch == '\r' || ch == 0x0C || ch == 0) {           // 这里判断引号之后紧跟回车的情况
                        if (ch == '\r' && ch_next == '\n') {
                            ch = loader->getByte(&eof);
                        }
                        if (linePtr > 0) {
                            curLine.rawLine = std::string((char*)lineBuff, linePtr);
                        }
                        lines->push_back(curLine);
                        linePtr = 0;
                        curLine.clear();    // 清除当前行，相当于起新行
                        tokenType = CurrTokenStartType::None;
                        continue;
                    }
                    else {
                        tokenBuff[tokenPtr++] = ch;
                    }
                    // 
                    if ((ch_next == '\'' && tokenType == CurrTokenStartType::StringSingleQuote) ||
                        (ch_next == '\"' && tokenType == CurrTokenStartType::StringDoubleQuote)) {   // 下一个字符是引号，表示结束字符串 
                        add_curr_token();
                        ch = loader->getByte(&eof);
                        tokenBuff[tokenPtr++] = ch;
                        lineBuff[linePtr++] = ch;
                        add_curr_token();
                    }
                    else if (ch_next == '\n' || ch_next == '\r' || ch_next == 0x0C || ch_next == 0) {
                        add_curr_token();
                    }
                }
                break;

        }
    }
    if (linePtr > 0) {
        curLine.rawLine = std::string((char*)lineBuff, linePtr);
    }
    lines->push_back(curLine);
    //
    //    // DEBUG OUTPUT
    FILE * fs = fopen("test.output.token.txt", "wt");
    for (auto it = lines->begin(); it != lines->end(); it++) {
        fprintf(fs, "---------------------------------------------------------------------------\n");
        fprintf(fs, "%s\n", it->rawLine.c_str());
        for (auto it2 = it->tokens.begin(); it2 != it->tokens.end(); it2++) {
            fprintf(fs, "%s\n", it2->c_str());
        }
    }
    fclose(fs);
    // DEBUG OUTPUT END

    return true; 
}



