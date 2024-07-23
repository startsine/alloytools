
using AlloyTools.Assembler.AMD64.Instruction;
using System.Collections;

namespace AlloyTools.Assembler.AMD64
{
    public class X64PseudoInsnList
    {
        private static X64PseudoInsnList? instance = null;
        private Hashtable htPseudoInsns = new Hashtable();

        private X64PseudoInsnList()
        {
            htPseudoInsns.Add("proc", new PROC());
        }

        public static X64PseudoInsnList Instance {
            get {
                if (instance == null) {
                    instance = new X64PseudoInsnList();
                }
                return instance;
            }
        }

        public bool IsPseudoInstruction(string str)
        {
            string str2 = str.ToLower();
            return htPseudoInsns.ContainsKey(str2);
        }

        public IInsnProcessor? GetPseudoInsnProcessor(string insn)
        {
            string str2 = insn.ToLower();
            if (htPseudoInsns.ContainsKey(str2)) {
                object? processor = htPseudoInsns[str2];
                if (processor is not null) {
                    return processor as IInsnProcessor;
                }
            }
            return null;
        }
    }
}

