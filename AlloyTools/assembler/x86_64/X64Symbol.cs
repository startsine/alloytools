
using AlloyTools.utils;
using System.Collections;

namespace AlloyTools.Assembler.AMD64
{
    // 重定位类型
    public enum RelocType
    {
        None = 0,
        ADDR64,                                 // 64位绝对地址
        ADDR32,                                 // 32位绝对地址
        REL32,                                  // 相对下一条指令的相对PC寻址
    }

    public class RelocInfo
    {
        public string name = "";
        public RelocType type = RelocType.None;
        public uint offset = 0;
    }

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
        Static,                             // 默认，本文件可见
        Public,                             // public 公共
        Weak,                               // public 公共, 但是是弱符号
        Comdat,                             // public 公共, 但是是 Comdat 类型
        Extern,                             // 用 extern 声明
        Common,                             // 用 COMM 定义的符号
        LocalCommon,                        // 用 LCOMM 定义的符号
        Local,                              // local，非全局符号  (只在同一个 fragment 内有效)
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
        public long fragmentIndex = -1;                                         // 位于哪个 fragment
        public int recordIndex = -1;                                            // 位于哪个 record （section -> fragment/proc -> record）
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
        public X64RecordType recordType = X64RecordType.Fixed;                  // 记录类型
        public ulong size = 0;                                                  // 记录的大小 （如果type==Variable, 这里表示可变记录最大的可能大小）
    }

    public class X64Fragment
    {
        public string sectionName = "";                                         // 属于哪个 section，默认情况下,代码段是 ".text"，数据段是 ".data"
        public uint align = 1;                                                  // 对齐
        public bool hasRecord = false;                                          // 此 Fragment 是否有 record
        public bool isDefault = false;                                          // 是否默认Fragment (代码一开始会产生一个默认的Fragment)
        public List<X64Record> records;                                         // record列表
        public int currRecordIndex = -1;                                        // 当前的 record 索引，如果当前的值为-1，则需要新建一个

        public X64Fragment(string secName, uint align = 1)
        {
            sectionName = secName;
            this.align = align;
            records = new List<X64Record>();
            currRecordIndex = -1;
        }

        public int GetCurrRecordtIndex()
        {
            if (currRecordIndex < 0) {
                X64Record newRecord = new X64Record();
                records.Add(newRecord);
                currRecordIndex = records.Count - 1;
            }
            return currRecordIndex;
        }

        public X64Record AddNewRecord()
        {
            X64Record x64Record = new X64Record();
            records.Add(x64Record);
            currRecordIndex = records.Count - 1;
            return x64Record;
        }

        public X64Record GetCurrRecord()
        {
            return records[GetCurrRecordtIndex()];
        }


        public void EndCurrRecord()
        {
            currRecordIndex = -1;
        }
    }

    public class X64FragmentList
    {
        public long currFragmentIndex = -1;
        public LargeList<X64Fragment> fragments;
        WeakReference weakAssembler;

        public X64FragmentList(X64Assembler asm)
        {
            weakAssembler = new WeakReference(asm);
            fragments = new LargeList<X64Fragment>();
            currFragmentIndex = -1;
        }

        public long AddNewFragment(string sectionName)
        {
            fragments.Add(new X64Fragment(sectionName));
            currFragmentIndex = (long)(fragments.Count - 1);
            return currFragmentIndex;
        }

        public long GetCurrFragmentIndex()
        {
            if (currFragmentIndex < 0) {
                X64Fragment x64Fragment = new X64Fragment(".text");
                fragments.Add(x64Fragment);
                currFragmentIndex = (long)(fragments.Count - 1);
            }
            return currFragmentIndex;
        }

        public X64Fragment? GetCurrFragment()
        {
            long idx = GetCurrFragmentIndex();
            if (idx >= 0 && (ulong)idx < fragments.Count) {
                return fragments[(ulong)idx];
            }
            return null;
        }
    }

    public class X64SymbolList
    {
        public LargeList<X64Symbol> symbols;
        public Hashtable htSymbol;

        public X64SymbolList()
        {
            symbols = new LargeList<X64Symbol>();
            htSymbol = new Hashtable();
        }

        public long AddSymbol(X64Symbol symbol)
        {
            ulong idx = 0;
            if (htSymbol.ContainsKey(symbol.symbolName)) {
                return -1;
            }
            symbols.Add(symbol);
            idx = symbols.Count - 1;
            htSymbol.Add(symbol.symbolName, idx);
            return (long)idx;
        }

        public X64Symbol? GetSymbol(string name)
        {
            if (htSymbol.ContainsKey(name)) {
                return null;
            }
            var value1 = htSymbol[name];
            if (value1 == null) {
                return null;
            }
            ulong idx = (ulong)value1;
            return symbols[idx];
        }

    }

}





