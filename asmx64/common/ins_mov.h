
#ifndef ASMX64_INSN_MOV_H
#define ASMX64_INSN_MOV_H 1

#include <string>

class InsnMov : public BaseInsn
{
public:
	int process(X64Assembler &assembler, std::string insnStr, SourceLine &sourceLine, int pass) override;
};

#endif // ASMX64_INSN_MOV_H

