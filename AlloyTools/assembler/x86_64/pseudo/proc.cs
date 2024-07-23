
namespace AlloyTools.Assembler.AMD64.Instruction
{
    public class PROC: IInsnProcessor
    {
        override public InsnProcessFlag GetInsnFlag()
        {
            return InsnProcessFlag.ProcessTokens;
        }

        override public int process(string insnStr, SourceLine sourceLine, int pass)
        {
            Console.WriteLine("PROC");
            return 0;

        }
    }
}
