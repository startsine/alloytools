


using AlloyTools.utils;
using System.Collections;
using System.Diagnostics.Metrics;
using System.Text;

namespace AlloyTools.Assembler.AMD64
{
    public class X64Assembler
    {
        private static LargeList<SourceLinePre>? sourceLinesP = null;
        private static LargeList<SourceLine>?    sourceLines = null;
        public static void Start(string[] args)
        {
            if (args.Length > 0) {
                Assembler(args[0]);
            }
        }

        private static void Assembler(string filepath)
        {
            var loader = new SourceLoader();
            loader.LoadFile(filepath);

            sourceLinesP = new LargeList<SourceLinePre>();
            var parser = new SourceParser(sourceLinesP, loader);
            parser.Parse();
            //


            Hashtable ht = new Hashtable();
            var ss = ht.Count;
            //Console.WriteLine("open ok");
            //string s = Encoding.UTF8.GetString(src);
            //Console.WriteLine(s);
        }

        private void AssemblerPass1()
        {
            if (sourceLines == null)
                return;
            ulong lineCnt = 0;
            int tokenCnt = 0;
            int insnStartIdx = 0;                                       // 当前行中，指令的token的起始索引
            int operandStartIdx = 0;                                    // 当前行中，操作数的token的起始索引
            ulong lineTotal = sourceLinesP!.Count;
            for (lineCnt = 0; lineCnt < lineTotal; lineCnt++) {
                SourceLinePre curLine = sourceLinesP[lineCnt];
                if (curLine.tokens == null || curLine.tokens.Count == 0)
                    continue;
                operandStartIdx = 0;
                if (curLine.tokens.Count >= 2) {
                    var token0 = curLine.tokens[0];
                    var token1 = curLine.tokens[1];
                    if (token1 != null) {
                        if (token1.str == ":") {
                            insnStartIdx = 2;
                            operandStartIdx = 3;
                        }
                        else if (X64Token.isWithNamePseudoInstruction(token1.str)) {    // 是否允许name在前面的伪指令 
                            insnStartIdx = 1;
                            operandStartIdx = 2;
                        }
                    }
                }

                if (curLine.tokens != null) {

                    int tokenTotal = curLine.tokens.Count;
                    for (tokenCnt = 0; tokenCnt < tokenTotal; tokenCnt++) {

                    }
                }
            }
        }
    }
}





