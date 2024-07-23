
namespace AlloyTools.Assembler.AMD64.Instruction
{
    public class DATA : IInsnProcessor
    {
        override public InsnProcessFlag GetInsnFlag()
        {
            return InsnProcessFlag.ProcessTokens;
        }

        override public int process(X64Assembler asm, string insnStr, SourceLine sourceLine, int pass)
        {
            return 0;

        }
    }
}
