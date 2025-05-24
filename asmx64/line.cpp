#include "asmx64.h"

enum class CurrTokenStartType   // 当前词法分析的Token起始类型
{
    None,
    Identifier,                     // 标识符开始 (非数字开头，允许字母、数字、$、?、@、_)
    Numeric,                        // 数字
    StringSingleQuote,              // 单引号开始的字符串
    StringDoubleQuote,              // 双引号开始的字符串
    Comment,                        // 注释
};

SourceLoader::SourceLoader()
{
    
}

void SourceLoader::loadFile(const std::string & filepath)
{
    
}

SourceParser::SourceParser(const std::list<SourceLinePrePro> * lines, const SourceLoader * loader)
{
    //
}

bool SourceParser::parse()
{
    return true;
}



