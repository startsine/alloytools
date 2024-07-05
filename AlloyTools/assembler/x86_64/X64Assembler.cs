


using AlloyTools.utils;
using System.Text;

namespace AlloyTools.Assembler.AMD64
{
    public class X64Assembler
    {
        private static List<SourceLinePre>? sourceLinesP = null;
        private static List<SourceLine>?    sourceLines = null;
        public static void Start(string[] args)
        {
            if (args.Length > 0) {
                Assembler(args[0]);
            }
        }

        private static void Assembler(string filepath)
        {
            var loader = new SourceLoader();
            byte[]? src = loader.LoadFile(filepath);
            if (src != null) {
                sourceLinesP = new List<SourceLinePre>();
                var parser = new SourceParser(sourceLinesP, src);
                parser.Parse();

                //Console.WriteLine("open ok");
                string s = Encoding.UTF8.GetString(src);
                //Console.WriteLine(s);

            }
        }
    }
}





