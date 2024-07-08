


using AlloyTools.utils;
using System.Collections;
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


            Hashtable ht = new Hashtable();
            var ss = ht.Count;
            //Console.WriteLine("open ok");
            //string s = Encoding.UTF8.GetString(src);
            //Console.WriteLine(s);
        }
    }
}





