
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
            htPseudoInsns.Add("proc", new MOV());
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
    }
}

