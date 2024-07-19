using System;

namespace AlloyTools.Assembler.AMD64
{
    public enum X64OperandType
    {
        Unknown = 0,                    // 未知类型
        Register,                       // 寄存器
        MemoryAddress,                  // 内存寻址
        Symbol,                         // 符号
        ImmediateValue,                 // 立即数(已经转换为ulong,无论整数负数都转换为ulong)
        ImmediateRaw,                   // 立即数(保留原始字符串表达,需要时再转换,通常是浮点数或者是超大整数)
        ImmediateString,                // 立即数(字符串-用引号括起来的字符串)
        MemoryAddressInfo,              // 内存寻址(寻址信息还没有完全，只能做中间值)
    }

    [Flags]
    public enum X64RegValue : uint
    {
        None = 0,
        //
        REG_VALUE_0 = 0,
        REG_VALUE_1 = 1,
        REG_VALUE_2 = 2,
        REG_VALUE_3 = 3,
        REG_VALUE_4 = 4,
        REG_VALUE_5 = 5,
        REG_VALUE_6 = 6,
        REG_VALUE_7 = 7,
        REG_VALUE_8 = 8,
        REG_VALUE_9 = 9,
        REG_VALUE_10 = 10,
        REG_VALUE_11 = 11,
        REG_VALUE_12 = 12,
        REG_VALUE_13 = 13,
        REG_VALUE_14 = 14,
        REG_VALUE_15 = 15,
        REG_VALUE_16 = 16,
        REG_VALUE_17 = 17,
        REG_VALUE_18 = 18,
        REG_VALUE_19 = 19,
        REG_VALUE_20 = 20,
        REG_VALUE_21 = 21,
        REG_VALUE_22 = 22,
        REG_VALUE_23 = 23,
        REG_VALUE_24 = 24,
        REG_VALUE_25 = 25,
        REG_VALUE_26 = 26,
        REG_VALUE_27 = 27,
        REG_VALUE_28 = 28,
        REG_VALUE_29 = 29,
        REG_VALUE_30 = 30,
        REG_VALUE_31 = 31,
        //
        Common = 0x100,                             // 通用寄存器标志
        REG_8bit = 0x200,                           // 8位标志一般在opcode中
        REG_16bit = 0x400,                          // 0x66扩展（opcode前加0x66前缀）
        REG_32bit = 0x800,                          // 
        REG_64bit = 0x1000,
        //
        EAX = Common | REG_32bit | REG_VALUE_0,
        ECX = Common | REG_32bit | REG_VALUE_1,
        EDX = Common | REG_32bit | REG_VALUE_2,
        EBX = Common | REG_32bit | REG_VALUE_3,
        ESP = Common | REG_32bit | REG_VALUE_4,
        EBP = Common | REG_32bit | REG_VALUE_5,
        ESI = Common | REG_32bit | REG_VALUE_6,
        EDI = Common | REG_32bit | REG_VALUE_7,
        //
        AL = Common | REG_8bit | REG_VALUE_0,
        CL = Common | REG_8bit | REG_VALUE_1,
        DL = Common | REG_8bit | REG_VALUE_2,
        BL = Common | REG_8bit | REG_VALUE_3,
        AH = Common | REG_8bit | REG_VALUE_4,
        CH = Common | REG_8bit | REG_VALUE_5,
        DH = Common | REG_8bit | REG_VALUE_6,
        BH = Common | REG_8bit | REG_VALUE_7,
        //
        AX = Common | REG_16bit | REG_VALUE_0,
        CX = Common | REG_16bit | REG_VALUE_1,
        DX = Common | REG_16bit | REG_VALUE_2,
        BX = Common | REG_16bit | REG_VALUE_3,
        SP = Common | REG_16bit | REG_VALUE_4,
        BP = Common | REG_16bit | REG_VALUE_5,
        SI = Common | REG_16bit | REG_VALUE_6,
        DI = Common | REG_16bit | REG_VALUE_7,
        //
        REXPreflx_40 = 0x2000,                      // REX的0x40标志
        REXPreflx_W = REXPreflx_40 | 0x4000,        // REX.W 标志 （扩展宽度为64位标志）
        RAX = Common | REG_64bit | REXPreflx_W | REG_VALUE_0,
        RCX = Common | REG_64bit | REXPreflx_W | REG_VALUE_1,
        RDX = Common | REG_64bit | REXPreflx_W | REG_VALUE_2,
        RBX = Common | REG_64bit | REXPreflx_W | REG_VALUE_3,
        RSP = Common | REG_64bit | REXPreflx_W | REG_VALUE_4,
        RBP = Common | REG_64bit | REXPreflx_W | REG_VALUE_5,
        RSI = Common | REG_64bit | REXPreflx_W | REG_VALUE_6,
        RDI = Common | REG_64bit | REXPreflx_W | REG_VALUE_7,
        // SP、BP、SI、DI 的低8位寄存器使用时需要加 REX:0x40 扩展，不加就是 AH、CH、DH、BH
        SPL = Common | REG_8bit | REXPreflx_40 | REG_VALUE_4,
        BPL = Common | REG_8bit | REXPreflx_40 | REG_VALUE_5,
        SIL = Common | REG_8bit | REXPreflx_40 | REG_VALUE_6,
        DIL = Common | REG_8bit | REXPreflx_40 | REG_VALUE_7,
        //
        REXPreflx_E = REXPreflx_40 | 0x8000,        // 需要扩展REX.R / REX.X / REX.B (reg、r/m、SIB域) 、扩展到8~15号寄存器
        //
        R8B = Common | REG_8bit | REXPreflx_E | REG_VALUE_8,
        R9B = Common | REG_8bit | REXPreflx_E | REG_VALUE_9,
        R10B = Common | REG_8bit | REXPreflx_E | REG_VALUE_10,
        R11B = Common | REG_8bit | REXPreflx_E | REG_VALUE_11,
        R12B = Common | REG_8bit | REXPreflx_E | REG_VALUE_12,
        R13B = Common | REG_8bit | REXPreflx_E | REG_VALUE_13,
        R14B = Common | REG_8bit | REXPreflx_E | REG_VALUE_14,
        R15B = Common | REG_8bit | REXPreflx_E | REG_VALUE_15,
        //
        R8W = Common | REG_16bit | REXPreflx_E | REG_VALUE_8,
        R9W = Common | REG_16bit | REXPreflx_E | REG_VALUE_9,
        R10W = Common | REG_16bit | REXPreflx_E | REG_VALUE_10,
        R11W = Common | REG_16bit | REXPreflx_E | REG_VALUE_11,
        R12W = Common | REG_16bit | REXPreflx_E | REG_VALUE_12,
        R13W = Common | REG_16bit | REXPreflx_E | REG_VALUE_13,
        R14W = Common | REG_16bit | REXPreflx_E | REG_VALUE_14,
        R15W = Common | REG_16bit | REXPreflx_E | REG_VALUE_15,
        //
        R8D = Common | REG_32bit | REXPreflx_E | REG_VALUE_8,
        R9D = Common | REG_32bit | REXPreflx_E | REG_VALUE_9,
        R10D = Common | REG_32bit | REXPreflx_E | REG_VALUE_10,
        R11D = Common | REG_32bit | REXPreflx_E | REG_VALUE_11,
        R12D = Common | REG_32bit | REXPreflx_E | REG_VALUE_12,
        R13D = Common | REG_32bit | REXPreflx_E | REG_VALUE_13,
        R14D = Common | REG_32bit | REXPreflx_E | REG_VALUE_14,
        R15D = Common | REG_32bit | REXPreflx_E | REG_VALUE_15,
        //
        R8 = Common | REG_64bit | REXPreflx_W | REXPreflx_E | REG_VALUE_8,
        R9 = Common | REG_64bit | REXPreflx_W | REXPreflx_E | REG_VALUE_9,
        R10 = Common | REG_64bit | REXPreflx_W | REXPreflx_E | REG_VALUE_10,
        R11 = Common | REG_64bit | REXPreflx_W | REXPreflx_E | REG_VALUE_11,
        R12 = Common | REG_64bit | REXPreflx_W | REXPreflx_E | REG_VALUE_12,
        R13 = Common | REG_64bit | REXPreflx_W | REXPreflx_E | REG_VALUE_13,
        R14 = Common | REG_64bit | REXPreflx_W | REXPreflx_E | REG_VALUE_14,
        R15 = Common | REG_64bit | REXPreflx_W | REXPreflx_E | REG_VALUE_15,
        //
        SegmentReg = 0x10000,                       // 段寄存器标志
        CtrlReg = 0x20000,                          // 控制寄存器(CR0~CR15)
        DebugReg = 0x40000,                         // 调试寄存器(DR0~DR15)
        X87Reg = 0x80000,                           // 浮点寄存器标志（ST0~ST7）
        MMXReg = 0x100000,                          // MMX寄存器（MM0~MM7，实际是ST0~ST7的子集）
        XMMReg = 0x200000,                          // XMM寄存器 SSE (128-bit) (XMM0~XMM31)
        YMMReg = 0x400000,                          // YMM寄存器 AVX-2 (256-bit) (YMM0~YMM31)
        ZMMReg = 0x800000,                          // ZMM寄存器 AVX-512 (512-bit) (ZMM0~ZMM31)
        TMMReg = 0x1000000,                         // TMM寄存器 (一个寄存器1KB大小的寄存器) (TMM0~TMM8)
        SSEVEX = 0x2000000,                         // SSE寄存器扩展（可以使用 XMM15~XMM31, YMM15~YMM31, ZMM15~ZMM31）
        //
        ES = SegmentReg | REG_VALUE_0,
        CS = SegmentReg | REG_VALUE_1,
        SS = SegmentReg | REG_VALUE_2,
        DS = SegmentReg | REG_VALUE_3,
        FS = SegmentReg | REG_VALUE_4,
        GS = SegmentReg | REG_VALUE_5,
        //
        CR0 = CtrlReg | REG_VALUE_0,
        CR1 = CtrlReg | REG_VALUE_1,
        CR2 = CtrlReg | REG_VALUE_2,
        CR3 = CtrlReg | REG_VALUE_3,
        CR4 = CtrlReg | REG_VALUE_4,
        CR5 = CtrlReg | REG_VALUE_5,
        CR6 = CtrlReg | REG_VALUE_6,
        CR7 = CtrlReg | REG_VALUE_7,
        CR8 = CtrlReg | REXPreflx_E | REG_VALUE_8,
        CR9 = CtrlReg | REXPreflx_E | REG_VALUE_9,
        CR10 = CtrlReg | REXPreflx_E | REG_VALUE_10,
        CR11 = CtrlReg | REXPreflx_E | REG_VALUE_11,
        CR12 = CtrlReg | REXPreflx_E | REG_VALUE_12,
        CR13 = CtrlReg | REXPreflx_E | REG_VALUE_13,
        CR14 = CtrlReg | REXPreflx_E | REG_VALUE_14,
        CR15 = CtrlReg | REXPreflx_E | REG_VALUE_15,
        //
        DR0 = DebugReg | REG_VALUE_0,
        DR1 = DebugReg | REG_VALUE_1,
        DR2 = DebugReg | REG_VALUE_2,
        DR3 = DebugReg | REG_VALUE_3,
        DR4 = DebugReg | REG_VALUE_4,
        DR5 = DebugReg | REG_VALUE_5,
        DR6 = DebugReg | REG_VALUE_6,
        DR7 = DebugReg | REG_VALUE_7,
        DR8 = DebugReg | REXPreflx_E | REG_VALUE_8,
        DR9 = DebugReg | REXPreflx_E | REG_VALUE_9,
        DR10 = DebugReg | REXPreflx_E | REG_VALUE_10,
        DR11 = DebugReg | REXPreflx_E | REG_VALUE_11,
        DR12 = DebugReg | REXPreflx_E | REG_VALUE_12,
        DR13 = DebugReg | REXPreflx_E | REG_VALUE_13,
        DR14 = DebugReg | REXPreflx_E | REG_VALUE_14,
        DR15 = DebugReg | REXPreflx_E | REG_VALUE_15,
        //
        ST0 = X87Reg | REG_VALUE_0,
        ST1 = X87Reg | REG_VALUE_1,
        ST2 = X87Reg | REG_VALUE_2,
        ST3 = X87Reg | REG_VALUE_3,
        ST4 = X87Reg | REG_VALUE_4,
        ST5 = X87Reg | REG_VALUE_5,
        ST6 = X87Reg | REG_VALUE_6,
        ST7 = X87Reg | REG_VALUE_7,
        //
        MM0 = MMXReg | REG_VALUE_0,
        MM1 = MMXReg | REG_VALUE_1,
        MM2 = MMXReg | REG_VALUE_2,
        MM3 = MMXReg | REG_VALUE_3,
        MM4 = MMXReg | REG_VALUE_4,
        MM5 = MMXReg | REG_VALUE_5,
        MM6 = MMXReg | REG_VALUE_6,
        MM7 = MMXReg | REG_VALUE_7,
        //
        XMM0 = XMMReg | REG_VALUE_0,
        XMM1 = XMMReg | REG_VALUE_1,
        XMM2 = XMMReg | REG_VALUE_2,
        XMM3 = XMMReg | REG_VALUE_3,
        XMM4 = XMMReg | REG_VALUE_4,
        XMM5 = XMMReg | REG_VALUE_5,
        XMM6 = XMMReg | REG_VALUE_6,
        XMM7 = XMMReg | REG_VALUE_7,
        XMM8 = XMMReg | REXPreflx_E | REG_VALUE_8,
        XMM9 = XMMReg | REXPreflx_E | REG_VALUE_9,
        XMM10 = XMMReg | REXPreflx_E | REG_VALUE_10,
        XMM11 = XMMReg | REXPreflx_E | REG_VALUE_11,
        XMM12 = XMMReg | REXPreflx_E | REG_VALUE_12,
        XMM13 = XMMReg | REXPreflx_E | REG_VALUE_13,
        XMM14 = XMMReg | REXPreflx_E | REG_VALUE_14,
        XMM15 = XMMReg | REXPreflx_E | REG_VALUE_15,
        XMM16 = XMMReg | SSEVEX | REG_VALUE_16,
        XMM17 = XMMReg | SSEVEX | REG_VALUE_17,
        XMM18 = XMMReg | SSEVEX | REG_VALUE_18,
        XMM19 = XMMReg | SSEVEX | REG_VALUE_19,
        XMM20 = XMMReg | SSEVEX | REG_VALUE_20,
        XMM21 = XMMReg | SSEVEX | REG_VALUE_21,
        XMM22 = XMMReg | SSEVEX | REG_VALUE_22,
        XMM23 = XMMReg | SSEVEX | REG_VALUE_23,
        XMM24 = XMMReg | SSEVEX | REG_VALUE_24,
        XMM25 = XMMReg | SSEVEX | REG_VALUE_25,
        XMM26 = XMMReg | SSEVEX | REG_VALUE_26,
        XMM27 = XMMReg | SSEVEX | REG_VALUE_27,
        XMM28 = XMMReg | SSEVEX | REG_VALUE_28,
        XMM29 = XMMReg | SSEVEX | REG_VALUE_29,
        XMM30 = XMMReg | SSEVEX | REG_VALUE_30,
        XMM31 = XMMReg | SSEVEX | REG_VALUE_31,
        //
        YMM0 = YMMReg | REG_VALUE_0,
        YMM1 = YMMReg | REG_VALUE_1,
        YMM2 = YMMReg | REG_VALUE_2,
        YMM3 = YMMReg | REG_VALUE_3,
        YMM4 = YMMReg | REG_VALUE_4,
        YMM5 = YMMReg | REG_VALUE_5,
        YMM6 = YMMReg | REG_VALUE_6,
        YMM7 = YMMReg | REG_VALUE_7,
        YMM8 = YMMReg | REG_VALUE_8,
        YMM9 = YMMReg | REG_VALUE_9,
        YMM10 = YMMReg | REG_VALUE_10,
        YMM11 = YMMReg | REG_VALUE_11,
        YMM12 = YMMReg | REG_VALUE_12,
        YMM13 = YMMReg | REG_VALUE_13,
        YMM14 = YMMReg | REG_VALUE_14,
        YMM15 = YMMReg | REG_VALUE_15,
        YMM16 = YMMReg | REG_VALUE_16,
        YMM17 = YMMReg | REG_VALUE_17,
        YMM18 = YMMReg | REG_VALUE_18,
        YMM19 = YMMReg | REG_VALUE_19,
        YMM20 = YMMReg | REG_VALUE_20,
        YMM21 = YMMReg | REG_VALUE_21,
        YMM22 = YMMReg | REG_VALUE_22,
        YMM23 = YMMReg | REG_VALUE_23,
        YMM24 = YMMReg | REG_VALUE_24,
        YMM25 = YMMReg | REG_VALUE_25,
        YMM26 = YMMReg | REG_VALUE_26,
        YMM27 = YMMReg | REG_VALUE_27,
        YMM28 = YMMReg | REG_VALUE_28,
        YMM29 = YMMReg | REG_VALUE_29,
        YMM30 = YMMReg | REG_VALUE_30,
        YMM31 = YMMReg | REG_VALUE_31,
        //
        ZMM0 = ZMMReg | REG_VALUE_0,
        ZMM1 = ZMMReg | REG_VALUE_1,
        ZMM2 = ZMMReg | REG_VALUE_2,
        ZMM3 = ZMMReg | REG_VALUE_3,
        ZMM4 = ZMMReg | REG_VALUE_4,
        ZMM5 = ZMMReg | REG_VALUE_5,
        ZMM6 = ZMMReg | REG_VALUE_6,
        ZMM7 = ZMMReg | REG_VALUE_7,
        ZMM8 = ZMMReg | REG_VALUE_8,
        ZMM9 = ZMMReg | REG_VALUE_9,
        ZMM10 = ZMMReg | REG_VALUE_10,
        ZMM11 = ZMMReg | REG_VALUE_11,
        ZMM12 = ZMMReg | REG_VALUE_12,
        ZMM13 = ZMMReg | REG_VALUE_13,
        ZMM14 = ZMMReg | REG_VALUE_14,
        ZMM15 = ZMMReg | REG_VALUE_15,
        ZMM16 = ZMMReg | REG_VALUE_16,
        ZMM17 = ZMMReg | REG_VALUE_17,
        ZMM18 = ZMMReg | REG_VALUE_18,
        ZMM19 = ZMMReg | REG_VALUE_19,
        ZMM20 = ZMMReg | REG_VALUE_20,
        ZMM21 = ZMMReg | REG_VALUE_21,
        ZMM22 = ZMMReg | REG_VALUE_22,
        ZMM23 = ZMMReg | REG_VALUE_23,
        ZMM24 = ZMMReg | REG_VALUE_24,
        ZMM25 = ZMMReg | REG_VALUE_25,
        ZMM26 = ZMMReg | REG_VALUE_26,
        ZMM27 = ZMMReg | REG_VALUE_27,
        ZMM28 = ZMMReg | REG_VALUE_28,
        ZMM29 = ZMMReg | REG_VALUE_29,
        ZMM30 = ZMMReg | REG_VALUE_30,
        ZMM31 = ZMMReg | REG_VALUE_31,
        //
        TMM0 = TMMReg | REG_VALUE_0,
        TMM1 = TMMReg | REG_VALUE_1,
        TMM2 = TMMReg | REG_VALUE_2,
        TMM3 = TMMReg | REG_VALUE_3,
        TMM4 = TMMReg | REG_VALUE_4,
        TMM5 = TMMReg | REG_VALUE_5,
        TMM6 = TMMReg | REG_VALUE_6,
        TMM7 = TMMReg | REG_VALUE_7,
        //
        
    }

    public class X64RegUtil
    {
        public static bool Is32BitReg(X64RegValue reg)
        {
            return reg.HasFlag(X64RegValue.REG_32bit) && reg.HasFlag(X64RegValue.Common);
        }

        public static bool Is64BitReg(X64RegValue reg)
        {
            return reg.HasFlag(X64RegValue.REG_64bit) && reg.HasFlag(X64RegValue.Common);
        }

        public static bool Is32Or64BitReg(X64RegValue reg)
        {
            return (reg.HasFlag(X64RegValue.REG_64bit) || reg.HasFlag(X64RegValue.REG_32bit)) && reg.HasFlag(X64RegValue.Common); 
        }
    }

    public enum SymbolModifier
    {
        None = 0,                   // 默认情况下，符号会产生 REL32+n 重定位（RIP相对寻址,相对下一条指令的地址）
        Offset = 1,                 // 用 offset 修饰 （取地址，产生 ADDR64 重定位）
        ImageRel = 2,               // 用 imagerel 修饰 （取RVA地址，产生 REL32NB 重定位）
        Addr32 = 3,                 // 用 addr32 修饰 （取地址，产生 ADDR32 重定位）, 例如: mov rbx, [addr32 labelSymbol]
        Addr64 = 4,                 // 用 addr64 修饰 （用64位寻址，产生 ADDR64 重定位)
                                    // 注：只能有 mov al/ax/eax/rax, [addr64 labelSymbol] 和 mov [addr64 labelSymbol], al/ax/eax/rax）两种指令,只能R0，不能其他寄存器,R8也不行 
    }

    [Flags]
    public enum MemoryAddressModifier
    {
        None = 0,
        BytePtr = 1,                // 用 byte ptr 修饰寻址
        WordPtr = 2,                // 用 word ptr 修饰寻址
        DWordPtr = 4,               // 用 dword ptr 修饰寻址
        QWordPtr = 8,               // 用 qword ptr 修饰寻址
        MmWord = 0x10,              // 用 mmword ptr 修饰寻址(同qword ptr)
        XmmWordPtr = 0x20,          // 用 xmmword ptr 修饰寻址
        YmmWordPtr = 0x40,          // 用 ymmword ptr 修饰寻址
        Addr32 = 0x10000000,        // 用 Addr32 修饰过的符号来寻址（32位截断绝对地址寻址）
        Addr64 = 0x20000000,        // 用 Addr64 修饰过的符号来寻址（64位绝对地址寻址）
    }

    [Flags]
    public enum MemoryAddressType
    {
        None = 0,
        // 源码层面的信息
        hasReg1 = 0x01,                 // 有寄存器1
        hasReg2 = 0x02,                 // 有寄存器2 （当有两个寄存器的时候，一定是SIB基址加变址，这时也一定有scale比例因子，隐藏的因子为1）
        hasExplicitScale = 0x04,        // 源码中有显式的比例因子(如果有显式的比例因子，则reg1和reg2不能互相调换基址寄存器和变址寄存器来适应一些特殊寄存器要求)
        hasDisp = 0x08,                 // 是否有数值上的偏移量
        hasSymbol = 0x10,               // 是否由符号来寻址(由符号来决定偏移量)
        // 机器层面
        withModRM = 0x1000,             // 此项其实一定有(除了 with64bitAbsAddr之外)
        withSIB = 0x2000,
        withDisp8 = 0x4000,
        withDisp32 = 0x8000,
        withSegment = 0x10000,          // 带有段前缀
        withNumericDisp = 0x20000,      // 源码层面带有数值上的偏移量
        withSymbol = 0x40000,           // 源码层面带有符号上的偏移量 (如果此项目有，则 withDisp32 或 with64bitAbsAddr 一定有其一)
        with32bitRegAddr = 0x80000,     // 使用了32位寄存器来寻址(如果此项目有，则要加0x67前缀)
        with32bitImmBase = 0x100000,    // 使用32位的无符号立即数做基址(此时 withSIB 一定有，withDisp32 一定有)
                                        // 注：规定rbp/r13做基址时必须带偏移量，rsp禁止做变址（rsp做变址表示没有变址也没有比例因子）
                                        //    所以如果mod==00，并且base==rbp/r13, index==rsp时，表示使用一个无符号的32位数值做基地址(这时可能会产生ADDR32重定位)
        with64bitAbsAddr = 0x200000,    // 使用64位绝对地址来寻址，
    }

    // 源码层面的寻址信息
    public class MemoryAddressInfo
    {
        public MemoryAddressType type;          // 寻址类型
        public X64RegValue reg1;                // 寄存器1
        public X64RegValue reg2;                // 寄存器2
        public byte scale;                      // 比例因子
        public ulong disp32;                    // 内部用ulong以方便表达式计算，实际上是需要转回int产生机器码
        // public X64RegValue seg;              // 段前缀 // 此标志注释掉，段前缀不应该放在源码层面，就是[]中括号内不应该有段前缀，段前缀应该放[]前面，例如 fs:[rbx]
        public string symName = "";
        public ulong symIndex;
    }

    // 机器层面的寻址信息
    public class MemoryAddressResult
    {
        public MemoryAddressModifier modifier;  // 寻址目标大小修饰
        public MemoryAddressType type;          // 寻址类型
        public byte[] code;                     // 寻址产生的机器码
        public int codeSize;                    // 机器码长度
        public int relocOffset;                 // 需要重定位时，重定位位置位于本codebyte数组中的偏移
        public X64RegValue indirectReg;         // 间接寻址寄存器(寄存器间接寻址是mod==00, 寄存器间接寻址不能是rsp/r12,带rsp/r12的必须转变为基址+变址寻址)
        public ulong disp32;                    // 偏移量(mod==01为带8位偏移量, mod==10为带32位偏移量, mod=11为直接表示寄存器本身) (内部用ulong以方便表达式计算，实际上是需要转回int产生机器码)
        public X64RegValue baseReg;             // 基址寄存器信息(rm==100时，才有SIB字节) [rsp 寄存器不能做为 index 寄存器，只能做 base 寄存器, rsp做index表示没有变址, 而r12却可以做index]
        public X64RegValue indexReg;            // 变址寄存器信息(rm==100时，才有SIB字节)
        public byte sacle;                      // 比例因子(rm==100时，才有SIB字节)
        public string symName;                  // 符号寻址(规定rbp/r13必须带偏移量,如果mod==00,rm=101时表示直接用一个32位数值来寻址,这里一般是指符号地址)
        public ulong symIndex;                  // 符号在符号列表的索引(为0表示找不到)
                                                // 另一种情况，mod==00，并且base==rbp/r13, index==rsp时，表示使用一个无符号的32位绝对数值(可以是变量符号)做基地址进行ADDR32寻址
        public X64RegValue segReg;              // 段前缀用的段寄存器

        public MemoryAddressResult()
        {
            modifier = MemoryAddressModifier.None;
            type = MemoryAddressType.None;
            code = new byte[8];
            codeSize = 0;
            relocOffset = 0;

            symName = "";
        }
    }

    public class X64Operand
    {
        public X64OperandType type;
        public X64RegValue regValue;            // type 为 Register 时有效
        public ulong ulongValue;                // type 为 ImmediateValue 时有效
        public byte[]? bstr;                    // type 为 ImmediateString 时有效
        public ulong symIndex;                  // type 为 Symbol 时有效,在符号列表的索引(为0表示找不到)
        public string str;                      // type 为 Symbol 或 ImmediateRaw 时有效,存符号字符串,或者立即数的字符串表达
        public MemoryAddressInfo? addressInfo;  // type 为 MemoryAddressInfo时有效（为内存寻址的中间值，以 [ 开头产生此类型值）
        public MemoryAddressResult? addressRes; // type 为 MemoryAddress 时有效（为内存寻址结果值，以 ] 结尾则产生此类型值）

        public X64Operand(ulong newValue)
        {
            type = X64OperandType.ImmediateValue;
            ulongValue = newValue;
            regValue = X64RegValue.None;
            bstr = null;
            symIndex = 0;
            str = "";
            addressInfo = null;
            addressRes = null;
        }

        public X64Operand(X64RegValue newValue)
        {
            type = X64OperandType.Register;
            regValue = newValue;
            ulongValue = 0;
            bstr = null;
            symIndex = 0;
            str = "";
            addressInfo = null;
            addressRes = null;
        }

        public X64Operand(byte[] rawStr)
        {
            type = X64OperandType.ImmediateString;
            regValue = X64RegValue.None;
            ulongValue = 0;
            bstr = rawStr;
            symIndex = 0;
            str = "";
            addressInfo = null;
            addressRes = null;
        }

        public X64Operand(X64OperandType type1, string str1)
        {
            type = type1;
            regValue = X64RegValue.None;
            ulongValue = 0;
            bstr = null;
            symIndex = 0;
            str = str1;
            addressInfo = null;
            addressRes = null;
        }

        public X64Operand(X64OperandType type1, string str1, ulong symIndex1)
        {
            type = type1;
            regValue = X64RegValue.None;
            ulongValue = 0;
            bstr = null;
            symIndex = symIndex1;
            str = str1;
            addressInfo = null;
            addressRes = null;
        }

        public X64Operand(MemoryAddressInfo info)
        {
            type = X64OperandType.MemoryAddressInfo;
            ulongValue = 0;
            regValue = X64RegValue.None;
            bstr = null;
            symIndex = 0;
            str = "";
            addressInfo = info;
            addressRes = null;
        }

        public X64Operand(MemoryAddressResult res)
        {
            type = X64OperandType.MemoryAddressInfo;
            ulongValue = 0;
            regValue = X64RegValue.None;
            bstr = null;
            symIndex = 0;
            str = "";
            addressInfo = null;
            addressRes = res;
        }
    }
}



