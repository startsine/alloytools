#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#ifdef _WIN32
#include "Windows.h"
#endif
#include "asmx64.h"

using namespace std;


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


SourceParser::SourceParser(std::list<SourceLinePrePro> * lines, SourceLoader * loader)
{
    this->lines = lines;
    this->loader = loader;
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
    
    ////auto add_curr_token = []() {
     ////   
    ////};
    
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
        // 只要不是换行和回车,把字符添加到行缓冲区(\n==0x0A，\r==0x0D, 0x0C==换页)
        if (ch != 0x0A && ch != 0x0D && ch != 0x0C && ch != 0)
            lineBuff[linePtr++] = ch;
        //
        ch_next = loader->preGet();
        ch_next2 = loader->preGetNext();
    }
    return true; 
}



