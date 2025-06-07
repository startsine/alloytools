#ifndef ASMX64_PSEUDO_INSN_PROC_H
#define ASMX64_PSEUDO_INSN_PROC_H 1

class PROC : public IInsnProcessor 
{
public:
	int process(X64Assembler &assembler, std::string insnStr, SourceLine &sourceLine, int pass) override;
	InsnProcessFlag getInsnFlag() override;
};


#endif // ASMX64_PSEUDO_INSN_PROC_H

