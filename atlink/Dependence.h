#pragma once
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <queue>
#include <mutex>

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





