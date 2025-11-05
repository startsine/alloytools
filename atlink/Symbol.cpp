#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "Symbol.h"

SymbolList::SymbolList()
{
    globalFinder.reserve(10000);
}

SymbolList::~SymbolList()
{
    if (symbols.size() != 0) {
        for (size_t i = 0; i < symbols.size(); i++) {
            if (symbols[i] != nullptr) {
                delete symbols[i];
                symbols[i] = nullptr;
            }
        }
    }
}



