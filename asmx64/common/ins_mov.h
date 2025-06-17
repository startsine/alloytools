
#ifndef ASMX64_INSN_MOV_H
#define ASMX64_INSN_MOV_H 1

#include <string>
#include <list>

class InsnMov : public BaseInsn
{
public:
    InsnMov();
	int process(X64Assembler &assembler, const std::string & insnStr, SourceLine &sourceLine, int pass) override;
private:
    static bool insnInitialized;
    static std::list<OpcodeInfos> opcodeInfos;
};

#endif // ASMX64_INSN_MOV_H

