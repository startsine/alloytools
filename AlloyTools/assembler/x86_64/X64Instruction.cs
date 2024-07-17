

using System.Collections;

namespace AlloyTools.Assembler.AMD64
{
    // CPU指令前缀ID列表
    [Flags]
    public enum CpuInsnPrefixID: uint
    {
        None = 0,
    }

    // 伪指令ID列表
    public enum PseudoInsnID : uint
    {
        None = 0,
    }

    //CPU指令ID列表
    public enum CpuInsnID : uint
    {
        None = 0,
    }

    public class X64CpuInsnList
    {
        private static X64CpuInsnList? instance = null;
        private Hashtable htCpuInsns = new Hashtable();

        private X64CpuInsnList() 
        {
            htCpuInsns.Add("mov", 1);
        }

        public static X64CpuInsnList Instance {
            get {
                if (instance == null) {
                    instance = new X64CpuInsnList();
                }
                return instance;
            }
        }

        public bool isCpuInstruction(string str)
        {
            string str2 = str.ToLower();
            return htCpuInsns.ContainsKey(str2);
        }

        public InsnProcessor? GetInsnProcessor(string insn)
        {
            if (htCpuInsns.ContainsKey(insn)) {

            }
            return null;
        }
    }

}



