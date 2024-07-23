

using System.Collections;

namespace AlloyTools.Assembler.AMD64
{
    public class LineErrorException : Exception
    {
        public int errorId;

        public LineErrorException(int errorId)
        {
            this.errorId = errorId;
        }
    }

    public class FatalErrorException : Exception
    {
        public int errorId;

        public FatalErrorException(int errorId)
        {
            this.errorId = errorId;
        }
    }

    public class X64Errors
    {
        public int errorsTotal = 0;
        public int warningTotal = 0;
        public int maxErrors = 10;                   // 错误个数操作这个就退出进程
        private Hashtable htCodeMapText;

        public X64Errors()
        {
            htCodeMapText = new Hashtable();
            htCodeMapText.Add(1, "error 1: ");
        }

        public void AddError(int errorId, params string[] infoStrs)
        {
            string text = string.Format("format", infoStrs);
            errorsTotal++;
            throw new LineErrorException(errorId);
        }

        public void AddWarning(int warningId, params string[] infoStrs)
        {
            string text = string.Format("format", infoStrs);
            warningTotal++;
        }

        public void AddFatalError(int errorId, params string[] infoStrs)
        {
            string text = string.Format("format", infoStrs);
            errorsTotal++;
            throw new FatalErrorException(errorId);
        }
    }
}



