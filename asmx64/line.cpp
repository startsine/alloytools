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



SourceParser::SourceParser(std::list<SourceLinePrePro> * lines, SourceLoader * loader)
{
    this->lines = lines;
    this->loader = loader;
}

bool SourceParser::parse()
{
    const int LINE_MAX_SIZE = 256 * 1024;
    const int TOKEN_MAX_SIZE = 64 * 1024;
    byte ch, ch_next, ch_next2;
    bool eof = false;
    byte[] lineBuff = new byte[LINE_MAX_SIZE];
    byte[] tokenBuff = new byte[TOKEN_MAX_SIZE];
    int linePtr = 0;
    int tokenPtr = 0;
    CuurTokenStartType tokenType = CuurTokenStartType.None;
    SourceLinePre curLine = new SourceLinePre();

    var add_curr_token = () => {
        if (tokenPtr <= 0)
            return;
        PreProToken preProToken = new PreProToken();
        if (tokenType != CuurTokenStartType.StringSingleQuote && tokenType != CuurTokenStartType.StringDoubleQuote) {
            preProToken.str = Encoding.UTF8.GetString(tokenBuff, 0, tokenPtr);
        } else {
            preProToken.isRawData = true;
            preProToken.rawBytes = new byte[tokenPtr];
            Array.Copy(tokenBuff, preProToken.rawBytes, tokenPtr);
        }
        if (curLine.tokens == null) {
            curLine.tokens = new List<PreProToken>();
        }
        curLine.tokens.Add(preProToken);
        //
        tokenPtr = 0;
        tokenType = CuurTokenStartType.None;
    };

    while (true) {
        ch = loader.GetByte(ref eof);
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
        ch_next = loader.PreGet();
        ch_next2 = loader.PreGetNext();
        //
        switch (tokenType) {
            case CuurTokenStartType.None: {
                    // 标识符起始字符
                    if (ch == 0x20 || ch == '\t') {
                        // 空格不做任何处理
                    } else if (isIdentifierStart(ch)) {
                        tokenType = CuurTokenStartType.Identifier;
                        tokenBuff[tokenPtr++] = ch;
                        // 如果下一个字符不是标识符后继字符
                        if (!isIdentifierNext(ch_next)) {
                            add_curr_token();
                        }
                    } else if (isNumericStart(ch)) {
                        tokenType |= CuurTokenStartType.Numeric;
                        tokenBuff[tokenPtr++] = ch;
                        // 如果下一个字符不是标识符后继字符
                        if (!isNumericNext(ch_next)) {
                            add_curr_token();
                        }
                    } else if (ch == '\n' || ch == '\r' || ch == 0x0C || ch == 0) {  // 换行、回车、换页
                        if (ch == '\r' && ch_next == '\n') {    // windows下是\r\n
                            ch = loader.GetByte(ref eof);
                        }
                        //
                        if (linePtr > 0) {
                            curLine.rawLine = new byte[linePtr];
                            Array.Copy(lineBuff, curLine.rawLine, linePtr);
                        }
                        lines.Add(curLine);
                        linePtr = 0;
                        curLine = new SourceLinePre();                              // 设置当前行为新行 
                    } else if (ch == ';') {     // 分号开始是注释，直到换一行 
                        tokenType = CuurTokenStartType.Comment;
                    } else if (ch == '\'' || ch == '\"') {
                        tokenPtr = 0;
                        tokenBuff[tokenPtr++] = ch;
                        add_curr_token();
                        tokenPtr = 0;
                        if (ch == '\'')
                            tokenType = CuurTokenStartType.StringSingleQuote;
                        else
                            tokenType = CuurTokenStartType.StringDoubleQuote;
                    } else if (ch == '&') {
                        tokenPtr = 0;
                        tokenBuff[tokenPtr++] = ch;
                        if (ch_next == '&') {
                            ch = loader.GetByte(ref eof);
                            tokenBuff[tokenPtr++] = ch;
                            lineBuff[linePtr++] = ch;
                        }
                        add_curr_token();
                    } else if (ch == '|') {
                        tokenPtr = 0;
                        tokenBuff[tokenPtr++] = ch;
                        if (ch_next == '|') {
                            ch = loader.GetByte(ref eof);
                            tokenBuff[tokenPtr++] = ch;
                            lineBuff[linePtr++] = ch;
                        }
                        add_curr_token();
                    }
                    else if (ch == '=') {
                        tokenPtr = 0;
                        tokenBuff[tokenPtr++] = ch;
                        if (ch_next == '=') {
                            ch = loader.GetByte(ref eof);
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
            case CuurTokenStartType.Identifier: {
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
            case CuurTokenStartType.Numeric: {
                    if (ch == (byte)('e') || ch == (byte)('E')) {                               // 匹配例如 3.14E+48, 5.9687e-102 这样的浮点数
                        if (ch_next == (byte)('+') || ch_next == (byte)('-')) {
                            if (ch_next2 >= '0' && ch_next2 <= '9') {
                                if (testTokenBuffIsAllDecimal(tokenBuff, tokenPtr)) {
                                    tokenBuff[tokenPtr++] = ch;
                                    ch = loader.GetByte(ref eof);
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
            case CuurTokenStartType.Comment: {
                    if (ch == '\n' || ch == '\r' || ch == 0x0C || ch == 0) {  // 注释以 换行、回车、换页结束 (这里处理的是分号紧跟回车的情况)
                        if (ch == '\r' && ch_next == '\n') {
                            ch = loader.GetByte(ref eof);
                        }
                        //
                        if (linePtr > 0) {
                            curLine.rawLine = new byte[linePtr];
                            Array.Copy(lineBuff, curLine.rawLine, linePtr);
                        }
                        lines.Add(curLine);
                        linePtr = 0;
                        curLine = new SourceLinePre();                              // 设置当前行为新行 
                        tokenType = CuurTokenStartType.None;
                        continue;
                    }
                    //
                    if (ch_next == '\n' || ch_next == '\r' || ch_next == 0x0C || ch_next == 0) {
                        tokenType = CuurTokenStartType.None;
                    }
                }
                break;
            case CuurTokenStartType.StringSingleQuote:
            case CuurTokenStartType.StringDoubleQuote: {
                    if ((ch == '\'' && tokenType == CuurTokenStartType.StringSingleQuote) ||
                        (ch == '\"' && tokenType == CuurTokenStartType.StringDoubleQuote)) { // 这里处理的是引号跟紧引号的情况
                        tokenPtr = 0;
                        tokenBuff[tokenPtr++] = ch;
                        add_curr_token();
                        tokenPtr = 0;
                        tokenType = CuurTokenStartType.None;
                        continue;
                    }
                    else if (ch == '\n' || ch == '\r' || ch == 0x0C || ch == 0) {           // 这里判断引号之后紧跟回车的情况
                        if (ch == '\r' && ch_next == '\n') {
                            ch = loader.GetByte(ref eof);
                        }
                        if (linePtr > 0) {
                            curLine.rawLine = new byte[linePtr];
                            Array.Copy(lineBuff, curLine.rawLine, linePtr);
                        }
                        lines.Add(curLine);
                        linePtr = 0;
                        curLine = new SourceLinePre();                              // 设置当前行为新行 
                        tokenType = CuurTokenStartType.None;
                        continue;
                    }
                    else {
                        tokenBuff[tokenPtr++] = ch;
                    }
                    // 
                    if ((ch_next == '\'' && tokenType == CuurTokenStartType.StringSingleQuote) ||
                        (ch_next == '\"' && tokenType == CuurTokenStartType.StringDoubleQuote)) {   // 下一个字符是引号，表示结束字符串 
                        add_curr_token();
                        ch = loader.GetByte(ref eof);
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
        /*
            Linux中\n表示回车+换行；
            Windows中\r\n表示回车+换行。
            Mac中\r表示回车+换行。

        Numeric,                        // 数字
StringSingleQuote,              // 单引号开始的字符串
StringDoubleQuote,              // 双引号开始的字符串
         */
    }
    if (linePtr > 0) {
        curLine.rawLine = new byte[linePtr];
        Array.Copy(lineBuff, curLine.rawLine, linePtr);
    }
    lines.Add(curLine);
    //
    // DEBUG OUTPUT
    var fs = new FileStream("test.output.token.txt", FileMode.Create);
    fs.Position = 0;
    using (StreamWriter writer = new StreamWriter(fs)) {
        for (ulong i = 0; i < lines.Count; i++) {
            SourceLinePre line = lines[i];
            writer.WriteLine("---------------------------------------------------------------------------");
            if (line.rawLine != null) {
                writer.WriteLine(Encoding.UTF8.GetString(line.rawLine));
            }
            if (line.tokens != null) {
                int k = 0;
                foreach (PreProToken token in line.tokens) {
                    if (!token.isRawData) {
                        writer.WriteLine($"  {k}: {token.str}");
                    }
                    else if (token.rawBytes != null) {
                        writer.WriteLine($"  {k}: {Encoding.UTF8.GetString(token.rawBytes)}");
                    }
                    k++;
                }
            }
        }
    }
    fs.Close();
    // DEBUG OUTPUT END
    return true; 
}



