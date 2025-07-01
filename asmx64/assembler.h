
#ifndef ASMX64_ASM_H
#define ASMX64_ASM_H 1

#include <string>
#include <vector>
#include <list>
#include <memory>

class X64Assembler
{
private:
    bool boNeedReScan = false;
    std::list<SourceLinePrePro> sourceLinesP;           // 将源代码进行预处理的行
    std::list<SourceLine>    sourceLines;
	X64SectionList 			 sectionList;
public:
    X64SymbolList globalSymbolList;

    /*
     private 

private LargeList<SourceLine>    sourceLines;
public X64SymbolList globalSymbolList;
public X64FragmentList fragmentList;
 */
private:
    void assemblePass1();
    void assemblePass2();
    void addLine(SourceLine & parsedLine);
public:
    X64Assembler();
    void assemble(const std::string & filepath);
    uint64_t getCurrOffset(uint64_t sectionIndex);
    void setNeedRescan(bool value1);
    bool isNeedRescan();
    void addCodeSize(uint64_t codesize, SourceLine & sourceLine, int pass);
    
};

#endif // 



