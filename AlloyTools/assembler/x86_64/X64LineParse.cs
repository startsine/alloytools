

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
        public byte[]? rawContent;                   // 源码行的原始内容
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
                if (ch != 0x0A && ch != 0x0D && ch != 0x0C)
                    lineBuff[linePtr++] = ch;
                //
                ch_next = loader.PreGet();
                ch_next2 = loader.PreGetNext();
                //
                switch (tokenType) {
                    case CuurTokenStartType.None: {
                            // 标识符起始字符
                            if (isIdentifierStart(ch)) {
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
                                if (curLine != null) {
                                    curLine.rawLine = new byte[linePtr];
                                    Array.Copy(lineBuff, curLine.rawLine, linePtr);
                                    lines.Add(curLine);
                                    linePtr = 0;
                                }
                                curLine = new SourceLinePre();                              // 设置当前行为新行 
                            } else if (ch == ';') {     // 分号开始是注释，直到换一行 
                                tokenType = CuurTokenStartType.Comment;
                            }
                            else {
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
                            if (ch == (byte)('e') || ch == (byte)('E')) {
                                if (ch_next == (byte)('+') || ch_next == (byte)('-')) {

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
                            if (ch == '\n' || ch == '\r' || ch == 0x0C || ch == 0) {  // 注释以 换行、回车、换页结束
                                if (ch == '\r' && ch_next == '\n') {
                                    ch = loader.GetByte(ref eof);
                                }
                                //
                                if (curLine != null) {
                                    curLine.rawLine = new byte[linePtr];
                                    Array.Copy(lineBuff, curLine.rawLine, linePtr);
                                    lines.Add(curLine);
                                    linePtr = 0;
                                }
                                curLine = new SourceLinePre();                              // 设置当前行为新行 
                                tokenType = CuurTokenStartType.None;
                            }
                            else {
                                //tokenBuff[tokenPtr++] = ch;
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
            //
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
    }
}

