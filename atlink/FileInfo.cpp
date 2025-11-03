#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include "FileInfo.h"

// libraryFlag 标志表示命令行加了 -l 
FileInfo::FileInfo(const std::string & name, bool libraryFlag) {

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

ObjectFile::ObjectFile(const std::string & name) :
    FileInfo(name, false)
{
}

LibraryFile::LibraryFile(const std::string & name) :
    FileInfo(name, true)
{

}

int InputList::addObject(const char * name) 
{
    ObjectFile * obj = new ObjectFile(name);
    fileList.push_back(obj);
    return 0;
}


int InputList::addLibrary(const char * name) {
    LibraryFile * lib = new LibraryFile(name);
    fileList.push_back(lib);
    return 0;
}

