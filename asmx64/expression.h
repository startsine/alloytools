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

    // 复位数据
    void reset();
    // 把预处理后的tokens转换为表达式列表,成功返回true，失败返回false
	static bool parseByPreProcessTokens(std::vector<X64Expression> & expressions, std::vector<std::string> & tokens, int startIdx);
    // 计算表达式
    bool calc();
    // 在token-list中查找操作符
    static int findOperator(const std::vector<X64Token> & xTokens, int startIndex, X64AsmOperator opt);
    // 在token-list中查找任何操作符
    static int findAnyOperator(const std::vector<X64Token> & xTokens, int startIndex);
private:
    // 计算寻址表达式，就是 [] 的部分, 成功返回 X64Operand，失败返回 null
    X64Operand calcAddressExpression(int start, int end);       // start和end是第一个token和最后一个token的索引，正常来说应该分别是 '[' 和 ']'
    // 计算寻址表达式表达式内部值,就是[]内部的值 
    static X64Operand calcAddressExpressionInnerValue(std::vector<X64Token> & xTokens);
    // 寻址中间值转为内存寻址的结果值
    X64Operand memoryAddressInfoToRet(const X64Operand & op);
    // 计算寻址操作的一个最小单元操作
    static X64Operand calcAddressExpressionMinOperator(const X64Token * left, const X64Token * operatorToken, const X64Token * right);
    // 将token转换为 MemoryAddressInfo 类型对象
    static MemoryAddressInfo toTempMemoryAddressInfo(const X64Token * token);
    // 获取 MemoryAddressInfo 对象中有存在几个寄存器了
    static int getRegCountFromMemoryAddressInfo(MemoryAddressInfo & info);
};


#endif // ASMX64_EXPRESSION_H

