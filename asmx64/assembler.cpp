#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "asmx64.h"

using namespace std;

X64Assembler::X64Assembler()
{
    
}

void X64Assembler::assemble(const std::string & filepath)
{
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
    int insnStartIdx = 0;                                       // 当前行中，指令的token的起始索引
    int operandStartIdx = 0;                                    // 当前行中，操作数的token的起始索引

    for (auto it = sourceLinesP.begin(); it != sourceLinesP.end(); it++) {
        lineCnt++;
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
                else if (X64Token.isAllowNamePseudoInstruction(token1.str)) {    // 是否为 允许name在前面的伪指令 
                    insnStartIdx = 1;
                    parsedLine.hasLabel = true;
                    parsedLine.labelStr = token0.str;
                }
                else {
                    insnStartIdx = 0;
                    parsedLine.hasLabel = false;
                    parsedLine.labelStr = "";
                }
            }
        } catch (...) {
        }

    }
/*
    if (sourceLines == null)
    return;

IInsnProcessor? insnProcessor = null;



ulong lineTotal = sourceLinesP!.Count;
for (lineCnt = 0; lineCnt < lineTotal; lineCnt++) {

    sourceLines.Add(parsedLine);

    try {
        
        // 找出所有前缀
        int j = insnStartIdx;
        while (j < curLine.tokens.Count) {
            if (X64Token.isInstructionPrefix(curLine.tokens[j].str)) {
                var prefixId = X64Token.getInstructionPrefixValue(curLine.tokens[j].str);
                parsedLine.hasInsnPrefix = true;
                parsedLine.prefixs |= prefixId;
            }
            else {
                break;
            }
            j++;
        }
        insnStartIdx = j;

        //
        string insnStr = "";
        if (insnStartIdx < curLine.tokens.Count) {
            bool needCalcExpression = false;
            //
            insnStr = curLine.tokens[insnStartIdx].str;
            if (X64Token.isCpuInstruction(insnStr)) {
                operandStartIdx = insnStartIdx + 1;
                if (operandStartIdx < curLine.tokens.Count) {
                    parsedLine.expressions = new List<X64Expression>();
                    X64Expression.ParseByPreProcessTokens(parsedLine.expressions, curLine.tokens, operandStartIdx);
                    needCalcExpression = true;
                }
                insnProcessor = X64CpuInsnList.Instance.GetInsnProcessor(insnStr);
                parsedLine.hasInsn = true;
                parsedLine.insnStr = insnStr;
                parsedLine.fragmentIndex = fragmentList.GetCurrFragmentIndex();
                parsedLine.recordIndex = fragmentList.GetCurrFragment()!.GetCurrRecordtIndex();
            }
            else if (X64Token.isVirtualInstruction(insnStr)) {
                operandStartIdx = insnStartIdx + 1;
                if (operandStartIdx < curLine.tokens.Count) {
                    parsedLine.expressions = new List<X64Expression>();
                    X64Expression.ParseByPreProcessTokens(parsedLine.expressions, curLine.tokens, operandStartIdx);
                    needCalcExpression = true;
                }
                parsedLine.hasInsn = true;
                parsedLine.insnStr = insnStr;
            }
            else if (X64Token.isPseudoInstruction(insnStr)) {
                operandStartIdx = insnStartIdx + 1;
                insnProcessor = X64PseudoInsnList.Instance.GetPseudoInsnProcessor(insnStr);
                if (insnProcessor is not null) {
                    InsnProcessFlag howto = insnProcessor.GetInsnFlag();
                    if (howto == InsnProcessFlag.ProcessTokens) {                           // 每个指令自己处理自己后面的token
                        operandStartIdx = insnStartIdx + 1;
                        if (operandStartIdx < curLine.tokens.Count) {
                            parsedLine.keepFollowingToken = true;
                            parsedLine.followingTokens = new List<PreProToken>();
                            for (int i = operandStartIdx; i < curLine.tokens.Count; i++) {
                                parsedLine.followingTokens.Add(curLine.tokens[i]);
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
                if (parsedLine.expressions is not null) {
                    foreach (X64Expression expr in parsedLine.expressions) {
                        expr.calc();
                    }
                }

            }
        }

        if (parsedLine.hasLabel && !parsedLine.hasInsn) {
            /// TO-DO 这里处理有标签但是没有指令的情况
        }

        if (insnProcessor is not null) {
            insnProcessor.process(this, insnStr, parsedLine, 1);
        }



    }
    catch (LineErrorException ex1) {
    }
    catch (FatalErrorException ex2) {
    }
    catch {
    }

    
}
*/
}

void X64Assembler::assemblePass2()
{
    
}



