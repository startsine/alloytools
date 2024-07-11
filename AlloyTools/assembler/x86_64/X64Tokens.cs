

using Microsoft.VisualBasic;
using System.Collections;
using System.Reflection;

namespace AlloyTools.Assembler.AMD64
{
    public enum X64TokenType : uint
    {
        None = 0,
        String,                                      // 字符串
        Numeric,                                     // 数字
        Register,                                    // 寄存器
        Operator,                                    // 运算符
        Symbol,                                      // 符号
    }

    [Flags]
    public enum X64TokenFlag : uint
    {
        None = 0,
        NumericToBeUlong = 0x1000,                          // 数字已经转换为ulong, type为Numeric时有效
    }

    public enum X64AsmOperator
    {
        None = 0,
        Plus,               // +
        Minus,              // -
        Multiplication,     // *
        Division,           // /
        AddressStart,       // [
        AddressEnd,         // ]
    }

    public class X64Token
    {
        public X64TokenType tokenType;
        public X64TokenFlag flag;
        public bool isRawData = false;                      // token的字符串是否使用byte[]原始格式
        public string str = string.Empty;                   // token的字符串, string 类型
        public byte[]? rawBytes = null;                     // token的字符串, byte[] 类型，token类型为String有效
        public X64RegValue regValue;                        // 寄存器的值, tokenType 为 Register 时有效
        public X64AsmOperator asmOperator;                  // 运算符的值, tokenType 为 Operator 时有效
        public ulong ulongValue;                            // 数字的值, tokenType 为 Numeric 时有效
        public ulong symbolIndex;                           // 符号在符号表中的索引, tokenType 为 Symbol 时有效

        private static Hashtable htAllowNamePseudoInstruction = new Hashtable();
        private static Hashtable htInstructionPrefix = new Hashtable();
        private static Hashtable htRegister = new Hashtable();
        private static Hashtable htOperator = new Hashtable();

        static X64Token()
        {
            initWithNamePseudoInstruction();
            initRegisterList();
            initOperator();
            initInstructionPrefix();
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

        // 尝试初步解释数字字符串为ulong
        public void tryParseToUlongValue()
        {
            var parseHex = (string hexStr, out ulong value1, out bool succeed) => {
                value1 = 0;
                succeed = false;
                uint tmpInt;
                ulong tmpLong = 0;
                checked {
                    try {
                        foreach (var ch in hexStr) {
                            if (ch >= '0' && ch <= '9')
                                tmpInt = (uint)(ch - '0');
                            else if (ch >= 'A' && ch <= 'F')
                                tmpInt = (uint)(ch - 'A' + 10);
                            else if (ch >= 'a' && ch <= 'f')
                                tmpInt = (uint)(ch - 'a' + 10);
                            else if (ch == '_')
                                continue;
                            else {
                                //  不合适字符
                                return;
                            }
                            //
                            tmpLong <<= 4;
                            tmpLong += tmpInt;
                        }
                        value1 = tmpLong;
                        succeed = true;
                    }
                    catch (OverflowException ex) {
                        //
                    }
                }
            };

            var parseBinary = (string binStr, out ulong value1, out bool succeed) => {
                value1 = 0;
                succeed = false;
                uint tmpInt;
                ulong tmpLong = 0;
                checked {
                    try {
                        foreach (var ch in binStr) {
                            if (ch == '0' || ch == '1')
                                tmpInt = (uint)(ch - '0');
                            else if (ch == '_')
                                continue;
                            else {
                                //  不合适字符
                                return;
                            }
                            //
                            tmpLong <<= 1;
                            tmpLong += tmpInt;
                        }
                        value1 = tmpLong;
                        succeed = true;
                    }
                    catch (OverflowException ex) {
                        //
                    }
                }
            };

            var parseDecimal = (string decStr, out ulong value1, out bool succeed) => {
                value1 = 0;
                succeed = false;
                uint tmpInt;
                ulong tmpLong = 0;
                checked {
                    try {
                        foreach (var ch in decStr) {
                            if (ch >= '0' && ch <= '9')
                                tmpInt = (uint)(ch - '0');
                            else if (ch == '_')
                                continue;
                            else {
                                //  不合适字符
                                return;
                            }
                            //
                            tmpLong *= 10;
                            tmpLong += tmpInt;
                        }
                        value1 = tmpLong;
                        succeed = true;
                    }
                    catch (OverflowException ex) {
                        //
                    }
                }
            };

            if (this.tokenType == X64TokenType.Numeric) {
                ulong value1 = 0;
                bool succeed = false;
                if (str.StartsWith("0x") || str.StartsWith("0X")) 
                    parseHex(str.Substring(2), out value1, out succeed);
                else if (str.EndsWith("h") || str.EndsWith("H")) 
                    parseHex(str.Substring(0, str.Length - 1), out value1, out succeed);
                else if (str.StartsWith("0b") || str.StartsWith("0B")) 
                    parseBinary(str.Substring(2), out value1, out succeed);
                else 
                    parseDecimal(str, out value1, out succeed);
                //
                if (succeed) {
                    this.flag |= X64TokenFlag.NumericToBeUlong;
                    this.ulongValue = value1;
                }
            }
        }

        // 是否是允许前面带name的伪指令
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

        // 是否虚拟指令
        public static bool isVirtualInstruction(string str)
        {
            return X64VirtualInsnList.Instance.isVirtualInstruction(str);
        }

        public static bool isRegister(string str)
        {
            string str2 = str.ToLower();
            return htRegister.ContainsKey(str2);
        }

        public static X64RegValue getRegisterValue(string str)
        {
            string str2 = str.ToLower();
            var obj = htRegister[str2];
            if (obj == null) {
                return X64RegValue.None;
            }
            return (X64RegValue)obj;
        }

        private static void initRegisterList()
        {
            htRegister.Add("al", X64RegValue.AL);
            htRegister.Add("cl", X64RegValue.CL);
            htRegister.Add("dl", X64RegValue.DL);
            htRegister.Add("bl", X64RegValue.BL);
            htRegister.Add("ah", X64RegValue.AH);
            htRegister.Add("ch", X64RegValue.CH);
            htRegister.Add("dh", X64RegValue.DH);
            htRegister.Add("bh", X64RegValue.BH);
            htRegister.Add("spl", X64RegValue.SPL);
            htRegister.Add("bpl", X64RegValue.BPL);
            htRegister.Add("sil", X64RegValue.SIL);
            htRegister.Add("dil", X64RegValue.DIL);
            htRegister.Add("r8b", X64RegValue.R8B);
            htRegister.Add("r9b", X64RegValue.R9B);
            htRegister.Add("r10b", X64RegValue.R10B);
            htRegister.Add("r11b", X64RegValue.R11B);
            htRegister.Add("r12b", X64RegValue.R12B);
            htRegister.Add("r13b", X64RegValue.R13B);
            htRegister.Add("r14b", X64RegValue.R14B);
            htRegister.Add("r15b", X64RegValue.R15B);
            htRegister.Add("ax", X64RegValue.AX);
            htRegister.Add("cx", X64RegValue.CX);
            htRegister.Add("dx", X64RegValue.DX);
            htRegister.Add("bx", X64RegValue.BX);
            htRegister.Add("sp", X64RegValue.SP);
            htRegister.Add("bp", X64RegValue.BP);
            htRegister.Add("si", X64RegValue.SI);
            htRegister.Add("di", X64RegValue.DI);
            htRegister.Add("r8w", X64RegValue.R8W);
            htRegister.Add("r9w", X64RegValue.R9W);
            htRegister.Add("r10w", X64RegValue.R10W);
            htRegister.Add("r11w", X64RegValue.R11W);
            htRegister.Add("r12w", X64RegValue.R12W);
            htRegister.Add("r13w", X64RegValue.R13W);
            htRegister.Add("r14w", X64RegValue.R14W);
            htRegister.Add("r15w", X64RegValue.R15W);
            htRegister.Add("eax", X64RegValue.EAX);
            htRegister.Add("ecx", X64RegValue.ECX);
            htRegister.Add("edx", X64RegValue.EDX);
            htRegister.Add("ebx", X64RegValue.EBX);
            htRegister.Add("esp", X64RegValue.ESP);
            htRegister.Add("ebp", X64RegValue.EBP);
            htRegister.Add("esi", X64RegValue.ESI);
            htRegister.Add("edi", X64RegValue.EDI);
            htRegister.Add("r8d", X64RegValue.R8D);
            htRegister.Add("r9d", X64RegValue.R9D);
            htRegister.Add("r10d", X64RegValue.R10D);
            htRegister.Add("r11d", X64RegValue.R11D);
            htRegister.Add("r12d", X64RegValue.R12D);
            htRegister.Add("r13d", X64RegValue.R13D);
            htRegister.Add("r14d", X64RegValue.R14D);
            htRegister.Add("r15d", X64RegValue.R15D);
            htRegister.Add("rax", X64RegValue.RAX);
            htRegister.Add("rcx", X64RegValue.RCX);
            htRegister.Add("rdx", X64RegValue.RDX);
            htRegister.Add("rbx", X64RegValue.RBX);
            htRegister.Add("rsp", X64RegValue.RSP);
            htRegister.Add("rbp", X64RegValue.RBP);
            htRegister.Add("rsi", X64RegValue.RSI);
            htRegister.Add("rdi", X64RegValue.RDI);
            htRegister.Add("r8", X64RegValue.R8);
            htRegister.Add("r9", X64RegValue.R9);
            htRegister.Add("r10", X64RegValue.R10);
            htRegister.Add("r11", X64RegValue.R11);
            htRegister.Add("r12", X64RegValue.R12);
            htRegister.Add("r13", X64RegValue.R13);
            htRegister.Add("r14", X64RegValue.R14);
            htRegister.Add("r15", X64RegValue.R15);
            // 省去 cs、ds、es、ss
            htRegister.Add("fs", X64RegValue.FS);
            htRegister.Add("gs", X64RegValue.GS);
            //
        }

        private static void initOperator()
        {
            htOperator.Add("+", X64AsmOperator.Plus);
            htOperator.Add("-", X64AsmOperator.Minus);
            htOperator.Add("*", X64AsmOperator.Multiplication);
            htOperator.Add("/", X64AsmOperator.Division);
            htOperator.Add("[", X64AsmOperator.AddressStart);
            htOperator.Add("]", X64AsmOperator.AddressEnd);
        }

        public static bool isOperator(string str)
        {
            string str2 = str.ToLower();
            return htOperator.ContainsKey(str2);
        }

        public static X64AsmOperator getOperatorValue(string str)
        {
            string str2 = str.ToLower();
            var obj = htOperator[str2];
            if (obj == null) {
                return X64AsmOperator.None;
            }
            return (X64AsmOperator)obj;
        }
        

    }
}


