using AlloyTools.Assembler.AMD64;

namespace asmx64
{
    internal class Program
    {
        static void Main(string[] args)
        {
            Console.OutputEncoding = System.Text.Encoding.UTF8;
            Console.InputEncoding = System.Text.Encoding.UTF8;
            Console.WriteLine("测试输出中文");
            X64Assembler.Start(args);
        }
    }
}
