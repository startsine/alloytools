#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include "FileInfo.h"

// libraryFlag 标志表示命令行加了 -l 
FileInfo::FileInfo(std::string name, bool libraryFlag) {

}

InputList::InputList() {

}

InputList::~InputList() {
    if (fileList.size() != 0) {
        for (size_t i = 0; i < fileList.size(); i++) {
            if (fileList[i] != nullptr) {
                delete fileList[i];
                fileList[i] = nullptr;
            }
        }
    }
}



