#ifndef ASMX64_PSEUDO_INSN_DATA_H
#define ASMX64_PSEUDO_INSN_DATA_H 1

class Segment : public IInsnProcessor 
{
public:
	int process(X64Assembler &assembler, std::string insnStr, SourceLine &sourceLine, int pass) override;
	InsnProcessFlag getInsnFlag() override;
};


#endif // ASMX64_PSEUDO_INSN_DATA_H


