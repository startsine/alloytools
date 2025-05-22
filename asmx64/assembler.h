
#ifndef ASMX64_ASM_H
#define ASMX64_ASM_H 1

#include <string>

class X64Assembler
{
private:
    void assemblePass1();
    void assemblePass2();
public:
    X64Assembler();
    void assemble(const std::string & filepath);
};

#endif // 



