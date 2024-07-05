

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
        private List<SourceLinePre> lines;
        private byte[] srcBuffer;
        //private byte[] tokenTemp;

        public SourceParser(List<SourceLinePre> lines, byte[] srcBuffer) 
        {
            this.lines = lines;
            this.srcBuffer = srcBuffer;
        }

        public bool Parse()
        {
            byte ch, ch_next;
            for (int i = 0;  i < srcBuffer.Length; i++) {
                ch = srcBuffer[i];
                ch_next = (i + 1) < srcBuffer.Length ? srcBuffer[i + 1] : (byte)0;
                var tt = lines;
            }
            return true; 
        }
    }
}

