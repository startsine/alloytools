
using AlloyTools.utils;

namespace AlloyTools.Assembler.AMD64
{
    public enum SymboSizeType
    {
        None = 0,
        Proc,                               // 用 : 定义 (注：用 proc定义也会产生)
        Byte,                               // 用 DB 定义, 或者是 COMM/LCOMM BYTE
        Word,                               // 用 DW 定义, 或者是 COMM/LCOMM WORD
        Dword,                              // 用 DD 定义, 或者是 COMM/LCOMM DWORD
        Qword,                              // 用 DQ 定义, 或者是 COMM/LCOMM QWORD
    }

    public enum SymboVisibilityType
    {
        None = 0,
        Local,                              // 默认，本文件可见
        Public,                             // public 公共
        Extern,                             // 用 extern 声明
        Common,                             // 用 COMM 定义的符号
        LocalCommon,                        // 用 LCOMM 定义的符号
    }

    public enum SymbolVarType
    {
        None = 0,
        Const,                              // 常量符号，用 = 定义
        NotConst,                           // 通过 db/dw/dd/dq/proc/标号 等定义的符号
    }

    public class X64Symbol
    {
        public SymboSizeType sizeType = SymboSizeType.None;                     // 符号的大小类型
        public SymboVisibilityType visibType = SymboVisibilityType.None;        // 可见性
        public SymbolVarType varType = SymbolVarType.None;                      // 标识是变量还是常量
        public ulong recordIndex;                                               // 位于哪个record （section -> fragment/proc -> record）
                                                                                // fragment/proc 是由多个连续的 record 组成
        public ulong offsetValue;                                               // 在 record 中的偏移量 (为const时，这里存放值)
        public string symbolName = "";                                          // 符号名
    }

    public enum X64RecordType
    {
        None = 0,
        Fixed,                                                                  // 固定大小记录
        Variable                                                                // 可变大小记录
    }

    public class X64Record
    {
        public X64RecordType recordType;                                        // 记录类型
        public ulong fixedSize;                                                 // 固定大小 (仅仅 recordType == Fixed 时有效)
        public ulong fragmentIndex;                                             // 属于哪个 Fragment/Proc
    }

    public class X64Fragment
    {
        public string sectionName = "";                                         // 属于哪个 section，默认情况下,代码段是 ".text"，数据段是 ".data"

        public X64Fragment(string secName)
        {
            sectionName = secName;
        }
    }

    public class X64FragmentList
    {
        public long currFragmentIndex;
        public static LargeList<X64FragmentList> fragments;

        static X64FragmentList()
        {
            fragments = new LargeList<X64FragmentList>();
        }

        static ulong getCurrFragmentIndex()
        {
            if (fragments.Count == 0) {
            }
            return 0;
        }
    }
}


