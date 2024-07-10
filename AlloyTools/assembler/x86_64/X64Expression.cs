
namespace AlloyTools.Assembler.AMD64
{
    public class X64Expression
    {
        public List<X64Token> tokens = new List<X64Token>();

        // 把预处理的tokens转换为表达式列表,成功返回true，失败返回false
        public static bool ParseByPreProcessTokens(List<X64Expression> expressions, List<PreProToken> tokens, int  startIdx)
        {
            return true;
        }
    }
}
