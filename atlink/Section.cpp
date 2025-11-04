#include <stdint.h>
#include <string>
#include "Section.h"


SectionList::SectionList()
{
}

SectionList::~SectionList()
{
    if (sections.size() != 0) {
        for (size_t i = 0; i < sections.size(); i++) {
            if (sections[i] != nullptr) {
                delete sections[i];
                sections[i] = nullptr;
            }
        }
    }
}






