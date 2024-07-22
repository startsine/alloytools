


using AlloyTools.utils;
using System.Collections;

namespace AlloyTools.Assembler.AMD64
{
    public class X64Assembler
    {
        private LargeList<SourceLinePre> sourceLinesP;
        private LargeList<SourceLine>    sourceLines;
        public X64SymbolList symbolList;

        public X64Assembler()
        {
            sourceLinesP = new LargeList<SourceLinePre>();
            sourceLines = new LargeList<SourceLine>();
            symbolList  = new X64SymbolList();
        }

        public static void Start(string[] args)
        {
            if (args.Length > 0) {
                Assembler(args[0]);
            }
        }

        private static void Assembler(string filepath)
        {
            var loader = new SourceLoader();
            loader.LoadFile(filepath);

            var parser = new SourceParser(sourceLinesP, loader);
            parser.Parse();
            //
            
            AssemblerPass1();

            Hashtable ht = new Hashtable();
            var ss = ht.Count;
            //Console.WriteLine("open ok");
            //string s = Encoding.UTF8.GetString(src);
            //Console.WriteLine(s);
        }

        private static void AssemblerPass1()
        {
            if (sourceLines == null)
                return;
            IInsnProcessor? insnProcessor = null;
            ulong lineCnt = 0;
            int insnStartIdx = 0;                                       // 当前行中，指令的token的起始索引
            int operandStartIdx = 0;                                    // 当前行中，操作数的token的起始索引
            ulong lineTotal = sourceLinesP!.Count;
            for (lineCnt = 0; lineCnt < lineTotal; lineCnt++) {
                SourceLinePre curLine = sourceLinesP[lineCnt];
                SourceLine parsedLine = new SourceLine();
                parsedLine.rawContent = curLine.rawLine;
                sourceLines.Add(parsedLine);
                //
                if (curLine.tokens == null || curLine.tokens.Count == 0)
                    continue;

                try {
                    if (curLine.tokens.Count >= 2) {
                        var token0 = curLine.tokens[0];
                        var token1 = curLine.tokens[1];
                        if (token1 != null) {
                            if (token1.str == ":") {
                                insnStartIdx = 2;
                                parsedLine.hasLabel = true;
                                parsedLine.labelStr = token0.str;
                            }
                            else if (X64Token.isAllowNamePseudoInstruction(token1.str)) {    // 是否为 允许name在前面的伪指令 
                                insnStartIdx = 1;
                                parsedLine.hasLabel = true;
                                parsedLine.labelStr = token0.str;
                            }
                        }
                    }
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
                        }
                        else if (X64Token.isVirtualInstruction(insnStr)) {
                            operandStartIdx = insnStartIdx + 1;
                            if (operandStartIdx < curLine.tokens.Count) {
                                parsedLine.expressions = new List<X64Expression>();
                                X64Expression.ParseByPreProcessTokens(parsedLine.expressions, curLine.tokens, operandStartIdx);
                                needCalcExpression = true;
                            }
                        }
                        else if (X64Token.isPseudoInstruction(insnStr)) {
                            operandStartIdx = insnStartIdx + 1;
                        }
                        else {
                            // 不认识的指令，报错
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

                    if (insnProcessor is not null) {
                        insnProcessor.process(insnStr, parsedLine, 1);
                    }



                }
                catch (LineErrorException ex1) {
                }
                catch (FatalErrorException ex2) {
                }
                catch {
                }

                
            }
        }
    }
}





