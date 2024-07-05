

using AlloyTools.utils;


namespace AlloyTools.Assembler.AMD64
{
    // 未经过预处理的行
    public class SourceLinePre
    {
        public byte[]? rawContent;                   // 源码行的原始内容
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
        StringSingleQuote,              // 单引号开始的字符串
        StringDoubleQuote,              // 双引号开始的字符串
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

            while (true) {
                ch = loader.GetByte(ref eof);
                if (ch == 0 && eof) {
                    break;
                }
                ch_next = loader.PreGet();
                ch_next2 = loader.PreGetNext();
                // 只要不是换行和回车,把字符添加到行缓冲区(\n==0x0A，\r==0x0D)
                if (ch != 0x0A && ch != 0x0D)
                    lineBuff[linePtr++] = ch;

                /*
                    Linux中\n表示回车+换行；
                    Windows中\r\n表示回车+换行。
                    Mac中\r表示回车+换行。
                 */
            }
            return true; 
        }

        // 是否是符号首字符
        private bool isIdentifierStart(byte ch)
        {
            if (ch >= 128) return true;                             // 大于128的 Unicode 字符
            if (ch >= 0x41 && ch <= 0x5A) return true;              // A-Z
            if (ch >= 0x61 && ch <= 0x7A) return true;              // a-z
            if (ch == 0x5F) return true;                            // _
            if (ch == 0x24) return true;                            // $
            if (ch == 0x3F) return true;                            // ?
            if (ch == 0x40) return true;                            // @
            return false;
        }

        // 是否是符号后续字符
        private bool isIdentifierNext(byte ch)
        {
            if (ch >= 0x30 && ch <= 0x39) return true;              // 0-9
            return isIdentifierStart(ch);
        }

        private bool isNumericStart(byte ch)
        {
            if (ch >= 0x30 && ch <= 0x39) return true;              // 0-9
            return false;
        }
    }
}

