
using AlloyTools.utils;
using System.Collections;

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
        public ulong maxSize;                                                   // 可变记录最大的可能大小 (仅仅 recordType == Variable 时有效)
        public ulong fragmentIndex;                                             // 属于哪个 Fragment/Proc
    }

    public class X64Fragment
    {
        public string sectionName = "";                                         // 属于哪个 section，默认情况下,代码段是 ".text"，数据段是 ".data"
        public uint align = 1;                                                  // 对齐
        public ulong startRecordIdx = 0;                                        // 首个 record 在 records 列表中的第几个
        public ulong endRecordIdx = 0;                                          // 最后一个 record 在 records 列表中的第几个
        public bool hasRecord = false;                                          // 此 Fragment 是否有 record

        public X64Fragment(string secName, uint align = 1)
        {
            sectionName = secName;
            this.align = align;
        }

        public ulong AddNewRecord(ulong indexOfRecordList)
        {
            if (hasRecord) {
                endRecordIdx = indexOfRecordList;
            }
            else {
                hasRecord = true;
                startRecordIdx = indexOfRecordList;
                endRecordIdx = indexOfRecordList;
            }
            return indexOfRecordList;
        }
    }

    public class X64FragmentList
    {
        public long currFragmentIndex;
        public static LargeList<X64Fragment> fragments;

        static X64FragmentList()
        {
            fragments = new LargeList<X64Fragment>();
        }

        public static ulong AddFragment(string sectionName)
        {
            fragments.Add(new X64Fragment(sectionName));
            return fragments.Count - 1;
        }

        public static ulong getCurrFragmentIndex()
        {
            if (fragments.Count == 0) {
                return AddFragment(".text");
            }
            return fragments.Count  - 1;
        }

        public static X64Fragment GetCurrFragment()
        {
            ulong idx = getCurrFragmentIndex();
            return fragments[idx];
        }
    }

    public class X64RecordList
    {
        public static LargeList<X64Record> records;
        public static long currRecordIndex;                             // 当前的 record 索引，如果当前的值为-1，则需要新建一个

        static X64RecordList()
        {
            records = new LargeList<X64Record>();
            currRecordIndex = -1;
        }

        public static ulong getCurrRecordtIndex()
        {
            if (currRecordIndex < 0) {
                X64Record newRecord = new X64Record();
                records.Add(newRecord);
                X64Fragment currFragment = X64FragmentList.GetCurrFragment();
                currFragment.AddNewRecord(records.Count - 1);
                currRecordIndex = (long)(records.Count - 1);
            }
            return (ulong)currRecordIndex;
        }

        public static void endCurrRecord()
        {
            currRecordIndex = -1;
        }
    }

    public class X64SymbolList
    {
        public static LargeList<X64Symbol> symbols;
        public static Hashtable htSymbol;

        static X64SymbolList()
        {
            symbols = new LargeList<X64Symbol>();
            htSymbol = new Hashtable();
        }

        public static long AddSymbol(X64Symbol symbol)
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

        public static X64Symbol? GetSymbol(string name)
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





