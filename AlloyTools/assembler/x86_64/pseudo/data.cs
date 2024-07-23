
namespace AlloyTools.Assembler.AMD64.Instruction
{
    public class DATA : IInsnProcessor
    {
        override public InsnFlag GetInsnFlag()
        {
            return InsnFlag.None;
        }

        override public int process(string insnStr, SourceLine sourceLine, int pass)
        {
            return 0;

        }
    }
}
