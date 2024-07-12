
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
        public ulong recordIndex;                                               // 位于哪个record
    }

}

