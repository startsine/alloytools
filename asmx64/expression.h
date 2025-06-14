#ifndef ASMX64_EXPRESSION_H
#define ASMX64_EXPRESSION_H 1

#include <string>
#include <vector>

class X64Expression
{
public:
    std::vector<X64Token> tokens;
    bool calculated = false;                                             // 已经计算标志
    X64Operand ? operand = null;                                          // 结果操作数

    // 把预处理后的tokens转换为表达式列表,成功返回true，失败返回false
	static bool parseByPreProcessTokens(std::vector<X64Expression> & expressions, std::vector<std::string> & tokens, int startIdx);
    // 计算表达式
    bool calc();
};


#endif // ASMX64_EXPRESSION_H

