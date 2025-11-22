#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>
#include "Dependence.h"

size_t SymbolDepend::addSymbol(const std::string & str)
{
    //mutex.lock();
    dependList.push(str);
    //mutex.unlock();
    return dependList.size();
}

std::string SymbolDepend::getSymbol()
{
    if (dependList.empty()) {
        return "";
    }
    //
    std::string ret = dependList.front();
    dependList.pop();
    return ret;
}

bool SymbolDepend::isEmpty()
{
    return dependList.empty();
}



