#ifndef ASMX64_EXPRESSION_H
#define ASMX64_EXPRESSION_H 1

#include <string>
#include <vector>

class X64Expression
{
public:
    std::vector<X64Token> tokens;
    bool calculated = false;                                             // 已经计算标志
    X64Operand operand;                                                  // 结果操作数

    // 把预处理后的tokens转换为表达式列表,成功返回true，失败返回false
	static bool parseByPreProcessTokens(std::vector<X64Expression> & expressions, std::vector<std::string> & tokens, int startIdx);
    // 计算表达式
    bool calc();
    // 在token-list中查找操作符
    static int findOperator(std::vector<X64Token> & xTokens, int startIndex, X64AsmOperator opt);
private:
    // 计算寻址表达式，就是 [] 的部分, 成功返回 X64Operand，失败返回 null
    X64Operand calcAddressExpression(int start, int end);       // start和end是第一个token和最后一个token的索引，正常来说应该分别是 '[' 和 ']'
};


#endif // ASMX64_EXPRESSION_H

