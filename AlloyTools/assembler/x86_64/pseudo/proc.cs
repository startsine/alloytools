
using System.Text;

namespace AlloyTools.Assembler.AMD64.Instruction
{
    public class PROC: IInsnProcessor
    {
        override public InsnProcessFlag GetInsnFlag()
        {
            return InsnProcessFlag.ProcessTokens;
        }

        override public int process(X64Assembler asm, string insnStr, SourceLine sourceLine, int pass)
        {
            X64FragmentList fragmentList = asm.fragmentList;
            SymboVisibilityType visibilityType = SymboVisibilityType.Static;
            string sectionName = ".text";

            if (pass <= 1) {
                if (sourceLine.keepFollowingToken && sourceLine.followingTokens is not null) {
                    for (int i = 0; i < sourceLine.followingTokens.Count; i++) {
                        PreProToken token = sourceLine.followingTokens[i];
                        if (!string.IsNullOrEmpty(token.str)) {
                            string cmpStr = token.str.ToLower();
                            if (cmpStr == "public") {
                                visibilityType = SymboVisibilityType.Public;
                            }
                            else if (cmpStr == "weak") {
                                visibilityType = SymboVisibilityType.Weak;
                            }
                            else if (cmpStr == "comdat") {
                                visibilityType = SymboVisibilityType.Comdat;
                            }
                            else if (cmpStr == "\"" || cmpStr == "\'") {
                                if (i + 2 >= sourceLine.followingTokens.Count) {
                                    //// 报错，不完整不匹配的的引号字符串
                                }
                                PreProToken token2 = sourceLine.followingTokens[i + 1];
                                PreProToken token3 = sourceLine.followingTokens[i + 2];
                                if (token2.str == cmpStr) {
                                    //// 报错，不允许空串
                                }
                                if (token3.str != cmpStr) {
                                    //// 报错，不完整不匹配的的引号字符串
                                }
                                if (token2.rawBytes is null) {
                                    ////
                                    return 0;
                                }
                                sectionName = Encoding.UTF8.GetString(token2.rawBytes);
                                i += 2;
                            }
                            else {
                                //// 报错
                            }
                        }
                    }
                }
                long fragmentIndex = asm.fragmentList.AddNewFragment(sectionName);
                if (sourceLine.hasLabel && !string.IsNullOrEmpty(sourceLine.labelStr)) {
                    X64Symbol symbol = new X64Symbol();
                    symbol.symbolName = sourceLine.labelStr;
                    symbol.fragmentIndex = fragmentIndex;
                    symbol.recordIndex = 0;
                    symbol.offsetValue = 0;
                    symbol.sizeType = SymboSizeType.Proc;
                    symbol.varType = SymbolVarType.NotConst;
                    symbol.visibType = visibilityType;
                    asm.globalSymbolList.AddSymbol(symbol);
                }
                //
            }
            else {

            }
            Console.WriteLine("PROC");
            return 0;
        }
    }
}



