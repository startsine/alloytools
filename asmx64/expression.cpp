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
                calcAddressExpression(addrStartIdx, addrEndIndex);
            }
        }
    }
    //

    return true;
}

int X64Expression::findOperator(std::vector<X64Token> & xTokens, int startIndex, X64AsmOperator opt)
{
    for (int i = startIndex; i < xTokens.size(); i++) {
        if (xTokens[i].tokenType == X64TokenType::Operator &&  xTokens[i].asmOperator == opt)
            return i;
    }
    return -1;
}

X64Operand X64Expression::calcAddressExpression(int start, int end)
{
    X64Operand opRet;

    if (start >= (int) tokens.size() || end >= (int) tokens.size())
        return opRet;
    if (tokens[start].tokenType != X64TokenType::Operator || tokens[start].asmOperator != X64AsmOperator::AddressStart)
        return opRet;
    if (tokens[end].tokenType != X64TokenType::Operator || tokens[end].asmOperator != X64AsmOperator::AddressEnd)
        return opRet;
    if (start + 1 >= end)
        return opRet;
    int start1 = start + 1;
    int end1 = end - 1;

    vector<X64Token> xTokens;
    for (int i = start1; i <= end1; i++) {
        xTokens.push_back(tokens[i]);
    }
    operand = calcAddressExpressionInnerValue(xTokens);
    if (operand.type != X64OperandType::Unknown) {
        if (operand.type == X64OperandType::MemoryAddressInfo) {
            memoryAddressInfoToRet();
        }
    }

    //this.tokens
    return opRet;
}



