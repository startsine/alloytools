
namespace AlloyTools.Assembler.AMD64
{
    public class X64Expression
    {
        public List<X64Token> tokens = new List<X64Token>();

        // 把预处理的tokens转换为表达式列表,成功返回true，失败返回false
        public static bool ParseByPreProcessTokens(List<X64Expression> expressions, List<PreProToken> tokens, int startIdx)
        {
            if (startIdx < expressions.Count) {
                X64Expression expr = new X64Expression();
                for (int i = startIdx; i < tokens.Count; i++) {
                    PreProToken token = tokens[i];
                    if (token.str == ",") {                     // end current expression
                        expressions.Add(expr);
                    }
                    else if (token.str == "\'" || token.str == "\"") {

                    }
                    else {
                        X64Token x64Token = new X64Token();
                        expr.tokens.Add(x64Token);
                    }
                }
            }
            return true;
        }
    }
}
