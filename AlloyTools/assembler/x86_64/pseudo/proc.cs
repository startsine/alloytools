
namespace AlloyTools.Assembler.AMD64.Instruction
{
    public class PROC: IInsnProcessor
    {
        override public InsnFlag GetInsnFlag()
        {
            return InsnFlag.None;
        }

        override public int process(string insnStr, SourceLine sourceLine, int pass)
        {
            Console.WriteLine("PROC");
            return 0;

        }
    }
}
