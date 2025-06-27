#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "asmx64.h"

using namespace std;

X64Assembler::X64Assembler() :
    sectionList(this)
{
}

void X64Assembler::assemble(const std::string & filepath)
{
    // init 
    X64Token::initTokenInfos();
    //
    SourceLoader loader;
    loader.loadFile(filepath);
    SourceParser parser(&sourceLinesP, &loader);
    parser.parse();
    
    assemblePass1();

    do {
        assemblePass2();
        if (!boNeedReScan) {
            break;
        }
        printf("=====哈哈=====\n");
    } while (true);

    //Console.WriteLine("open ok");
    //string s = Encoding.UTF8.GetString(src);
    //Console.WriteLine(s);
}

void X64Assembler::assemblePass1()
{
    size_t lineCnt = 0;
    uint32_t insnStartIdx = 0;                                       // 当前行中，指令的token的起始索引
    uint32_t operandStartIdx = 0;                                    // 当前行中，操作数的token的起始索引
	IInsnProcessor * insnProcessor = nullptr;						 // 指令处理器

    for (auto it = sourceLinesP.begin(); it != sourceLinesP.end(); it++) {
        lineCnt++;
		//
		insnStartIdx = 0;
		operandStartIdx = 0;
		//
        SourceLinePrePro & curLine = *it;
        SourceLine parsedLine;
        //
        parsedLine.rawContent = curLine.rawLine;
        if (curLine.tokens.size() == 0) {
            continue;
        }
        try {
            if (curLine.tokens.size() >= 2) {
                string & token0 = curLine.tokens[0];
                string & token1 = curLine.tokens[1];
                if (token1 == ":") {
                    insnStartIdx = 2;
                    parsedLine.hasLabel = true;
                    parsedLine.labelStr = token0;
                }
                else if (X64Token::isAllowNamePseudoInstruction(token1)) {    // 是否为 允许name在前面的伪指令 
                    insnStartIdx = 1;
                    parsedLine.hasLabel = true;
                    parsedLine.labelStr = token0;
                }
                else {
                    insnStartIdx = 0;
                    parsedLine.hasLabel = false;
                    parsedLine.labelStr = "";
                }
            }

            // 找出所有前缀
            uint32_t j = insnStartIdx;
            while (j < curLine.tokens.size()) {
                if (X64Token::isInstructionPrefix(curLine.tokens[j])) {
                    CpuInsnPrefixID prefixId = X64Token::getInstructionPrefixValue(curLine.tokens[j]);
                    parsedLine.hasInsnPrefix = true;
                    parsedLine.prefixs |= (uint32_t) prefixId;
                }
                else {
                    break;
                }
                j++;
            }
            insnStartIdx = j;

			//
			string insnStr = "";
			if (insnStartIdx < curLine.tokens.size()) {
				bool needCalcExpression = false;
				//
				insnStr = curLine.tokens[insnStartIdx];
				if (X64Token::isCpuInstruction(insnStr)) {
					operandStartIdx = insnStartIdx + 1;
					if (operandStartIdx < curLine.tokens.size()) {
						X64Expression::parseByPreProcessTokens(parsedLine.expressions, curLine.tokens, operandStartIdx);
						needCalcExpression = true;
					}
					insnProcessor = X64CpuInsnList::getInstance().getInsnProcessor(insnStr);
					parsedLine.hasInsn = true;
					parsedLine.insnStr = insnStr;
					parsedLine.sectionIndex = sectionList.getCurrSectionIndex();
				}
				else if (X64Token::isVirtualInstruction(insnStr)) {
					operandStartIdx = insnStartIdx + 1;
					if (operandStartIdx < curLine.tokens.size()) {
						X64Expression::parseByPreProcessTokens(parsedLine.expressions, curLine.tokens, operandStartIdx);
						needCalcExpression = true;
					}
					parsedLine.hasInsn = true;
					parsedLine.insnStr = insnStr;
				}
				else if (X64Token::isPseudoInstruction(insnStr)) {
					operandStartIdx = insnStartIdx + 1;
					insnProcessor = X64PseudoInsnList::getInstance().getPseudoInsnProcessor(insnStr);
					if (insnProcessor != nullptr) {
						InsnProcessFlag howto = insnProcessor->getInsnFlag();
						if (howto == InsnProcessFlag::ProcessTokens) {                           // 每个指令自己处理自己后面的token
							operandStartIdx = insnStartIdx + 1;
							if (operandStartIdx < curLine.tokens.size()) {
								parsedLine.keepFollowingToken = true;
								for (int i = operandStartIdx; i < curLine.tokens.size(); i++) {
									parsedLine.followingTokens.push_back(curLine.tokens[i]);
								}
							}
							
						}
					}
					parsedLine.hasInsn = true;
					parsedLine.insnStr = insnStr;
				}
				else {
					//// 不认识的指令，报错
					continue;
				}
				// 初步计算表达式的值
				if (needCalcExpression) {
					if (parsedLine.expressions.size() != 0) {
						for (auto it = parsedLine.expressions.begin(); it != parsedLine.expressions.end(); it++) {
							it->calc();
						}
					}

				}
			}

			if (parsedLine.hasLabel && !parsedLine.hasInsn) {
				//// TO-DO 这里处理有标签但是没有指令的情况
			}

			if (insnProcessor != nullptr) {
				insnProcessor->process(*this, insnStr, parsedLine, 1);
			}
        } catch (...) {
        }

    }
}

void X64Assembler::assemblePass2()
{
    
}

void X64Assembler::setNeedRescan(bool value1)
{
    boNeedReScan = value1;
}

void X64Assembler::addCodeSize(uint32_t codesize, SourceLine & sourceLine, int pass)
{

}

uint64_t X64Assembler::getCurrOffset(uint64_t sectionIndex)
{
    return 0;
}


