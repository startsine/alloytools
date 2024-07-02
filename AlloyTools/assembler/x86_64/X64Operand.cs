using System;

namespace AlloyTools.Assembler.AMD64
{
    public enum X64OperandType
    {
        Unknown = 0,                    // 未知类型
        Immediate,                      // 立即数
        Register,                       // 寄存器
        MemoryAddress,                  // 内存寻址
    }

    [Flags]
    public enum X64RegValue: ulong
    {
        None = 0x8000000000000000,
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
        //
        Common = 0x100,                             // 通用寄存器标志
        REG_8bit = 0x1000,                          // 8位标志一般在opcode中
        REG_16bit = 0x2000,                         // 0x66扩展（opcode前加0x66前缀）
        REG_32bit = 0x4000,                         //
        REG_64bit = 0x8000,
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
        REXPreflx_40 = 0x10000,                     // REX的0x40标志
        REXPreflx_W = REXPreflx_40 | 0x20000,       // REX.W 标志 （扩展宽度为64位标志）
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
        REXPreflx_E = REXPreflx_40 | 0x40000,       // 需要扩展REX.R / REX.X / REX.B (reg、r/m、SIB域) 、扩展到8~15号寄存器
        //
        R8B =  Common | REG_8bit | REXPreflx_E | REG_VALUE_0,
        R9B =  Common | REG_8bit | REXPreflx_E | REG_VALUE_1,
        R10B = Common | REG_8bit | REXPreflx_E | REG_VALUE_2,
        R11B = Common | REG_8bit | REXPreflx_E | REG_VALUE_3,
        R12B = Common | REG_8bit | REXPreflx_E | REG_VALUE_4,
        R13B = Common | REG_8bit | REXPreflx_E | REG_VALUE_5,
        R14B = Common | REG_8bit | REXPreflx_E | REG_VALUE_6,
        R15B = Common | REG_8bit | REXPreflx_E | REG_VALUE_7,
        //
        R8W =  Common | REG_16bit | REXPreflx_E | REG_VALUE_0,
        R9W =  Common | REG_16bit | REXPreflx_E | REG_VALUE_1,
        R10W = Common | REG_16bit | REXPreflx_E | REG_VALUE_2,
        R11W = Common | REG_16bit | REXPreflx_E | REG_VALUE_3,
        R12W = Common | REG_16bit | REXPreflx_E | REG_VALUE_4,
        R13W = Common | REG_16bit | REXPreflx_E | REG_VALUE_5,
        R14W = Common | REG_16bit | REXPreflx_E | REG_VALUE_6,
        R15W = Common | REG_16bit | REXPreflx_E | REG_VALUE_7,
        //
        R8D =  Common | REG_32bit | REXPreflx_E | REG_VALUE_0,
        R9D =  Common | REG_32bit | REXPreflx_E | REG_VALUE_1,
        R10D = Common | REG_32bit | REXPreflx_E | REG_VALUE_2,
        R11D = Common | REG_32bit | REXPreflx_E | REG_VALUE_3,
        R12D = Common | REG_32bit | REXPreflx_E | REG_VALUE_4,
        R13D = Common | REG_32bit | REXPreflx_E | REG_VALUE_5,
        R14D = Common | REG_32bit | REXPreflx_E | REG_VALUE_6,
        R15D = Common | REG_32bit | REXPreflx_E | REG_VALUE_7,
        //
        R8 =  Common | REG_64bit | REXPreflx_W | REXPreflx_E | REG_VALUE_0,
        R9 =  Common | REG_64bit | REXPreflx_W | REXPreflx_E | REG_VALUE_1,
        R10 = Common | REG_64bit | REXPreflx_W | REXPreflx_E | REG_VALUE_2,
        R11 = Common | REG_64bit | REXPreflx_W | REXPreflx_E | REG_VALUE_3,
        R12 = Common | REG_64bit | REXPreflx_W | REXPreflx_E | REG_VALUE_4,
        R13 = Common | REG_64bit | REXPreflx_W | REXPreflx_E | REG_VALUE_5,
        R14 = Common | REG_64bit | REXPreflx_W | REXPreflx_E | REG_VALUE_6,
        R15 = Common | REG_64bit | REXPreflx_W | REXPreflx_E | REG_VALUE_7,
    }

    public class X64Operand
    {
        public X64OperandType Type;
        public ulong IntValue;

        public X64Operand() { }
    }
}



