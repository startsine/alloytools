

using Microsoft.VisualBasic;
using System.Collections;

namespace AlloyTools.Assembler.AMD64
{
    public enum X64TokenType : uint
    {
        Register = 0x01,                                    // 寄存器
        Operator = 0x02,                                    // 运算符
        Numeric = 0x04,                                     // 数字
        Symbol = 0x08,                                      // 符号
        String = 0x10,                                      // 字符串
        //
        NumericToBeUlong = 0x1000,                          // 数字已经转换为ulong
        //
        PseudoInstruction = 0x10000000,                     // 伪指令标志
        DataPseudoInstruction = 0x20000000,                 // 是否产生数据的伪指令（DB,DW,DD,DQ等）
        CpuInstruction = 0x40000000,                        // CPU真实指令标志
        PrefixInstruction = CpuInstruction | 0x80000000,    // 前缀指令（LOCK/REPNE/REPNZ/REP/REPE/REPZ等），前缀指令同时也是CPU真实指令标志
    }

    public enum X64AsmOperator
    {
        None = 0,
    }

    public class X64Token
    {
        public X64TokenType tokenType;
        public bool isRawData = false;                      // token的字符串是否使用byte[]原始格式
        public string str = string.Empty;                   // token的字符串, string 类型
        public byte[]? rawBytes = null;                     // token的字符串, byte[] 类型，token类型为String有效
        public X64RegValue regValue;                        // 寄存器的值, tokenType 为 Register 时有效
        public X64AsmOperator asmOperator;                  // 运算符的值, tokenType 为 Operator 时有效
        public ulong ulongValue;                            // 数字的值, tokenType 为 Numeric 时有效
        public ulong symbolIndex;                           // 符号在符号表中的索引, tokenType 为 Symbol 时有效

        private static Hashtable htAllowNamePseudoInstruction = new Hashtable();
        private static Hashtable htInstructionPrefix = new Hashtable();

        static X64Token()
        {
            initWithNamePseudoInstruction();
        }

        private static void initWithNamePseudoInstruction()
        {
            htAllowNamePseudoInstruction.Add("db", 1);
            htAllowNamePseudoInstruction.Add("dw", 1);
            htAllowNamePseudoInstruction.Add("dd", 1);
            htAllowNamePseudoInstruction.Add("dq", 1);
            htAllowNamePseudoInstruction.Add("proc", 1);
            htAllowNamePseudoInstruction.Add("endp", 1);
            htAllowNamePseudoInstruction.Add("segment_data", 1);
            htAllowNamePseudoInstruction.Add("ends", 1);
        }

        private static void initInstructionPrefix()
        {
            htInstructionPrefix.Add("rep", 1);
        }

        // 判断字符串是否为数字token
        public static bool isNumericStr(string str)
        {
            if (str.Length == 0)
                return false;
            if (str[0] >= '0' && str[0] <= '9') {
                return true;
            }
            return false;
        }

        // 是否是允许带name的伪指令
        public static bool isAllowNamePseudoInstruction(string str) 
        {
            string str2 = str.ToLower();
            return htAllowNamePseudoInstruction.ContainsKey(str2);
        }

        // 是否指令前缀
        public static bool isInstructionPrefix(string str) 
        {
            string str2 = str.ToLower();
            return htInstructionPrefix.ContainsKey(str2);
        }

        // 获得指令前缀的ID值
        public static CpuInsnPrefixID getInstructionPrefixValue(string str)
        {
            string str2 = str.ToLower();
            var obj = htInstructionPrefix[str2];
            if (obj == null) {
                return CpuInsnPrefixID.None;
            }
            return (CpuInsnPrefixID)obj;
        }

        // 是否CPU指令
        public static bool isCpuInstruction(string str)
        {
            return X64CpuInsnList.Instance.isCpuInstruction(str);
        }

        // 是否伪指令
        public static bool isPseudoInstruction(string str)
        {
            return X64PseudoInsnList.Instance.isPseudoInstruction(str);
        }


    }
}


