
namespace AlloyTools.Assembler.AMD64
{
    public class X64Expression
    {
        public List<X64Token> tokens = new List<X64Token>();
        public bool calculated = false;                                             // 已经计算标志
        public X64Operand? operand = null;                                          // 结果操作数

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

        // 计算表达式结果，成功返回true，失败返回false
        public bool calc()
        {
            if (calculated)
                return true;
            if (tokens == null || tokens.Count == 0) {
                //// 这里缺处理
                return false;
            }
            if (tokens.Count == 1) {
                X64Token token = tokens[0];
                switch (token.tokenType) {
                    case X64TokenType.Register: {
                            operand = new X64Operand(token.regValue);
                            calculated = true;
                            return true;
                        }
                    case X64TokenType.Numeric: { 
                            if (token.flag.HasFlag(X64TokenFlag.NumericToBeUlong)) {    // 如果已经转换为ulong
                                operand = new X64Operand(token.ulongValue);
                                calculated = true;
                                return true;
                            }
                        }
                        break;
                }
                return false;
            }
            //

            return true;
        }

        // 计算寻址表达式，就是 [] 的部分, 成功返回 X64Operand，失败返回 null
        private X64Operand? calcAddressExpression(int start, int end)          // start和end是第一个token和最后一个token的索引，正常来说应该分别是 '[' 和 ']'
        {
            if (start >= tokens.Count || end >= tokens.Count)
                return null;
            if (tokens[start].tokenType != X64TokenType.Operator || tokens[start].asmOperator != X64AsmOperator.AddressStart)
                return null;
            if (tokens[end].tokenType != X64TokenType.Operator || tokens[end].asmOperator != X64AsmOperator.AddressEnd)
                return null;
            if (start + 1 >= end)
                return null;
            int start1 = start + 1;
            int end1 = end - 1;
            
            //this.tokens
            return null;
        }

        // 在token-list中查找操作符
        public static int FindOperator(List<X64Token> xTokens, int startIndex, X64AsmOperator opt)
        {
            for (int i = startIndex; i < xTokens.Count; i++) {
                if (xTokens[i].tokenType == X64TokenType.Operator &&  xTokens[i].asmOperator == opt) 
                    return i;
            }
            return -1;
        }

        // 计算寻址表达式表达式内部值
        private static X64Operand? calcAddressExpressionInnerValue(List<X64Token> xTokens)
        {
            // 优先级排列
            // 1. () 括号
            // 2. * / %
            // 3. 寄存器 *
            // 4. +, - 单目
            // 5. +, - 双目
            // 6. 寄存器 +,-
            // 7. << >> 左移右移 - 寻址内部不处理
            // 8. & 位与 - 寻址内部不处理
            // 9. ^ 位异或 - 寻址内部不处理
            // 10. | 位或 - 寻址内部不处理
            //
            // 查找首个操作符所在的索引

            if (!xTokens.Any()) 
                return null;
            //
            int findStartIndex = 0;
            while (true) {
                int left = FindOperator(xTokens, findStartIndex, X64AsmOperator.ParenthesesL);      // 查找左括号
                if (left >= 0) {
                    int right = FindOperator(xTokens, left + 1, X64AsmOperator.ParenthesesR);       // 从左括号后面开始找第一个右括号
                    //
                    if (right < 0) {
                        //// 这里增加报错，找不到右括号与之匹配
                        return null;
                    }
                    int leftNext = FindOperator(xTokens, left + 1, X64AsmOperator.ParenthesesL);    // 找下一个左括号
                    if ((leftNext >= 0) && (leftNext < right)) {
                        findStartIndex = leftNext;
                        continue;
                    }
                    List<X64Token> yTokens = new List<X64Token>();
                    for (int i = left + 1; i < right; i++) {
                        yTokens.Add(new X64Token(xTokens[i]));
                    }
                    X64Operand? tmpOperand = calcAddressExpressionInnerValue(yTokens);              // 递归计算
                    if (tmpOperand != null) {
                        X64Token oprdToken = new X64Token();
                        oprdToken.tokenType = X64TokenType.TempOperand;
                        oprdToken.tempOperand = tmpOperand;
                        //
                        List<X64Token> zTokens = new List<X64Token>();
                        for (int k = 0; k < left; k++) {
                            zTokens.Add(xTokens[k]);
                        }
                        zTokens.Add(oprdToken);                                                     // 把括号中的表达式计算出值后，重组表达式再次计算
                        for (int k = right; k < xTokens.Count; k++) {
                            zTokens.Add(xTokens[k]);
                        }
                        return calcAddressExpressionInnerValue(zTokens);
                    } 
                    else {
                        //// 报错
                        return null;
                    }
                    break;
                }
                break;
            }
            // 将首个 +- 运算符设置为单目+-，将寄存器旁边的 +- 设置为寄存器 +-，将寄存器旁边的* 设置为寄存器* 
            if (xTokens[0].tokenType == X64TokenType.Operator) {
                if (xTokens[0].asmOperator == X64AsmOperator.Plus)
                    xTokens[0].asmOperator = X64AsmOperator.PositiveSign;
                else if (xTokens[0].asmOperator == X64AsmOperator.Minus)
                    xTokens[0].asmOperator = X64AsmOperator.NegativeSign;
            }
            for (int cur = 1; cur < xTokens.Count; cur++) {
                if (xTokens[cur].tokenType == X64TokenType.Operator) {
                    if (xTokens[cur].asmOperator == X64AsmOperator.Plus ||
                        xTokens[cur].asmOperator == X64AsmOperator.Minus ||
                        xTokens[cur].asmOperator == X64AsmOperator.Multiplication) {
                        //
                        X64AsmOperator willBe;
                        if (xTokens[cur].asmOperator == X64AsmOperator.Plus)
                            willBe = X64AsmOperator.RegPlus;
                        else if (xTokens[cur].asmOperator == X64AsmOperator.Minus)
                            willBe = X64AsmOperator.RegMinus;
                        else
                            willBe = X64AsmOperator.RegMulti;
                        int prev = cur - 1;
                        int next = cur + 1;
                        if (xTokens[prev].tokenType == X64TokenType.Register) {
                            xTokens[cur].asmOperator = willBe;
                            continue;
                        }
                        if (next < xTokens.Count && xTokens[next].tokenType == X64TokenType.Register) {
                            xTokens[cur].asmOperator = willBe;
                            continue;
                        }
                    }
                }
            }
            //


            return null;
        }


    }
}
