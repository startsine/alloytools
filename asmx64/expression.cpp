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

MemoryAddressInfo X64Expression::toTempMemoryAddressInfo(const X64Token * token)
{
    MemoryAddressInfo info;
    switch (token->tokenType) {
    case X64TokenType::Numeric:
        u64SetFlag((uint64_t*) &info.type, (uint64_t) MemoryAddressType::hasDisp);
        info.disp32 = token->ulongValue;
        break;
    case X64TokenType::Register:
        u64SetFlag((uint64_t*)&info.type, (uint64_t)MemoryAddressType::hasReg1);
        info.reg1 = token->regValue;
        break;
    case X64TokenType::TempOperand:
        if (token->tempOperand.type != X64OperandType::Unknown) {
            if (token->tempOperand.addressInfo.type != MemoryAddressType::None)
                info = token->tempOperand.addressInfo;
        }
        break;
    case X64TokenType::Symbol:
        u64SetFlag((uint64_t*)&info.type, (uint64_t)MemoryAddressType::hasSymbol);
        info.symName = token->str;
        break;
    }
    return info;
}

int X64Expression::getRegCountFromMemoryAddressInfo(MemoryAddressInfo & info)
{
    int count = 0;
    if (u64HasFlag((uint64_t)info.type, (uint64_t)MemoryAddressType::hasReg1))
        count++;
    if (u64HasFlag((uint64_t)info.type, (uint64_t)MemoryAddressType::hasReg2))
        count++;
    return count;
}

// 计算寻址操作的一个最小单元操作
X64Operand X64Expression::calcAddressExpressionMinOperator(const X64Token * left, const X64Token * operatorToken, const X64Token * right)
{
    X64Operand ret;         // 默认为空结果

    if (operatorToken == nullptr)
        return ret;
    if (operatorToken->tokenType != X64TokenType::Operator)
        return ret;
    if (operatorToken->asmOperator == X64AsmOperator::PositiveSign || operatorToken->asmOperator == X64AsmOperator::NegativeSign) {
        if (right == nullptr)
            return ret;
    }
    else {
        if (left == nullptr || right == nullptr)
            return ret;
        MemoryAddressInfo op1 = toTempMemoryAddressInfo(left);
        MemoryAddressInfo op2 = toTempMemoryAddressInfo(right);
        switch (operatorToken->asmOperator) {
        case X64AsmOperator::Plus:
        case X64AsmOperator::RegPlus: {
            if (getRegCountFromMemoryAddressInfo(op1) + getRegCountFromMemoryAddressInfo(op2) >= 3) {
                //// 寻址中不循序超过3个寄存器,报错
                return ret;
            }
            if (u64HasFlag((uint64_t)op1.type, (uint64_t)MemoryAddressType::hasExplicitScale) 
                    && u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasExplicitScale)) {
                //// 寻址中不能有2个变址索引,报错
                return ret;
            }
            if (u64HasFlag((uint64_t)op1.type, (uint64_t)MemoryAddressType::hasReg1)) {
                if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasReg1)) {
                    u64SetFlag((uint64_t*) &op1.type, (uint64_t) MemoryAddressType::hasReg2);
                    op1.reg2 = op2.reg1;
                }
                else if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasReg2)) {
                    u64SetFlag((uint64_t*)&op1.type, (uint64_t)MemoryAddressType::hasReg2);
                    op1.reg2 = op2.reg2;
                }
            }
            else {
                if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasReg1)) {
                    u64SetFlag((uint64_t*)&op1.type, (uint64_t)MemoryAddressType::hasReg1);
                    op1.reg1 = op2.reg1;
                }
                else if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasReg2)) {
                    u64SetFlag((uint64_t*)&op1.type, (uint64_t)MemoryAddressType::hasReg2);
                    op1.reg2 = op2.reg2;
                }
            }
            if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasExplicitScale)) {
                u64SetFlag((uint64_t*)&op1.type, (uint64_t)MemoryAddressType::hasExplicitScale);
                op1.scale = op2.scale;
            }
            if (u64HasFlag((uint64_t)op1.type, (uint64_t)MemoryAddressType::hasDisp)) {
                if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasDisp)) {
                    op1.disp32 += op2.disp32;
                }
            }
            else {
                if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasDisp)) {
                    u64SetFlag((uint64_t*)&op1.type, (uint64_t)MemoryAddressType::hasDisp);
                    op1.disp32 = op2.disp32;
                }
            }
            if (u64HasFlag((uint64_t)op1.type, (uint64_t)MemoryAddressType::hasSymbol)) {
                if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasSymbol)) {
                    //// 报错，不允许两个符号来重定位
                }
            }
            else {
                if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasSymbol)) {
                    u64SetFlag((uint64_t*)&op1.type, (uint64_t)MemoryAddressType::hasSymbol);
                    op1.symName = op2.symName;
                }
            }
            ret = X64Operand(op1);
            return ret;
        }
        case X64AsmOperator::Minus:
        case X64AsmOperator::RegMinus: {
            if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasReg1)) {
                //// 被减数不能带寄存器
                return ret;
            }
            if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasReg2)) {
                //// 被减数不能带寄存器
                return ret;
            }
            if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasExplicitScale)) {
                //// 被减数不能带因子
                return ret;
            }
            if (u64HasFlag((uint64_t)op1.type, (uint64_t)MemoryAddressType::hasDisp)) {
                if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasDisp)) {
                    op1.disp32 -= op2.disp32;
                }
            }
            else {
                if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasDisp)) {
                    op1.disp32 = (0 - op2.disp32);
                }
            }
            ret = X64Operand(op1);
            return ret;
        }
        case X64AsmOperator::RegMulti: {
            if (getRegCountFromMemoryAddressInfo(op1) + getRegCountFromMemoryAddressInfo(op2) >= 2) {
                //// 报错,两边操作数总寄存器个数大于或者等于2
                return ret;
            }
            if (u64HasFlag((uint64_t)op1.type, (uint64_t)MemoryAddressType::hasExplicitScale)) {
                //// 不能带因子
                return ret;
            }
            if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasExplicitScale)) {
                //// 不能带因子
                return ret;
            }
            if (u64HasFlag((uint64_t)op1.type, (uint64_t)MemoryAddressType::hasReg2)) {
                //// 不能带reg2
                return ret;
            }
            if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasReg2)) {
                //// 不能带reg2
                return ret;
            }
            if (u64HasFlag((uint64_t)op1.type, (uint64_t)MemoryAddressType::hasReg1)
                && u64HasFlag((uint64_t)op1.type, (uint64_t)MemoryAddressType::hasDisp) ) {
                //// 不能带寄存器又带Disp
                return ret;
            }
            if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasReg1)
                && u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasDisp) ) {
                //// 不能带寄存器又带Disp
                return ret;
            }
            // 如果寄存器在右侧，则互换
            if (u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasReg1)) {
                MemoryAddressInfo tempForSwap;
                tempForSwap = op2;
                op2 = op1;
                op1 = tempForSwap;
            }
            //
            if (!u64HasFlag((uint64_t)op2.type, (uint64_t)MemoryAddressType::hasDisp)) {
                //// 报错,op2必须Disp
                return ret;
            }
            if (op2.disp32 != 1 && op2.disp32 != 2 && op2.disp32 != 4 && op2.disp32 != 8) {
                //// 报错,op2 disp 不是1/2/4/8
                return ret;
            }
            u64SetFlag((uint64_t*)&op1.type, (uint64_t)MemoryAddressType::hasExplicitScale);
            op1.scale = (uint8_t)op2.disp32;
            u64SetFlag((uint64_t*)&op1.type, (uint64_t)MemoryAddressType::hasReg2);
            op1.reg2 = op1.reg1;
            enum64ClearFlag((uint64_t*)&op1.type, (uint64_t)MemoryAddressType::hasReg1);
            op1.reg1 = X64RegValue::None;
            ret = X64Operand(op1);
            return ret;
        }
        case X64AsmOperator::Multiplication: {
            if (op1.type != MemoryAddressType::hasDisp) {
                //// 报错,乘法操作只能纯数字
                return ret;
            }
            if (op2.type != MemoryAddressType::hasDisp) {
                //// 报错,乘法操作只能纯数字
                return ret;
            }
            u64SetFlag((uint64_t*)&op1.type, (uint64_t)MemoryAddressType::hasDisp);
            op1.disp32 = (uint64_t)(op1.disp32 * op2.disp32);
            ret = X64Operand(op1);
            return ret;
        }
        case X64AsmOperator::Division: {
            if (op1.type != MemoryAddressType::hasDisp) {
                //// 报错,除法操作只能纯数字
                return ret;
            }
            if (op2.type != MemoryAddressType::hasDisp) {
                //// 报错,除法操作只能纯数字
                return ret;
            }
            if (op2.disp32 == 0) {
                //// 报错,除法不能除以0
                return ret;
            }
            u64SetFlag((uint64_t*)&op1.type, (uint64_t)MemoryAddressType::hasDisp);
            op1.disp32 = op1.disp32 / op2.disp32;
            ret = X64Operand(op1);
            return ret;
        }
        }
    }
    return ret;
}

// 寻址中间值转为内存寻址的结果值
X64Operand X64Expression::memoryAddressInfoToRet(const X64Operand & op)
{
    X64Operand target;         // 默认为空结果
    MemoryAddressResult res;
    
    if (op.type == X64OperandType::MemoryAddressInfo) {
        MemoryAddressInfo info = op.addressInfo;            //  复制，中途可能需要改写
        bool use64 = false;
        uint8_t scaleValue = 0;
        uint8_t indexValue = 0;
        uint8_t baseValue = 0;
        //
        if (u64HasFlag((uint64_t)info.type, (uint64_t)MemoryAddressType::hasReg1)
            && u64HasFlag((uint64_t)info.type, (uint64_t)MemoryAddressType::hasReg2)) {
            // 下面处理有 reg1 和 reg2 的情况
            if (X64RegUtil::is64BitReg(info.reg1) && X64RegUtil::is64BitReg(info.reg2)) {         //  同时64位基址寄存器和变址寄存器
                use64 = true;
            }
            else if (X64RegUtil::is32BitReg(info.reg1) && X64RegUtil::is32BitReg(info.reg2)) {    //  同时32位基址寄存器和变址寄存器
                use64 = false;
            }
            else {
                //// 报错退出
                return;
            }
            //
            if (u64HasFlag((uint64_t)info.type, (uint64_t)MemoryAddressType::hasExplicitScale) && (info.reg2 == X64RegValue::RSP || info.reg2 == X64RegValue::ESP)) {
                //// 这里报错，RSP/ESP不允许做变址寄存器（注:R12可以做变址寄存器）
                return;
            }
            if ((info.reg1 == X64RegValue::RSP || info.reg1 == X64RegValue::ESP) && (info.reg2 == X64RegValue::RSP || info.reg2 == X64RegValue::ESP)) {
                //// 这里报错，RSP/ESP不允许做变址寄存器（注:R12可以做变址寄存器）
                //// 基址和变址寄存器都是RSP的情况
                return;
            }
            if ((info.reg2 == X64RegValue::RSP || info.reg2 == X64RegValue::ESP) && (info.reg1 != X64RegValue::RSP && info.reg1 != X64RegValue::ESP)) {
                // 变址为rsp/rsp时,基址为其他寄存器时，把基址与变址对调 (rsp/esp必须做基址寄存器)
                X64RegValue temp;
                temp = info.reg2;
                info.reg2 = info.reg1;
                info.reg1 = temp;
            }
            //
            if (!use64)
                u64SetFlag((uint64_t*)&res.type, (uint64_t)MemoryAddressType::with32bitRegAddr);    // 用32位寄存器做内存寻址需要加0x67前缀         
            //
            u64SetFlag((uint64_t*)&res.type, (uint64_t)MemoryAddressType::withModRM);
            u64SetFlag((uint64_t*)&res.type, (uint64_t)MemoryAddressType::withSIB);
            if (u64HasFlag((uint64_t)info.type, (uint64_t)MemoryAddressType::hasSymbol)) {
                // 如果有符号重定位。自动有disp32， 即使是disp8也会变成disp32
                u64SetFlag((uint64_t*)&res.type, (uint64_t)MemoryAddressType::withDisp32);
                u64SetFlag((uint64_t*)&res.type, (uint64_t)MemoryAddressType::withSymbol);
                res.symName = info.symName;
                res.relocType = RelocType::ADDR32;
                res.relocOffset = 2;
            }
            else if (u64HasFlag((uint64_t)info.type, (uint64_t)MemoryAddressType::hasDisp)) {
                long disp = (long)info.disp32;
                if (disp >= (-128) && disp <= 127) {
                    u64SetFlag((uint64_t*)&res.type, (uint64_t)MemoryAddressType::withDisp8);
                }
                else {
                    u64SetFlag((uint64_t*)&res.type, (uint64_t)MemoryAddressType::withDisp32);
                }
            }
            else {
                // 这里判断没有hasDisp时，基地址又是rbp/r13时，必须默默加一个为 0 的 disp8
                if (info.reg1 == X64RegValue::RBP || info.reg1 == X64RegValue::R13 || info.reg1 == X64RegValue::EBP || info.reg1 == X64RegValue::R13D) {
                    u64SetFlag((uint64_t*)&res.type, (uint64_t)MemoryAddressType::withDisp8);
                    info.disp32 = 0;
                }
            }
            //
            res.baseReg = info.reg1;
            res.indexReg = info.reg2;
            res.sacle = info.scale;
            res.disp32 = info.disp32;
            //
            if (u64HasFlag((uint64_t)info.type, (uint64_t)MemoryAddressType::hasExplicitScale)) {
                switch (info.scale) {
                case 1:
                    scaleValue = 0;
                    break;
                case 2:
                    scaleValue = 1;
                    break;
                case 4:
                    scaleValue = 2;
                    break;
                case 8:
                    scaleValue = 3;
                    break;
                }
            }
            baseValue = (uint8_t)((uint64_t)(info.reg1) & 0x07);
            indexValue = (uint8_t)((uint64_t)(info.reg2) & 0x07);
            //
            res.codeSize = 2;
            res.code[0] = 0x04;     // 0b00000100, mod未设定, reg未设定, rm=100表示使用SIB
            res.code[1] = (uint8_t)(scaleValue << 6);          // SIB的组成是 scale(2bit)、index(3bit)、base(2bit)
            res.code[1] |= (uint8_t)(indexValue << 3);
            res.code[1] |= baseValue;
            if (u64HasFlag((uint64_t)info.type, (uint64_t)MemoryAddressType::withDisp8)) {
                res.code[0] |= (0x01 << 6);                             // disp8 设置 mod == 01b
                res.codeSize += 1;
                res.code[2] = (uint8_t)(info.disp32 & 0xff);
            }
            else if (u64HasFlag((uint64_t)info.type, (uint64_t)MemoryAddressType::withDisp32)) {
                res.code[0] |= (0x02 << 6);                             // disp32 设置 mod == 10b
                res.codeSize += 4;
                res.code[2] = (uint8_t)(info.disp32 & 0xff);
                res.code[3] = (uint8_t)((info.disp32 >> 8) & 0xff);
                res.code[4] = (uint8_t)((info.disp32 >> 16) & 0xff);
                res.code[5] = (uint8_t)((info.disp32 >> 24) & 0xff);
            }
            // 是否需要扩展 REX
            if (X64RegUtil::isRexExtensionReg(info.reg1))
                u64SetFlag((uint64_t*)&res.type, (uint64_t)MemoryAddressType::withRex_B);
            if (X64RegUtil::isRexExtensionReg(info.reg2))
                u64SetFlag((uint64_t*)&res.type, (uint64_t)MemoryAddressType::withRex_X);
        }
        else if (info.type.HasFlag(MemoryAddressType.hasReg1) && (!info.type.HasFlag(MemoryAddressType.hasReg2))) {
            // 下面处理有 reg1 , 无 reg2 的情况
            int dispStart = 1;
            use64 = X64RegUtil.Is64BitReg(info.reg1);
            //
            if (!use64)
                ret.type |= MemoryAddressType.with32bitRegAddr;             // 用32位寄存器做内存寻址需要加0x67前缀
            //
            ret.codeSize = 1;
            ret.type |= MemoryAddressType.withModRM;
            ret.code[0] = 0;
            if (info.type.HasFlag(MemoryAddressType.hasDisp) || info.type.HasFlag(MemoryAddressType.hasSymbol)) {
                if (info.type.HasFlag(MemoryAddressType.hasSymbol)) {
                    // 如果有符号重定位。自动有disp32， 即使是disp8也会变成disp32
                    ret.type |= MemoryAddressType.withDisp32;
                    ret.type |= MemoryAddressType.withSymbol;
                    ret.symName = info.symName;
                    ret.relocType = RelocType.ADDR32;
                }
                else {
                    long disp = (long)info.disp32;
                    if (disp >= (-128) && disp <= 127) {
                        ret.type |= MemoryAddressType.withDisp8;
                    }
                    else {
                        ret.type |= MemoryAddressType.withDisp32;
                    }
                }
            }
            else {
                // 这里判断没有hasDisp时，基地址又是rbp/r13时，必须默默加一个为 0 的 disp8
                if (info.reg1 == X64RegValue.RBP || info.reg1 == X64RegValue.R13 || info.reg1 == X64RegValue.EBP || info.reg1 == X64RegValue.R13D) {
                    ret.type |= MemoryAddressType.withDisp8;
                    info.disp32 = 0;
                }
            }
            // 基地址是RSP/R12的，需要用 SIB 来表达
            if (info.reg1 == X64RegValue.RSP || info.reg1 == X64RegValue.ESP || info.reg1 == X64RegValue.R12 || info.reg1 == X64RegValue.R12D) {
                dispStart++;
                ret.codeSize++;
                ret.type |= MemoryAddressType.withSIB;
                ret.code[0] = 0x04;                             // modRM 中的 r/m 域设置为 100b
                ret.code[1] = 0x24;                             // SIB 中的 scale 设置为 00b, index 设置为 100b, base 设置为 100b (index==100b表示 index和sacle无效)
            }
            else {
                baseValue = (byte)((uint)(info.reg1) & 0x07);
                ret.code[0] |= baseValue;                       // modRM 中的 r/m 域设置为 寄存器的值
            }
            //
            if (ret.type.HasFlag(MemoryAddressType.withDisp8)) {
                ret.code[0] |= (0x01 << 6);                             // disp8 设置 mod == 01b
                ret.codeSize += 1;
                ret.code[dispStart + 0] = (byte)(info.disp32 & 0xff);
            }
            else if (ret.type.HasFlag(MemoryAddressType.withDisp32)) {
                ret.code[0] |= (0x02 << 6);                             // disp32 设置 mod == 10b
                ret.codeSize += 4;
                ret.code[dispStart + 0] = (byte)(info.disp32 & 0xff);
                ret.code[dispStart + 1] = (byte)((info.disp32 >> 8) & 0xff);
                ret.code[dispStart + 2] = (byte)((info.disp32 >> 16) & 0xff);
                ret.code[dispStart + 3] = (byte)((info.disp32 >> 24) & 0xff);
                if (ret.type.HasFlag(MemoryAddressType.withSymbol)) {
                    ret.relocOffset = dispStart;
                }
            }
            // 是否需要扩展 REX
            if (X64RegUtil.IsRexExtensionReg(info.reg1))
                ret.type |= MemoryAddressType.withRex_B;
        }
        else if ((!info.type.HasFlag(MemoryAddressType.hasReg1)) && info.type.HasFlag(MemoryAddressType.hasReg2)) {
            // 下面处理没 reg1 , 有 reg2 的情况 (没有基址寄存器，但是有变址寄存器，必须后面带一个disp32作为基地址)
            if (info.reg2 == X64RegValue.RSP || info.reg2 == X64RegValue.ESP) {
                //// 这里报错，RSP/ESP不允许做变址寄存器（注:R12可以做变址寄存器）
                //// 基址和变址寄存器都是RSP的情况
                return;
            }
            //
            use64 = X64RegUtil.Is64BitReg(info.reg2);
            //
            if (!use64)
                ret.type |= MemoryAddressType.with32bitRegAddr;             // 用32位寄存器做内存寻址需要加0x67前缀
            if (!info.type.HasFlag(MemoryAddressType.hasDisp)) {
                info.disp32 = 0;
            }
            ret.type |= MemoryAddressType.withDisp32;
            // mod == 00，r/m == 100，base == 101时，base基址寄存器字段并不是表示 RBP/EBP/R13，而是要忽略这个基址寄存器，不存在基址寄存器，只存在变址寄存器，并把一个 32 位的偏移量作为基地址。
            if (info.type.HasFlag(MemoryAddressType.hasExplicitScale)) {
                switch (info.scale) {
                case 1:
                    scaleValue = 0;
                    break;
                case 2:
                    scaleValue = 1;
                    break;
                case 4:
                    scaleValue = 2;
                    break;
                case 8:
                    scaleValue = 3;
                    break;
                }
            }
            indexValue = (byte)((uint)(info.reg2) & 0x07);
            ret.type |= MemoryAddressType.withSIB;
            ret.type |= MemoryAddressType.withModRM;
            ret.codeSize = 6;
            ret.code[0] = 0x04;     // 0b00000100, mod设为00b, reg未设定, rm=100表示使用SIB
            ret.code[1] = (byte)(scaleValue << 6);          // SIB的组成是 scale(2bit)、index(3bit)、base(2bit)
            ret.code[1] |= (byte)(indexValue << 3);
            ret.code[1] |= 0x05;                            // base设置为 101b (base==101b表示必须带disp做偏移量，此时如果mod==00b，则表示没有基址寄存器,disp的数值作为基址)
            ret.code[2] = (byte)(info.disp32 & 0xff);
            ret.code[3] = (byte)((info.disp32 >> 8) & 0xff);
            ret.code[4] = (byte)((info.disp32 >> 16) & 0xff);
            ret.code[5] = (byte)((info.disp32 >> 24) & 0xff);
            if (info.type.HasFlag(MemoryAddressType.hasSymbol)) {
                ret.type |= MemoryAddressType.withSymbol;
                ret.symName = info.symName;
                ret.relocType = RelocType.ADDR32;
                ret.relocOffset = 2;
            }
            // 是否需要扩展 REX
            if (X64RegUtil.IsRexExtensionReg(info.reg2))
                ret.type |= MemoryAddressType.withRex_X;
        }
        else {          // 没有reg1和reg2的情况
            if (info.type.HasFlag(MemoryAddressType.hasDisp) || info.type.HasFlag(MemoryAddressType.hasSymbol)) {
                if (info.type.HasFlag(MemoryAddressType.hasAddr32)) {
                    // 使用32位绝对地址，
                    // mod == 00，r/m == 100，base == 101时，base基址寄存器字段并不是表示 RBP/EBP/R13(因为必须带disp)，而是要忽略这个基址寄存器，不存在基址寄存器，只存在变址寄存器，并把后面的一个 32 位的偏移量作为基地址
                    ret.type |= MemoryAddressType.withModRM;
                    ret.type |= MemoryAddressType.withSIB;
                    ret.type |= MemoryAddressType.withDisp32;
                    ret.code[0] = 0x04;     // mod==00b, rm=100b
                    ret.code[1] = 0x25;     // scale=00b, index==100b, base=101b
                    ret.code[2] = (byte)(info.disp32 & 0xff);
                    ret.code[3] = (byte)((info.disp32 >> 8) & 0xff);
                    ret.code[4] = (byte)((info.disp32 >> 16) & 0xff);
                    ret.code[5] = (byte)((info.disp32 >> 24) & 0xff);
                    ret.codeSize = 6;
                    if (info.type.HasFlag(MemoryAddressType.hasSymbol)) {
                        ret.type |= MemoryAddressType.withSymbol;
                        ret.symName = info.symName;
                        ret.relocType = RelocType.ADDR32;
                        ret.relocOffset = 2;
                    }
                }
                else if (info.type.HasFlag(MemoryAddressType.hasAddr64)) {
                    // 使用64位绝对地址, 只有AL/AX/EAX/RAX 的 mov 指令有效
                    ret.type |= MemoryAddressType.with64bitAbsAddr;
                    ret.code[0] = (byte)(info.disp32 & 0xff);
                    ret.code[1] = (byte)((info.disp32 >> 8) & 0xff);
                    ret.code[2] = (byte)((info.disp32 >> 16) & 0xff);
                    ret.code[3] = (byte)((info.disp32 >> 24) & 0xff);
                    ret.code[4] = (byte)((info.disp32 >> 32) & 0xff);
                    ret.code[5] = (byte)((info.disp32 >> 40) & 0xff);
                    ret.code[6] = (byte)((info.disp32 >> 48) & 0xff);
                    ret.code[7] = (byte)((info.disp32 >> 56) & 0xff);
                    ret.codeSize = 8;
                    if (info.type.HasFlag(MemoryAddressType.hasSymbol)) {
                        ret.type |= MemoryAddressType.withSymbol;
                        ret.symName = info.symName;
                        ret.relocType = RelocType.ADDR64;
                        ret.relocOffset = 0;
                    }
                }
                else {
                    // PC 相对寻址
                    // mod==00b, r/m=101b 表示只采用 rip 相对寻址(相对下一条指令) - 注:x86规定rbp/r13必须带8/32的偏移量，如mod==00b则没有偏移量，用以表达rip相对寻址 
                    ret.type |= MemoryAddressType.withModRM;
                    ret.code[0] = 0x05;             // mod=00b, reg未知, r/m=101b
                    ret.code[1] = (byte)(info.disp32 & 0xff);
                    ret.code[2] = (byte)((info.disp32 >> 8) & 0xff);
                    ret.code[3] = (byte)((info.disp32 >> 16) & 0xff);
                    ret.code[4] = (byte)((info.disp32 >> 24) & 0xff);
                    ret.codeSize = 5;
                    if (info.type.HasFlag(MemoryAddressType.hasSymbol)) {
                        ret.type |= MemoryAddressType.withSymbol;
                        ret.symName = info.symName;
                        ret.relocType = RelocType.REL32;
                        ret.relocOffset = 1;
                    }
                }
            }
            else {
                //// 报错
                return;
            }
        }

        op.type = X64OperandType.MemoryAddress;
        op.addressRes = ret;
        //MemoryAddress
    }

    return target;

    /*
     public enum MemoryAddressType
{
    None = 0,
    // 源码层面的信息
    hasReg1 = 0x01,                 // 有寄存器1
    hasReg2 = 0x02,                 // 有寄存器2 （当有两个寄存器的时候，一定是SIB基址加变址，这时也一定有scale比例因子，隐藏的因子为1）
    hasExplicitScale = 0x04,        // 源码中有显式的比例因子(如果有显式的比例因子，则reg1和reg2不能互相调换基址寄存器和变址寄存器来适应一些特殊寄存器要求)
    hasDisp = 0x08,                 // 是否有数值上的偏移量
    hasSymbol = 0x10,               // 是否由符号来寻址(由符号来决定偏移量)
    // 机器层面
    withModRM = 0x1000,             // 此项其实一定有(除了 with64bitAbsAddr)
    withSIB = 0x2000,
    withDisp8 = 0x4000,
    withDisp32 = 0x8000,
    withSegment = 0x10000,          // 带有段前缀
    withNumericDisp = 0x20000,      // 源码层面带有数值上的偏移量
    withSymbol = 0x40000,           // 源码层面带有符号上的偏移量 (如果此项目有，则 withDisp32 或 with64bitAbsAddr 一定有其一)
    with32bitRegAddr = 0x80000,     // 使用了32位寄存器来寻址(如果此项目有，则要加0x67前缀)
    with32bitImmBase = 0x100000,    // 使用32位的无符号立即数做基址(此时 withSIB 一定有，withDisp32 一定有)
                                    // 注：规定rbp/r13做基址时必须带偏移量，rsp禁止做变址（rsp做变址表示没有变址也没有比例因子）
                                    //    所以如果mod==00，并且base==rbp/r13, index==rsp时，表示使用一个无符号的32位数值做基地址(这时可能会产生ADDR32重定位)
    with64bitAbsAddr = 0x200000,    // 使用64位绝对地址来寻址，
}
         */

}



