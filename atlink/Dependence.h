#pragma once
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <queue>
#include <mutex>
#include <unordered_set>

// 外部符号依赖队列
class SymbolDepend
{
private:
    std::queue<std::string>     dependList;
    //std::mutex                  mutex;
public:
    size_t addSymbol(const std::string & str);
    std::string getSymbol();
    bool isEmpty();
};

// 已经解决的外部依赖符号列表
class FoundExternSymbolList
{
private:
    std::unordered_set<std::string> symbols;
public:
    size_t addSymbol(const std::string & str);
    bool inList(const std::string & str);
};





