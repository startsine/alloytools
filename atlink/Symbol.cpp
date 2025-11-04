#include <stdint.h>
#include <string>
#include "Symbol.h"

SymbolList::SymbolList()
{
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



