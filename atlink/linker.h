#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FileInfo.h"
#include "Section.h"
#include "Symbol.h"

class Linker
{
public:
    InputList       inputList;
    SectionList     flatSections;

    void scanInputObjects();
    void scanObject(ObjectFile & obj);

};

