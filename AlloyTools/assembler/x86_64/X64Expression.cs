
using System.Collections.Generic;

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

        // 在token-list中查找任何操作符
        public static int FindAnyOperator(List<X64Token> xTokens, int startIndex)
        {
            for (int i = startIndex; i < xTokens.Count; i++) {
                if (xTokens[i].tokenType == X64TokenType.Operator)
                    return i;
            }
            return -1;
        }

        // 计算寻址表达式表达式内部值
        private static X64Operand? calcAddressExpressionInnerValue(List<X64Token> xTokens)
        {
            if (!xTokens.Any()) 
                return null;
            // 查找首个左括号操作符所在的索引
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
            var getOperatorLevel = (X64AsmOperator opt) => {
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
                switch (opt) {
                    case X64AsmOperator.Multiplication:
                    case X64AsmOperator.Division:
                        return 1;
                    case X64AsmOperator.RegMulti:
                        return 2;
                    case X64AsmOperator.PositiveSign:
                    case X64AsmOperator.NegativeSign:
                        return 3;
                    case X64AsmOperator.Plus:
                    case X64AsmOperator.Minus:
                        return 4;
                    case X64AsmOperator.RegPlus:
                    case X64AsmOperator.RegMinus:
                        return 5;

                }
                return 10000;
            };
            //
            List<X64Token> aTokens = xTokens;
            int startIdx = 0;
re_calculate:
            startIdx = 0;
            while (true) {
                List<int> operatorIndexes = new List<int>();        // 操作符的索引列表
                int foundIdx = FindAnyOperator(aTokens, startIdx);
                if (foundIdx >= 0) {
                    if (operatorIndexes.Count > 0) {    //之前存在操作符的情况
                        int lastIndex = operatorIndexes[operatorIndexes.Count - 1];
                        int lastLevel = getOperatorLevel(aTokens[lastIndex].asmOperator);       // 上一个操作符的优先级
                        int currLevel = getOperatorLevel(aTokens[foundIdx].asmOperator);        // 当前操作符的优先级
                        if (currLevel < lastLevel) {        // level越小，优先级越高
                            operatorIndexes.Add(foundIdx);
                            startIdx = foundIdx + 1;
                        }
                        else {
                            // 这里是当前操作符的优先级比上一个低的情况，这种情况应该是先处理上一个运算符的运算
                            X64Token? operatorToken = (lastIndex >= 0) && (lastIndex < aTokens.Count) ? aTokens[lastIndex] : null;
                            if (operatorToken == null) {
                                //// 报错
                                return null;
                            }
                            //
                            X64Operand? res = null;
                            int restructureIndexValue1;     // 重组索引1
                            int restructureIndexValue2;     // 重组索引2
                            if (operatorToken.asmOperator == X64AsmOperator.PositiveSign || operatorToken.asmOperator == X64AsmOperator.NegativeSign) {
                                X64Token? right = (lastIndex + 1 >= 0) && (lastIndex + 1 < aTokens.Count) ? aTokens[lastIndex + 1] : null;
                                res = calcAddressExpressionMinOperator(null, operatorToken, right);
                                restructureIndexValue1 = lastIndex;
                                restructureIndexValue2 = lastIndex + 2;
                            }
                            else {
                                X64Token? left = (lastIndex - 1 >= 0) && (lastIndex - 1 < aTokens.Count) ? aTokens[lastIndex - 1] : null;
                                X64Token? right = (lastIndex + 1 >= 0) && (lastIndex + 1 < aTokens.Count) ? aTokens[lastIndex + 1] : null;
                                res = calcAddressExpressionMinOperator(left, operatorToken, right);
                                restructureIndexValue1 = lastIndex - 1;
                                restructureIndexValue2 = lastIndex + 2;
                            }
                            if (res != null) {
                                List <X64Token> restructureTokens = new List < X64Token >();
                                for (int i = 0; i < restructureIndexValue1; i++) {
                                    restructureTokens.Add(aTokens[i]);
                                }
                                X64Token resToken = new X64Token();
                                resToken.tokenType = X64TokenType.TempOperand;
                                resToken.tempOperand = res;
                                restructureTokens.Add(resToken);
                                for (int i = restructureIndexValue2; i < aTokens.Count; i++) {
                                    restructureTokens.Add(aTokens[i]);
                                }
                                aTokens = restructureTokens;
                                goto re_calculate;
                            }
                            else {
                                //// 计算出错，报错？
                                return null;
                            }
                        }
                    }
                    else {  // 找到操作符，之前又没有存在操作符的情况 (operatorIndexes.Count==0)
                        operatorIndexes.Add(foundIdx);
                        startIdx = foundIdx + 1;
                    }
                }
                else {  // 再也找不到操作符的情况
                    if (operatorIndexes.Count > 0) {
                        // 存在操作符列表则从右往左开始计算（因为遇到更高优先级的操作符才会继续 Add 到 operatorIndexes）
                        List<X64Token> bTokens = aTokens;
                        X64Operand? result = null;
                        for (int i = operatorIndexes.Count - 1; i >= 0; i--) {
                            int optIndex = operatorIndexes[i];
                            if (bTokens.Count > optIndex + 2) {
                                //// 报错。操作符右边还有不止一个操作数
                                return null;
                            }
                            X64Token? operatorToken = (optIndex >= 0) && (optIndex < bTokens.Count) ? bTokens[optIndex] : null;
                            if (operatorToken == null) {
                                //// 报错
                                return null;
                            }
                            //
                            X64Operand? res = null;
                            int restructureIndexValue1;     // 重组索引1
                            if (operatorToken.asmOperator == X64AsmOperator.PositiveSign || operatorToken.asmOperator == X64AsmOperator.NegativeSign) {
                                X64Token? right = (optIndex + 1 >= 0) && (optIndex + 1 < bTokens.Count) ? bTokens[optIndex + 1] : null;
                                res = calcAddressExpressionMinOperator(null, operatorToken, right);
                                restructureIndexValue1 = optIndex;
                            }
                            else {
                                X64Token? left = (optIndex - 1 >= 0) && (optIndex - 1 < bTokens.Count) ? bTokens[optIndex - 1] : null;
                                X64Token? right = (optIndex + 1 >= 0) && (optIndex + 1 < bTokens.Count) ? bTokens[optIndex + 1] : null;
                                res = calcAddressExpressionMinOperator(left, operatorToken, right);
                                restructureIndexValue1 = optIndex - 1;
                            }
                            result = res;
                            if (res != null) {
                                List<X64Token> restructureTokens = new List<X64Token>();
                                for (int j = 0; j < restructureIndexValue1; j++) {
                                    restructureTokens.Add(bTokens[i]);
                                }
                                X64Token resToken = new X64Token();
                                resToken.tokenType = X64TokenType.TempOperand;
                                resToken.tempOperand = res;
                                restructureTokens.Add(resToken);
                                bTokens = restructureTokens;
                                continue;
                            }
                            else {
                                //// 计算出错，报错？
                                return null;
                            }
                        }
                        return result;
                    }
                    else { // 没有操作符的情况
                        if (aTokens.Count == 1) {
                            /// 这里加入单操作数转换为 操作数
                            X64Token token = aTokens[0];
                            if (token.tokenType == X64TokenType.TempOperand) 
                                return token.tempOperand;
                            else if (token.tokenType == X64TokenType.Register) {
                                if (X64RegUtil.Is32Or64BitReg(token.regValue)) {
                                    MemoryAddressInfo memoryAddressInfo = new MemoryAddressInfo();
                                    memoryAddressInfo.reg1 = token.regValue;
                                    memoryAddressInfo.type |= MemoryAddressType.hasReg1;
                                    X64Operand x64Operand = new X64Operand(memoryAddressInfo);
                                    return x64Operand;
                                }
                                else if (token.tokenType == X64TokenType.Numeric) {
                                    X64Operand x64Operand = new X64Operand(token.ulongValue);
                                    return x64Operand;
                                }
                                else {
                                    ////
                                }
                                
                            }
                        }
                        else {
                            //// 报错
                            return null;
                        }
                    }
                }
            }
        }

        // 计算寻址操作的一个最小单元操作
        private static X64Operand? calcAddressExpressionMinOperator(X64Token? left, X64Token? operatorToken, X64Token? right)
        {
            if (operatorToken == null)
                return null;
            if (operatorToken.tokenType != X64TokenType.Operator)
                return null;
            if (operatorToken.asmOperator == X64AsmOperator.PositiveSign || operatorToken.asmOperator == X64AsmOperator.NegativeSign) {
                if (right == null)
                    return null;
            }
            else {
                if (left == null || right == null)
                    return null;
            }
            return null;
        }

    }
}
