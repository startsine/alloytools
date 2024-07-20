


using System.Collections.Generic;
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
        RMInRight = 0x40,               //  该bit为1时表示匹配的 R/M 域放置于第2个操作数，为0则R/M 域放置于第1个操作数
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


    public abstract class IInsnProcessor
    {
        public abstract int process(string insnStr, SourceLine sourceLine, int pass);
    }

    public abstract class BaseInsn: IInsnProcessor
    {
        protected static OpcodeInfos noOperand = new OpcodeInfos();

        protected int processCpuIns(string insnStr, SourceLine sourceLine, int pass, LinkedList<OpcodeInfos> opcodeInfos)
        {
            Console.WriteLine("process. " + insnStr);

            
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
                    if (info.numberOfOperand == 1) {
                        if (checkOperandMatch(sourceLine?.expressions?[0].operand, info.op1)) {
                            matchedInfo = info;
                            break;
                        }
                    }
                    else if (info.numberOfOperand == 2) {
                        if (checkOperandMatch(sourceLine?.expressions?[0].operand, info.op1) &&
                            checkOperandMatch(sourceLine?.expressions?[1].operand, info.op2) ) {
                            matchedInfo = info;
                            break;
                        }
                    }
                    else if (info.numberOfOperand == 3) {
                        if (checkOperandMatch(sourceLine?.expressions?[0].operand, info.op1) &&
                            checkOperandMatch(sourceLine?.expressions?[1].operand, info.op2) &&
                            checkOperandMatch(sourceLine?.expressions?[2].operand, info.op3) ) {
                            matchedInfo = info;
                            break;
                        }
                    }
                }
                currentNode = currentNode.Next;
            }

            // matchedInfo非空则表示匹配
            if (matchedInfo is not null) {

            }



            return 0;
        }

        bool checkOperandMatch(X64Operand? operand, MatchType matchType)
        {

            return true; 
        }
        
    }
}

