#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "asmx64.h"

using namespace std;

bool X64Expression::parseByPreProcessTokens(std::vector<X64Expression> & expressions, std::vector<std::string> & tokens, int startIdx)
{
	return true;
}

bool X64Expression::calc()
{
    if (calculated)
        return true;
    if (tokens.size() == 0) {
        //// 这里缺处理
        return false;
    }
    if (tokens.size() == 1) {
        X64Token token = tokens[0];
        switch (token.tokenType) {
            case X64TokenType::Register: {
                operand = X64Operand(token.regValue);
                calculated = true;
                return true;
            }
            case X64TokenType::Numeric: {
                if ((uint64_t)token.flag & (uint64_t)(X64TokenFlag::NumericToBeUlong)) {    // 如果已经转换为ulong
                    operand = X64Operand(token.ulongValue);
                    calculated = true;
                    return true;
                }
            }
            break;
        }
        return false;
    }
    else {
        int addrStartIdx = findOperator(this->tokens, 0, X64AsmOperator::AddressStart);
        int addrEndIndex = findOperator(this->tokens, 0, X64AsmOperator::AddressEnd);
        if (addrStartIdx >= 0) {
            if (addrEndIndex >= 0 && addrEndIndex > addrStartIdx) {
                operand = calcAddressExpression(addrStartIdx, addrEndIndex);
            }
        }
    }
    //

    return true;
}

// 在token-list中查找操作符
int X64Expression::findOperator(const std::vector<X64Token> & xTokens, int startIndex, X64AsmOperator opt)
{
    for (int i = startIndex; i < (int) xTokens.size(); i++) {
        if (xTokens[i].tokenType == X64TokenType::Operator &&  xTokens[i].asmOperator == opt)
            return i;
    }
    return -1;
}

// 在token-list中查找任何操作符
int X64Expression::findAnyOperator(const std::vector<X64Token> & xTokens, int startIndex)
{
    for (int i = startIndex; i < (int) xTokens.size(); i++) {
        if (xTokens[i].tokenType == X64TokenType::Operator)
            return i;
    }
    return -1;
}

X64Operand X64Expression::calcAddressExpression(int start, int end)
{
    X64Operand calcRet;         // 计算结果

    if (start >= (int) tokens.size() || end >= (int) tokens.size())
        return calcRet;
    if (tokens[start].tokenType != X64TokenType::Operator || tokens[start].asmOperator != X64AsmOperator::AddressStart)
        return calcRet;
    if (tokens[end].tokenType != X64TokenType::Operator || tokens[end].asmOperator != X64AsmOperator::AddressEnd)
        return calcRet;
    if (start + 1 >= end)
        return calcRet;
    int start1 = start + 1;
    int end1 = end - 1;

    vector<X64Token> xTokens;
    for (int i = start1; i <= end1; i++) {
        xTokens.push_back(tokens[i]);
    }
    auto tempOperand = calcAddressExpressionInnerValue(xTokens);
    if (tempOperand.type != X64OperandType::Unknown) {
        if (tempOperand.type == X64OperandType::MemoryAddressInfo) {
            calcRet = memoryAddressInfoToRet(tempOperand);
        }
    }
    return calcRet;
}

// 计算寻址操作的一个最小单元操作
X64Operand X64Expression::calcAddressExpressionMinOperator(const X64Token * left, const X64Token * operatorToken, const X64Token * right)
{
    return X64Operand();
}

// 计算寻址表达式表达式内部值
X64Operand X64Expression::calcAddressExpressionInnerValue(std::vector<X64Token> & xTokens)
{
    X64Operand nullRet;         // 空结果

    if (xTokens.size() == 0)
        return nullRet;
    // 查找首个左括号操作符所在的索引
    int findStartIndex = 0;
    while (true) {
        int left = findOperator(xTokens, findStartIndex, X64AsmOperator::ParenthesesL);      // 查找左括号
        if (left >= 0) {
            int right = findOperator(xTokens, left + 1, X64AsmOperator::ParenthesesR);       // 从左括号后面开始找第一个右括号
            //
            if (right < 0) {
                //// 这里增加报错，找不到右括号与之匹配
                return nullRet;
            }
            int leftNext = findOperator(xTokens, left + 1, X64AsmOperator::ParenthesesL);    // 找下一个左括号
            if ((leftNext >= 0) && (leftNext < right)) {
                findStartIndex = leftNext;
                continue;
            }
            vector<X64Token> yTokens;
            for (int i = left + 1; i < right; i++) {
                yTokens.push_back(X64Token(xTokens[i]));
            }
            X64Operand tmpOperand = calcAddressExpressionInnerValue(yTokens);              // 递归计算
            if (tmpOperand.type != X64OperandType::Unknown) {
                X64Token oprdToken;
                oprdToken.tokenType = X64TokenType::TempOperand;
                oprdToken.tempOperand = tmpOperand;
                //
                vector<X64Token> zTokens;
                for (int k = 0; k < left; k++) {
                    zTokens.push_back(xTokens[k]);
                }
                zTokens.push_back(oprdToken);                                                     // 把括号中的表达式计算出值后，重组表达式再次计算
                for (int k = right + 1; k < (int) xTokens.size(); k++) {
                    zTokens.push_back(xTokens[k]);
                }
                return calcAddressExpressionInnerValue(zTokens);
            }
            else {
                //// 计算出错， 报错
                return nullRet;
            }
        }
        break;
    }
    // 将首个 +- 运算符设置为单目+-，将寄存器旁边的 +- 设置为寄存器 +-，将寄存器旁边的* 设置为寄存器* 
    if (xTokens[0].tokenType == X64TokenType::Operator) {
        if (xTokens[0].asmOperator == X64AsmOperator::Plus)
            xTokens[0].asmOperator = X64AsmOperator::PositiveSign;
        else if (xTokens[0].asmOperator == X64AsmOperator::Minus)
            xTokens[0].asmOperator = X64AsmOperator::NegativeSign;
    }
    for (int cur = 1; cur < (int) xTokens.size(); cur++) {
        if (xTokens[cur].tokenType == X64TokenType::Operator) {
            if (xTokens[cur].asmOperator == X64AsmOperator::Plus ||
                xTokens[cur].asmOperator == X64AsmOperator::Minus ||
                xTokens[cur].asmOperator == X64AsmOperator::Multiplication) {
                //
                X64AsmOperator willBe;
                if (xTokens[cur].asmOperator == X64AsmOperator::Plus)
                    willBe = X64AsmOperator::RegPlus;
                else if (xTokens[cur].asmOperator == X64AsmOperator::Minus)
                    willBe = X64AsmOperator::RegMinus;
                else
                    willBe = X64AsmOperator::RegMulti;
                int prev = cur - 1;
                int next = cur + 1;
                if (xTokens[prev].tokenType == X64TokenType::Register) {
                    xTokens[cur].asmOperator = willBe;
                    continue;
                }
                if (next < (int) xTokens.size() && xTokens[next].tokenType == X64TokenType::Register) {
                    xTokens[cur].asmOperator = willBe;
                    continue;
                }
            }
        }
    }
    //
    auto getOperatorLevel = [](X64AsmOperator opt) -> int {
        // 优先级排列, 越小，优先级越高
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
        case X64AsmOperator::Multiplication:
        case X64AsmOperator::Division:
            return 1;
        case X64AsmOperator::RegMulti:
            return 2;
        case X64AsmOperator::PositiveSign:
        case X64AsmOperator::NegativeSign:
            return 3;
        case X64AsmOperator::Plus:
        case X64AsmOperator::Minus:
            return 4;
        case X64AsmOperator::RegPlus:
        case X64AsmOperator::RegMinus:
            return 5;

        }
        return 10000;
    };
    //
    vector<X64Token> aTokens = xTokens;
    int startIdx = 0;
re_calculate:
    startIdx = 0;
    vector<int> operatorIndexes;        // 操作符的索引列表
    while (true) {
        int foundIdx = findAnyOperator(aTokens, startIdx);
        if (foundIdx >= 0) {
            if (operatorIndexes.size() > 0) {    //之前存在操作符的情况
                int lastIndex = operatorIndexes[operatorIndexes.size() - 1];
                int lastLevel = getOperatorLevel(aTokens[lastIndex].asmOperator);       // 上一个操作符的优先级
                int currLevel = getOperatorLevel(aTokens[foundIdx].asmOperator);        // 当前操作符的优先级
                if (currLevel < lastLevel) {        // level越小，优先级越高
                    operatorIndexes.push_back(foundIdx);
                    startIdx = foundIdx + 1;
                }
                else {
                    // 这里是当前操作符的优先级比上一个低的情况，这种情况应该是先处理上一个运算符的运算
                    X64Token * operatorToken = nullptr;
                    operatorToken = (lastIndex >= 0) && (lastIndex < (int)aTokens.size()) ? (&aTokens[lastIndex]) : nullptr;
                    if (operatorToken == nullptr) {
                        //// 报错
                        return nullRet;
                    }
                    //
                    X64Operand res;
                    int restructureIndexValue1;     // 重组索引1
                    int restructureIndexValue2;     // 重组索引2
                    if (operatorToken->asmOperator == X64AsmOperator::PositiveSign || operatorToken->asmOperator == X64AsmOperator::NegativeSign) {
                        X64Token * right = (lastIndex + 1 >= 0) && (lastIndex + 1 < (int)aTokens.size()) ? (& aTokens[lastIndex + 1]) : nullptr;
                        res = calcAddressExpressionMinOperator(nullptr, operatorToken, right);
                        restructureIndexValue1 = lastIndex;
                        restructureIndexValue2 = lastIndex + 2;
                    }
                    else {
                        X64Token * left = (lastIndex - 1 >= 0) && (lastIndex - 1 < (int) aTokens.size()) ? (& aTokens[lastIndex - 1]) : nullptr;
                        X64Token * right = (lastIndex + 1 >= 0) && (lastIndex + 1 < (int) aTokens.size()) ? (& aTokens[lastIndex + 1]) : nullptr;
                        res = calcAddressExpressionMinOperator(left, operatorToken, right);
                        restructureIndexValue1 = lastIndex - 1;
                        restructureIndexValue2 = lastIndex + 2;
                    }
                    if (res.type != X64OperandType::Unknown) {
                        vector <X64Token> restructureTokens;
                        for (int i = 0; i < restructureIndexValue1; i++) {
                            restructureTokens.push_back(aTokens[i]);
                        }
                        X64Token resToken;
                        resToken.tokenType = X64TokenType::TempOperand;
                        resToken.tempOperand = res;
                        restructureTokens.push_back(resToken);
                        for (int i = restructureIndexValue2; i < (int) aTokens.size(); i++) {
                            restructureTokens.push_back(aTokens[i]);
                        }
                        aTokens = restructureTokens;
                        goto re_calculate;
                    }
                    else {
                        //// 计算出错，报错？
                        return nullRet;
                    }
                }
            }
            else {  // 找到操作符，之前又没有存在操作符的情况 (operatorIndexes.Count==0)
                operatorIndexes.push_back(foundIdx);
                startIdx = foundIdx + 1;
            }
        }
        else {  // 再也找不到操作符的情况
            if (operatorIndexes.size() > 0) {
                // 存在操作符列表则从右往左开始计算（因为遇到更高优先级的操作符才会继续 Add 到 operatorIndexes）
                vector<X64Token> bTokens = aTokens;
                X64Operand result;
                for (int i = operatorIndexes.size() - 1; i >= 0; i--) {
                    int optIndex = operatorIndexes[i];
                    if ((int)bTokens.size() > optIndex + 2) {
                        //// 报错。操作符右边还有不止一个操作数
                        return nullRet;
                    }
                    X64Token * operatorToken = (optIndex >= 0) && (optIndex < (int)bTokens.size()) ? (& bTokens[optIndex]) : nullptr;
                    if (operatorToken == nullptr) {
                        //// 报错
                        return nullRet;
                    }
                    //
                    X64Operand res;
                    int restructureIndexValue1;     // 重组索引1
                    if (operatorToken->asmOperator == X64AsmOperator::PositiveSign || operatorToken->asmOperator == X64AsmOperator::NegativeSign) {
                        X64Token * right = (optIndex + 1 >= 0) && (optIndex + 1 < (int)bTokens.size()) ? (& bTokens[optIndex + 1]) : nullptr;
                        res = calcAddressExpressionMinOperator(nullptr, operatorToken, right);
                        restructureIndexValue1 = optIndex;
                    }
                    else {
                        X64Token * left = (optIndex - 1 >= 0) && (optIndex - 1 < (int)bTokens.size()) ? (& bTokens[optIndex - 1]) : nullptr;
                        X64Token * right = (optIndex + 1 >= 0) && (optIndex + 1 < (int)bTokens.size()) ? (& bTokens[optIndex + 1]) : nullptr;
                        res = calcAddressExpressionMinOperator(left, operatorToken, right);
                        restructureIndexValue1 = optIndex - 1;
                    }
                    result = res;
                    if (res.type != X64OperandType::Unknown) {
                        vector<X64Token> restructureTokens;
                        for (int j = 0; j < restructureIndexValue1; j++) {
                            restructureTokens.push_back(bTokens[j]);
                        }
                        X64Token resToken;
                        resToken.tokenType = X64TokenType::TempOperand;
                        resToken.tempOperand = res;
                        restructureTokens.push_back(resToken);
                        bTokens = restructureTokens;
                        continue;
                    }
                    else {
                        //// 计算出错，报错？
                        return nullRet;
                    }
                }
                return result;
            }
            else { // 没有操作符的情况
                if (aTokens.size() == 1) {
                    /// 这里加入单操作数转换为 操作数
                    X64Token token = aTokens[0];
                    if (token.tokenType == X64TokenType::TempOperand)
                        return token.tempOperand;
                    else if (token.tokenType == X64TokenType::Register) {
                        if (X64RegUtil::is32Or64BitReg(token.regValue)) {
                            MemoryAddressInfo memoryAddressInfo;
                            memoryAddressInfo.reg1 = token.regValue;
                            *((uint64_t*) &memoryAddressInfo.type) |= (uint64_t) MemoryAddressType::hasReg1;
                            X64Operand x64Operand(memoryAddressInfo);
                            return x64Operand;
                        }
                        else if (token.tokenType == X64TokenType::Numeric) {
                            X64Operand x64Operand(token.ulongValue);
                            return x64Operand;
                        }
                        else {
                            ////
                        }

                    }
                }
                else {
                    //// 报错
                    return nullRet;
                }
            }
        }
    }
}

// 寻址中间值转为内存寻址的结果值
X64Operand X64Expression::memoryAddressInfoToRet(const X64Operand & op)
{
    return X64Operand();
}



