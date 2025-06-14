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
            operand = new X64Operand(token.regValue);
            calculated = true;
            return true;
        }
        case X64TokenType::Numeric: {
            if (token.flag.HasFlag(X64TokenFlag::NumericToBeUlong)) {    // 如果已经转换为ulong
                operand = new X64Operand(token.ulongValue);
                calculated = true;
                return true;
            }
        }
                                   break;
        }
        return false;
    }
    else {
        int addrStartIdx = FindOperator(this.tokens, 0, X64AsmOperator.AddressStart);
        int addrEndIndex = FindOperator(this.tokens, 0, X64AsmOperator.AddressEnd);
        if (addrStartIdx >= 0) {
            if (addrEndIndex >= 0 && addrEndIndex > addrStartIdx) {
                calcAddressExpression(addrStartIdx, addrEndIndex);
            }
        }
    }
    //

    return true;
}




