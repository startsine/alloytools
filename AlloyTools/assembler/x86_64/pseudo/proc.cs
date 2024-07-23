
namespace AlloyTools.Assembler.AMD64.Instruction
{
    public class PROC: IInsnProcessor
    {
        override public InsnProcessFlag GetInsnFlag()
        {
            return InsnProcessFlag.ProcessTokens;
        }

        override public int process(X64Assembler asm, string insnStr, SourceLine sourceLine, int pass)
        {
            X64FragmentList fragmentList = asm.fragmentList;
            if (pass <= 1) {
                if (fragmentList.fragments.Count == 1 && fragmentList.fragments[0].isDefault) {
                }
            }
            else {

            }
            Console.WriteLine("PROC");
            return 0;
        }
    }
}
