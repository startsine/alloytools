
namespace AlloyTools.Assembler.AMD64
{
    public class X64Expression
    {
        public List<X64Token> tokens = new List<X64Token>();

        // 把预处理的tokens转换为表达式列表,成功返回true，失败返回false
        public static bool ParseByPreProcessTokens(List<X64Expression> expressions, List<PreProToken> tokens, int startIdx)
        {
            if (startIdx < tokens.Count) {
                X64Expression expr = new X64Expression();
                for (int i = startIdx; i < tokens.Count; i++) {
                    PreProToken token = tokens[i];
                    if (token.str == ",") {                     // end current expression
                        expressions.Add(expr);
                        expr = new X64Expression();
                    }
                    else if (token.str == "\'" || token.str == "\"") {
                        string currMark = token.str;
                        if (i + 1 >= tokens.Count) {
                            //报错，不完整的字符串表达式
                        }
                        if (tokens[i + 1].str == currMark) {
                            // 空字符串的情形
                            X64Token exprToken = new X64Token();
                            exprToken.tokenType = X64TokenType.String;
                            expr.tokens.Add(exprToken);
                            i++;
                            continue;
                        }
                        if (i + 2 >= tokens.Count) {
                            //报错，不完整的字符串表达式
                        }
                        if (tokens[i + 2].str == currMark) {
                            // 真实字符串的情形
                            X64Token exprToken = new X64Token();
                            exprToken.tokenType = X64TokenType.String;
                            exprToken.rawBytes = tokens[i + 1].rawBytes;
                            expr.tokens.Add(exprToken);
                            i += 2;
                            continue;
                        }
                        //这里加入报错，不完整的字符串表达式
                    }
                    else {
                        X64Token exprToken = new X64Token();
                        if (X64Token.isNumericStr(token.str)) {
                            exprToken.tokenType = X64TokenType.Numeric;
                            exprToken.str = token.str;
                            exprToken.tryParseToUlongValue();
                        }
                        else if (X64Token.isRegister(token.str)) {
                            exprToken.tokenType = X64TokenType.Register;
                            exprToken.str = token.str;
                            exprToken.regValue = X64Token.getRegisterValue(token.str);
                        }
                        else if (X64Token.isOperator(token.str)) {
                            exprToken.tokenType = X64TokenType.Operator;
                            exprToken.str = token.str;
                            exprToken.asmOperator = X64Token.getOperatorValue(token.str);
                        }
                        else {  // 标识符的情况
                            exprToken.tokenType = X64TokenType.Symbol;
                            exprToken.str = token.str;
                            // 添加到符号表
                            exprToken.symbolIndex = 0;                      // 这里要改为符号表索引
                        }
                        expr.tokens.Add(exprToken);
                    }
                }
                expressions.Add(expr);
            }
            return true;
        }
    }
}
