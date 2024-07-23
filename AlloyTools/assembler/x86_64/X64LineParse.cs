

using AlloyTools.utils;
using System.IO;
using System.IO.Pipes;
using System.Text;
using System.Xml.Linq;


namespace AlloyTools.Assembler.AMD64
{
    // 未经过预处理的行
    public class SourceLinePre
    {
        public byte[]? rawLine = null;              // 源码行的原始内容
        public List<PreProToken>? tokens = null;    // token列表
    }

    // 经过预处理之后的行
    public class SourceLine
    {
        public byte[]? rawContent;                          // 源码行的原始内容
        public List<X64Expression>? expressions = null;     // 当前行的表达式列表(指令后面的表达式)
        public bool hasLabel = false;                       // 该行是否拥有标签符号
        public string labelStr = "";                        // 标签符号字符串
        //public bool hasInsn = false;                        // 该行是否拥有指令(包括指令与伪指令)
        //public bool isCpuInsn = false;                      // 如果该行拥有指令，该指令是否CPU真实指令，如果为false则表示是伪指令
        public bool hasInsnPrefix = false;                  // 是否拥有指令前缀
        public CpuInsnPrefixID prefixs = CpuInsnPrefixID.None;  // 指令前缀列表
        //public CpuInsnID cpuInsnID = CpuInsnID.None;            // CPU指令ID (当 hasInsn==true 及 isCpuInsn == true 有效)
        //public PseudoInsnID pseudoInsnID = PseudoInsnID.None;   // 伪指令ID (当 hasInsn==true 及 isCpuInsn == false 有效)
        //public uint bytesize = 0;                           // 该行产生的机器代码的字节大小（如果是虚拟指令或者Jcc指令，这里先存放它的可能最长的大小,后面再扫描修正）
    }

    enum CuurTokenStartType                         // 当前词法分析的Token起始类型
    {
        None,
        Identifier,                     // 标识符开始 (非数字开头，允许字母、数字、$、?、@、_)
        Numeric,                        // 数字
        StringSingleQuote,              // 单引号开始的字符串
        StringDoubleQuote,              // 双引号开始的字符串
        Comment,                        // 注释
    }

    public class SourceParser
    {
        private LargeList<SourceLinePre> lines;
        private SourceLoader loader;
        //private byte[] tokenTemp;

        public SourceParser(LargeList<SourceLinePre> lines, SourceLoader loader) 
        {
            this.lines = lines;
            this.loader = loader;
        }

        public bool Parse()
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

        // 是否是符号首字符
        private bool isIdentifierStart(byte ch)
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
        private bool isIdentifierNext(byte ch)
        {
            if (ch >= 0x30 && ch <= 0x39) return true;              // 0-9
            return isIdentifierStart(ch);
        }

        // 是否数字起始字符
        private bool isNumericStart(byte ch)
        {
            if (ch >= 0x30 && ch <= 0x39) return true;              // 0-9
            return false;
        }

        // 是否是数字后续字符
        private bool isNumericNext(byte ch)
        {
            if (ch >= 0x30 && ch <= 0x39) return true;              // 0-9
            if (isIdentifierNext(ch)) return true;
            return false;
        }

        private bool testTokenBuffIsAllDecimal(byte[] buffer, int size) {   // 测试指定tiken缓冲区是否全是10进制数字表示式（0-9加上小数点）
            for (int i = 0; i < size; i++) {
                if (buffer[i] >= '0' && buffer[i] <= '9' || buffer[i] == '.') {
                    continue;
                }
                break;
            }
            return true;
        }
    }
}

