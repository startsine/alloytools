#ifndef ASMX64_OUTPUT_H
#define ASMX64_OUTPUT_H 1

class X64Assembler;

class ElfWriter
{
protected:
    X64Assembler & assembler;
public:
    ElfWriter(X64Assembler * assem);
    bool write();
};


#endif // ASMX64_OUTPUT_H


