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

std::string SymbolDepend::getSymbol(bool & isEmpty)
{
    if (dependList.empty()) {
        isEmpty = true;
        return "";
    }
    //
    std::string ret = dependList.front();
    dependList.pop();
    isEmpty = dependList.empty();
    return ret;
}


