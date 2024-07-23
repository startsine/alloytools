


using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Xml.Linq;

namespace AlloyTools.Assembler.AMD64
{
    public enum MatchType
    {
        None = 0,
        imm,                            // 操作数是立即数
        acc,                            // 操作数是AL,AX,EAX,RAX
        reg,                            // 操作数是一个通用寄存器
        rm,                             // 操作数是寄存器或者内存寻址
        //moffset32,                      // 操作数是内存寻址，用[imm]寻址的
        moffset64,                      // 操作数是内存寻址，用[imm64]寻址的
        segReg,                         // 操作数是段寄存器
        debugReg,                       // 操作数是DR0-DR15
        ctrlReg,                        // 操作数是CR0-CR15
    }

    [Flags]
    public enum MatchForbid: uint
    {
        None = 0,
        Forbid_8bit = 0x01,
        Forbid_16bit = 0x02,
        Forbid_32bit = 0x04,
        Forbid_64bit = 0x08,
        Forbid_mem = 0x10,
    }

    [Flags]
    public enum OpcodeFlag: uint
    {
        None = 0,
        ModRM_R = 0x01,                 //  /r :     带 ModRM 并且 其中代表两个值，reg 和 r/m
        ModRM_Digit = 0x02,             //  /digit:  带 ModRM 并且 r/m 域代表r/m, reg 域代表 3bit 的额外 opcode
        bit0Size = 0x04,                //  指令码的 bit0 代表操作数大小, bit0==0是为8bit, bit0==1是为 16/32/64 bit, 
        bit3Size = 0x08,                //  指令码的 bit3 代表操作数大小, bit3==0是为8bit, bit3==1是为 16/32/64 bit
        opcodeWithReg = 0x10,           //  将寄存器插入到 opcode 的 bit2-bit0 位置
        withImm = 0x20,                 //  指令码最后带立即数作为操作数
        withM64 = 0x40,                 //  指令码最后带64位立即数做寻址
        RMInRight = 0x80,               //  该bit为1时表示匹配的 R/M 域放置于第2个操作数，为0则R/M 域放置于第1个操作数
    }

    public class OpcodeInfos
    {
        public byte[]? opcodes = null;
        public int numberOfOperand = 0;
        public MatchType op0 = MatchType.None;
        public MatchType op1 = MatchType.None;
        public MatchType op2 = MatchType.None;
        public MatchType op3 = MatchType.None;
        public OpcodeFlag opcodeFlag = OpcodeFlag.None;
        public byte digit = 0;
        public MatchForbid forbidInfo = MatchForbid.None;
    }

    public enum InsnProcessFlag: uint
    {
        None = 0,
        ProcessTokens = 0x01,                           // 伪指令自己处理 token
        ProcessExpressions = 0x02,                      // 伪指令处理ulong值以内大小的表达式(如DB、DW、DD、DQ)
    }

    public abstract class IInsnProcessor
    {
        public abstract int process(string insnStr, SourceLine sourceLine, int pass);
        public abstract InsnProcessFlag GetInsnFlag();
    }

    public abstract class BaseInsn: IInsnProcessor
    {
        protected static OpcodeInfos noOperand = new OpcodeInfos();
        protected static byte[] finalCode = new byte[32];           // 全体code
        protected static byte[] prefixCode = new byte[32];          // 前缀部分的code
        protected static byte[] insCode = new byte[32];             // 指令部分的code
        protected static byte[] addrCode = new byte[32];            // 内存寻址部分的code
        protected static byte[] immCode = new byte[32];             // 立即数部分的code

        override public InsnProcessFlag GetInsnFlag()
        {
            return InsnProcessFlag.None;
        }

        protected int processCpuIns(string insnStr, SourceLine sourceLine, int pass, LinkedList<OpcodeInfos> opcodeInfos)
        {
            Console.WriteLine("process. " + insnStr);

            int insTotalSize = 0;
            LinkedListNode<OpcodeInfos>? currentNode;
            OpcodeInfos? info = null;
            OpcodeInfos? matchedInfo = null;

            int expressionsCount = sourceLine.expressions != null ? sourceLine.expressions.Count : 0;       // 当前指令的表达式的个数 
            currentNode = opcodeInfos.First;
            while (currentNode is not null) {
                info = currentNode.Value;
                if (info.numberOfOperand == expressionsCount) {
                    if (info.numberOfOperand == 0) {
                        matchedInfo = noOperand;
                        break;                                              // 不需要操作数的指令直接匹配
                    }
                    //
                    if (expressionsCount == 1) {
                        if (checkOperandMatch(sourceLine?.expressions?[0].operand, info.op0)) {
                            matchedInfo = info;
                            break;
                        }
                    }
                    else if (expressionsCount == 2) {
                        if (checkOperandMatch(sourceLine?.expressions?[0].operand, info.op0) &&
                            checkOperandMatch(sourceLine?.expressions?[1].operand, info.op1) ) {
                            matchedInfo = info;
                            break;
                        }
                    }
                    else if (expressionsCount == 3) {
                        if (checkOperandMatch(sourceLine?.expressions?[0].operand, info.op0) &&
                            checkOperandMatch(sourceLine?.expressions?[1].operand, info.op1) &&
                            checkOperandMatch(sourceLine?.expressions?[2].operand, info.op2) ) {
                            matchedInfo = info;
                            break;
                        }
                    }
                }
                currentNode = currentNode.Next;
            }

            // matchedInfo非空则表示匹配
            if (matchedInfo is not null) {
                insTotalSize = matchedInfo.opcodes!.Length;
                MemoryAddressResult? mem = null;
                X64RegValue regFieldInModRM = X64RegValue.None;
                X64RegValue rmFieldInModRM = X64RegValue.None;
                int prefixCodeSize = 0;
                int insCodeSize = 0;
                int addrCodeSize = 0;
                int immCodeSize = 0;
                bool flagRexE = false;
                bool flagRexW = false;
                bool flagRexR = false;
                bool flagRexX = false;
                bool flagRexB = false;
                bool addr32bitPrefix = false;
                int bitSize = 0;

                switch (expressionsCount) {
                    case 0: {

                        }
                        break;
                    case 1: {

                        }
                        break;
                    case 2: {
                            bitSize = getBaseInsnOpSize2(sourceLine!, pass, opcodeInfos);
                            if (bitSize == 0) {
                                //// 报错, 无法决定操作数类型
                                return 0;
                            }
                            if (matchedInfo.opcodeFlag.HasFlag(OpcodeFlag.ModRM_R) || matchedInfo.opcodeFlag.HasFlag(OpcodeFlag.ModRM_Digit)) {
                                // 存在 ModRM 字段
                                if (matchedInfo.op0 == MatchType.rm) {                          // 如果 op0 匹配了 R/M 域
                                    var operand0 = sourceLine?.expressions?[0].operand;
                                    var operand1 = sourceLine?.expressions?[1].operand;
                                    if (operand0!.type == X64OperandType.MemoryAddress) {
                                        mem = operand0.addressRes;
                                    } else if (operand0!.type == X64OperandType.Register) {
                                        rmFieldInModRM = operand0.regValue;
                                    } else if (operand0!.type == X64OperandType.Symbol) {
                                        // to-do
                                    }
                                    //
                                    if (operand1!.type == X64OperandType.Register) {
                                        regFieldInModRM = operand1.regValue;
                                    }
                                }
                                else if (matchedInfo.op1 == MatchType.rm) {                     // 如果 op1 匹配了 R/M 域
                                    var operand0 = sourceLine?.expressions?[0].operand;
                                    var operand1 = sourceLine?.expressions?[1].operand;
                                    if (operand1!.type == X64OperandType.MemoryAddress) {
                                        mem = operand1.addressRes;
                                    } else if (operand1!.type == X64OperandType.Register) {
                                        rmFieldInModRM = operand1.regValue;
                                    } else if (operand1!.type == X64OperandType.Symbol) {
                                        // to-do
                                    }
                                    //
                                    if (operand0!.type == X64OperandType.Register) {
                                        regFieldInModRM = operand0.regValue;
                                    }
                                }
                                else {
                                    //// 报错
                                    return 0;
                                }
                                //
                                Array.Copy(matchedInfo.opcodes, insCode, matchedInfo.opcodes.Length);
                                insCodeSize = matchedInfo.opcodes.Length;
                                Array.Copy(mem!.code, addrCode, mem.codeSize);
                                addrCodeSize = mem.codeSize;
                                //
                                if (mem.type.HasFlag(MemoryAddressType.withRex_X))
                                    flagRexX = true;
                                if (mem.type.HasFlag(MemoryAddressType.withRex_B))
                                    flagRexB = true;
                                if (mem.type.HasFlag(MemoryAddressType.with32bitRegAddr))
                                    addr32bitPrefix = true;
                                //
                                if (matchedInfo.opcodeFlag.HasFlag(OpcodeFlag.ModRM_Digit)) {
                                    // 把操作码插入到 ModRM 中的 reg 域
                                    addrCode[0] &= 0xC7;
                                    addrCode[0] |= (byte)((matchedInfo.digit & 0x07) << 3);
                                }
                                else { //HasFlag(OpcodeFlag.ModRM_R)
                                    // 将寄存器插入到 ModRM 中的 reg 域
                                    if (regFieldInModRM != X64RegValue.None) {
                                        uint regValue = (uint)regFieldInModRM;
                                        addrCode[0] &= 0xC7;
                                        addrCode[0] |= (byte)((regValue & 0x07) << 3);
                                    }
                                    if (X64RegUtil.IsRexExtensionReg(regFieldInModRM)) 
                                        flagRexR = true;
                                    if (X64RegUtil.IsRexPrefixReg(regFieldInModRM))
                                        flagRexE = true;
                                }
                                //
                                if (matchedInfo.opcodeFlag.HasFlag(OpcodeFlag.bit0Size)) {
                                    if (bitSize != 8) {
                                        insCode[insCodeSize - 1] |= 0x01;
                                    }
                                }
                                if (matchedInfo.opcodeFlag.HasFlag(OpcodeFlag.bit3Size)) {
                                    if (bitSize != 8) {
                                        insCode[insCodeSize - 1] |= 0x08;
                                    }
                                }
                                if (matchedInfo.opcodeFlag.HasFlag(OpcodeFlag.withImm)) {
                                    int bytesize = bitSize / 8;
                                }
                                if (bitSize == 64) 
                                    flagRexW = true;
                                getPrefixCode(ref prefixCodeSize, sourceLine!, matchedInfo, addr32bitPrefix, bitSize, flagRexE, flagRexW, flagRexR, flagRexX, flagRexB);
                                return 0;
                            }
                        }
                        break;
                    case 3: {

                        }
                        break;
                    default: {
                            //// 报错，没有超过3个操作数的指令
                        }
                        break;
                }
                
            }
            else {
                //// 报错，找不到指令匹配
                return 0;
            }

            /*
             public enum OpcodeFlag: uint
    {
        None = 0,
        ModRM_R = 0x01,                 //  /r :     带 ModRM 并且 其中代表两个值，reg 和 r/m
        ModRM_Digit = 0x02,             //  /digit:  带 ModRM 并且 r/m 域代表r/m, reg 域代表 3bit 的额外 opcode
        bit0Size = 0x04,                //  指令码的 bit0 代表操作数大小, bit0==0是为8bit, bit0==1是为 16/32/64 bit, 
        bit3Size = 0x08,                //  指令码的 bit3 代表操作数大小, bit3==0是为8bit, bit3==1是为 16/32/64 bit
        opcodeWithReg = 0x10,           //  将寄存器插入到 opcode 的 bit2-bit0 位置
        withImm = 0x20,                 //  指令码最后带立即数作为操作数
        withM64 = 0x40,
        RMInRight = 0x80,               //  该bit为1时表示匹配的 R/M 域放置于第2个操作数，为0则R/M 域放置于第1个操作数
    }
             */


            return 0;
        }

        protected bool checkOperandMatch(X64Operand? operand, MatchType matchType)
        {
            if (operand is null) { return false; }
            switch (matchType) {
                case MatchType.reg: {   // 操作数是一个通用寄存器
                        if (operand.type == X64OperandType.Register) {
                            if (X64RegUtil.IsCommonReg(operand.regValue)) {
                                return true;
                            }
                        }
                    }
                    break;
                case MatchType.acc: {
                        if (operand.type == X64OperandType.Register) {
                            if (operand.regValue == X64RegValue.AL || operand.regValue == X64RegValue.AX 
                                || operand.regValue == X64RegValue.EAX || operand.regValue == X64RegValue.RAX) {
                                return true;
                            }
                        }
                    }
                    break;
                case MatchType.rm: {
                        if (operand.type == X64OperandType.Register) {
                            if (X64RegUtil.IsCommonReg(operand.regValue)) {
                                return true;
                            }
                        }
                        if (operand.type == X64OperandType.MemoryAddress) {
                            return true;
                        }
                        if (operand.type == X64OperandType.Symbol) {
                            // 不加 offset 修饰的符号当作内存寻址
                        }
                    }
                    break;
                case MatchType.imm: {
                        //if (operand.type) {

                        //}
                    }
                    break;




                /*
                 
        imm,                            // 操作数是立即数
        rm,                             // 操作数是寄存器或者内存寻址
        //moffset32,                      // 操作数是内存寻址，用[imm]寻址的
        moffset64,                      // 操作数是内存寻址，用[imm64]寻址的
        segReg,                         // 操作数是段寄存器
        debugReg,                       // 操作数是DR0-DR15
        ctrlReg,                        // 操作数是CR0-CR15
                 */
            }
            return false; 
        }

        // 返回操作数的大小（bit数）,无法获得则返回0
        protected int getOpSize(X64Operand? op)
        {
            if (op is null) { return 0; }
            if (op.type == X64OperandType.Register) {
                if (X64RegUtil.Is8BitReg(op.regValue)) {
                    return 8;
                }
                if (X64RegUtil.Is16BitReg(op.regValue)) {
                    return 16;
                }
                if (X64RegUtil.Is32BitReg(op.regValue)) {
                    return 32;
                }
                if (X64RegUtil.Is64BitReg(op.regValue)) {
                    return 64;
                }
                if (X64RegUtil.IsSegReg(op.regValue)) {
                    return 16;
                }
                if (X64RegUtil.IsDebugReg(op.regValue)) {
                    return 64;
                }
                if (X64RegUtil.IsCtrlReg(op.regValue)) {
                    return 64;
                }
            }
            return 0;
        }

        // 获取基本指令的整体操作数大小，返回位数
        virtual protected int getBaseInsnOpSize2(SourceLine sourceLine, int pass, LinkedList<OpcodeInfos> opcodeInfos)
        {
            int opSize0, opSize1;
            int ret = 0;
            opSize0 = getOpSize(sourceLine?.expressions?[0].operand);
            opSize1 = getOpSize(sourceLine?.expressions?[1].operand);
            if (opSize0 == 0 && opSize1 == 0) {
                //// 报错
                return 0;
            }
            if ((opSize0 != 0 && opSize1 != 0) && opSize0 != opSize1) {
                //// 报错
                return 0;
            }
            if (opSize0 != 0)
                ret = opSize0;
            else if (opSize1 != 0)
                ret = opSize1;
            
            return ret;
        }

        int getPrefixCode(ref int prefixCodeSize, SourceLine sourceLine, OpcodeInfos matchedInfo, 
            bool addr32bit, int bitSize, bool flagRexE, bool flagRexW, bool flagRexR, bool flagRexX, bool flagRexB)
        {
            byte rex = 0;
            int cnt = 0;
            // 前缀排列 F0, F2/F3, 67, 66, 64/65, 40  (LOCK，REP, 寻址, 16位操作, 段前缀, 寄存器扩展)
            if (flagRexE || flagRexW || flagRexR || flagRexX || flagRexB)
                rex = 0x40;
            if (flagRexW)
                rex |= 0x08;
            if (flagRexR)
                rex |= 0x04;
            if (flagRexX)
                rex |= 0x02;
            if (flagRexB)
                rex |= 0x01;
            //// TO-DO 加上 LOCK,rep前缀
            if (addr32bit)
                prefixCode[cnt++] = 0x67;
            if (bitSize == 16)
                prefixCode[cnt++] = 0x66;
            //// TO-DO 加上段前缀
            if (rex != 0)
                prefixCode[cnt++] = rex;

            prefixCodeSize = cnt;
            return cnt;
        }
    }

    // 基本数据定义伪指令 (DB,DW,DD,DQ)
    public abstract class DataDefineBaseInsn : IInsnProcessor
    {

    }


}

