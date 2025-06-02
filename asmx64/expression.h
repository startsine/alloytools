#ifndef ASMX64_EXPRESSION_H
#define ASMX64_EXPRESSION_H 1

#include <string>
#include <vector>

class X64Expression
{
public:
    // 把预处理后的tokens转换为表达式列表,成功返回true，失败返回false
	static bool parseByPreProcessTokens(std::vector<X64Expression> & expressions, std::vector<std::string> & tokens, int startIdx);

};


#endif // ASMX64_EXPRESSION_H

