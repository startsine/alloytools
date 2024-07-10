
using System.Collections;

namespace AlloyTools.Assembler.AMD64
{
    public class X64PseudoInsnList
    {
        private static X64PseudoInsnList? instance = null;
        private Hashtable htPseudoInsns = new Hashtable();

        private X64PseudoInsnList()
        {
        }

        public static X64PseudoInsnList Instance {
            get {
                if (instance == null) {
                    instance = new X64PseudoInsnList();
                }
                return instance;
            }
        }

        public bool isPseudoInstruction(string str)
        {
            string str2 = str.ToLower();
            return htPseudoInsns.ContainsKey(str2);
        }
    }
}

