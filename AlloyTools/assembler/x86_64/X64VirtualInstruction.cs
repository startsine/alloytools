
using System.Collections;

namespace AlloyTools.Assembler.AMD64
{
    public class X64VirtualInsnList
    {
        private static X64VirtualInsnList? instance = null;
        private Hashtable htVirtualInsns = new Hashtable();

        private X64VirtualInsnList()
        {
        }

        public static X64VirtualInsnList Instance {
            get {
                if (instance == null) {
                    instance = new X64VirtualInsnList();
                }
                return instance;
            }
        }

        public bool isVirtualInstruction(string str)
        {
            string str2 = str.ToLower();
            return htVirtualInsns.ContainsKey(str2);
        }
    }
}

