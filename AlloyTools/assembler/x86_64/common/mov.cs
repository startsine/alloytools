
namespace AlloyTools.Assembler.AMD64.Instruction
{
    public class MOV: BaseInsn
    {
        static LinkedList<OpcodeInfos> opcodeInfos;

        static MOV()
        {
            OpcodeInfos info;
            opcodeInfos = new LinkedList<OpcodeInfos>();
            // MOV acc, moffset64  == A0 /s0
            info = new OpcodeInfos();
            info.opcodes = new byte[1] { 0xA0 };
            info.numberOfOperand = 2;
            info.op0 = MatchType.acc;
            info.op1 = MatchType.moffset64;
            info.opcodeFlag = OpcodeFlag.bit0Size | OpcodeFlag.withM64;
            opcodeInfos.AddLast(info);
            // MOV moffset64, acc  == A2 /s0
            info = new OpcodeInfos();
            info.opcodes = new byte[1] { 0xA2 };
            info.numberOfOperand = 2;
            info.op0 = MatchType.moffset64;
            info.op1 = MatchType.acc;
            info.opcodeFlag = OpcodeFlag.bit0Size | OpcodeFlag.withM64;
            opcodeInfos.AddLast(info);
            // MOV reg/mem, reg  == 88 /r /s0
            info = new OpcodeInfos();
            info.opcodes = new byte[1] { 0x88 };
            info.numberOfOperand = 2;
            info.op0 = MatchType.rm;
            info.op1 = MatchType.reg;
            info.opcodeFlag = OpcodeFlag.ModRM_R | OpcodeFlag.bit0Size;
            opcodeInfos.AddLast(info);
            // MOV reg, reg/mem,  == 8A /r /s0
            info = new OpcodeInfos();
            info.opcodes = new byte[1] { 0x8A };
            info.numberOfOperand = 2;
            info.op0 = MatchType.reg;
            info.op1 = MatchType.rm;
            info.opcodeFlag = OpcodeFlag.ModRM_R | OpcodeFlag.bit0Size | OpcodeFlag.RMInRight;
            opcodeInfos.AddLast(info);
            // MOV reg, imm == B0 +r i /s3
            info = new OpcodeInfos();
            info.opcodes = new byte[1] { 0xB0 };
            info.numberOfOperand = 2;
            info.op0 = MatchType.reg;
            info.op1 = MatchType.imm;
            info.opcodeFlag = OpcodeFlag.bit3Size | OpcodeFlag.withImm | OpcodeFlag.opcodeWithReg;
            opcodeInfos.AddLast(info);
            // MOV reg/mem, imm  == C6 /0 i /s0
            info = new OpcodeInfos();
            info.opcodes = new byte[1] { 0xC6 };
            info.numberOfOperand = 2;
            info.op0 = MatchType.rm;
            info.op1 = MatchType.imm;
            info.opcodeFlag = OpcodeFlag.bit0Size | OpcodeFlag.withImm | OpcodeFlag.ModRM_Digit;
            info.digit = 0;
            opcodeInfos.AddLast(info);
            // MOV reg/mem, segReg == 8C /r
            info = new OpcodeInfos();
            info.opcodes = new byte[1] { 0x8C };
            info.numberOfOperand = 2;
            info.op0 = MatchType.rm;
            info.op1 = MatchType.segReg;
            info.opcodeFlag = OpcodeFlag.ModRM_R;
            info.forbidInfo = MatchForbid.Forbid_8bit;              // ½ûÖ¹8bit²Ù×÷
            opcodeInfos.AddLast(info);
            // MOV segReg, reg/mem16 == 8E /r
            info = new OpcodeInfos();
            info.opcodes = new byte[1] { 0x8E };
            info.numberOfOperand = 2;
            info.op0 = MatchType.segReg;
            info.op1 = MatchType.rm;
            info.opcodeFlag = OpcodeFlag.ModRM_R | OpcodeFlag.RMInRight;
            info.forbidInfo = MatchForbid.Forbid_8bit | MatchForbid.Forbid_32bit | MatchForbid.Forbid_64bit;    // ½ûÖ¹8/32/64bit²Ù×÷
            opcodeInfos.AddLast(info);
            // MOV reg64, DRn == 0F 21 /r
            info = new OpcodeInfos();
            info.opcodes = new byte[2] { 0x0F, 0x21 };
            info.numberOfOperand = 2;
            info.op0 = MatchType.rm;
            info.op1 = MatchType.debugReg;
            info.opcodeFlag = OpcodeFlag.ModRM_R;
            info.forbidInfo = MatchForbid.Forbid_8bit | MatchForbid.Forbid_16bit | MatchForbid.Forbid_32bit | MatchForbid.Forbid_mem;   // ½ûÖ¹ÄÚ´æ²Ù×÷ºÍ8/16/32bit¼Ä´æÆ÷
            opcodeInfos.AddLast(info);
            // MOV DRn, reg64 == 0F 23 /r
            info = new OpcodeInfos();
            info.opcodes = new byte[2] { 0x0F, 0x23 };
            info.numberOfOperand = 2;
            info.op0 = MatchType.debugReg;
            info.op1 = MatchType.rm;
            info.opcodeFlag = OpcodeFlag.ModRM_R | OpcodeFlag.RMInRight;
            info.forbidInfo = MatchForbid.Forbid_8bit | MatchForbid.Forbid_16bit | MatchForbid.Forbid_32bit | MatchForbid.Forbid_mem;   // ½ûÖ¹ÄÚ´æ²Ù×÷ºÍ8/16/32bit¼Ä´æÆ÷
            opcodeInfos.AddLast(info);
            // MOV reg64, CRn == 0F 20 /r
            info = new OpcodeInfos();
            info.opcodes = new byte[2] { 0x0F, 0x20 };
            info.numberOfOperand = 2;
            info.op0 = MatchType.rm;
            info.op1 = MatchType.ctrlReg;
            info.opcodeFlag = OpcodeFlag.ModRM_R;
            info.forbidInfo = MatchForbid.Forbid_8bit | MatchForbid.Forbid_16bit | MatchForbid.Forbid_32bit | MatchForbid.Forbid_mem;   // ½ûÖ¹ÄÚ´æ²Ù×÷ºÍ8/16/32bit¼Ä´æÆ÷
            opcodeInfos.AddLast(info);
            // MOV CRn, reg64 == 0F 22 /r
            info = new OpcodeInfos();
            info.opcodes = new byte[2] { 0x0F, 0x22 };
            info.numberOfOperand = 2;
            info.op0 = MatchType.ctrlReg;
            info.op1 = MatchType.rm;
            info.opcodeFlag = OpcodeFlag.ModRM_R | OpcodeFlag.RMInRight;
            info.forbidInfo = MatchForbid.Forbid_8bit | MatchForbid.Forbid_16bit | MatchForbid.Forbid_32bit | MatchForbid.Forbid_mem;   // ½ûÖ¹ÄÚ´æ²Ù×÷ºÍ8/16/32bit¼Ä´æÆ÷
            opcodeInfos.AddLast(info);
        }

        override public int process(X64Assembler asm, string insnStr, SourceLine sourceLine, int pass)
        {
            processCpuIns(insnStr, sourceLine, pass, opcodeInfos);
            
            return 0; 
        }
    }
}

