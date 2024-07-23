

using AlloyTools.Assembler.AMD64.Instruction;
using System.Collections;

namespace AlloyTools.Assembler.AMD64
{
    // CPU指令前缀ID列表
    [Flags]
    public enum CpuInsnPrefixID: uint
    {
        None = 0,
    }

    public class X64CpuInsnList
    {
        private static X64CpuInsnList? instance = null;
        private Hashtable htCpuInsns = new Hashtable();

        private X64CpuInsnList() 
        {
            htCpuInsns.Add("mov", new MOV());
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

        public IInsnProcessor? GetInsnProcessor(string insn)
        {
            string str2 = insn.ToLower();
            if (htCpuInsns.ContainsKey(str2)) {
                object? processor = htCpuInsns[str2];
                if (processor is not null) {
                    return processor as IInsnProcessor;
                }
            }
            return null;
        }
    }

}



