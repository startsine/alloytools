
#ifndef ASMX64_ASM_H
#define ASMX64_ASM_H 1

#include <string>

class X64Assembler
{
private:
    void Assembler(const std::string & filepath);
    void AssemblerPass1();
    void AssemblerPass2();
public:
    public X64Assembler();
};

#endif // 



