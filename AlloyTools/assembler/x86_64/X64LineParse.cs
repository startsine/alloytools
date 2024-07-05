

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
            byte ch, ch_next, ch_next2;
            bool eof = false;
            while (true) {
                ch = loader.GetByte(ref eof);
                if (ch == 0 && eof) {
                    break;
                }
                ch_next = loader.PreGet();
                ch_next2 = loader.PreGetNext();
            }
            return true; 
        }
    }
}

