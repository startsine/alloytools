
namespace AlloyTools.utils
{
    public class SourceLoader
    {
        private const int BUFF_SIZE = 4096;
        private List<string> serachPathList;
        private FileStream? fileStream;
        private byte[] buffer;
        private bool endOfFile;
        int endCursor;                              // 有效数据结尾指针
        int curCursor;                              // 当前读取指针

        public SourceLoader()
        {
            serachPathList = new List<string> ();
            buffer = new byte [BUFF_SIZE + 2];
            endOfFile = false;
        }

        public SourceLoader(List<string>? paths)
        {
            if (paths == null)
                serachPathList = new List<string> ();
            else
                serachPathList = paths;
            buffer = new byte[BUFF_SIZE + 2];
            endOfFile = false;
        }

        public void AddSearchPath(string path) 
        {
            serachPathList.Add(path);
        }

        public void LoadFile(string filepath)
        {
            fileStream = new FileStream(filepath, FileMode.Open);
            int n = fileStream.Read(buffer, 2, BUFF_SIZE);
            if (n > 0) {
                endOfFile = false;
                curCursor = 2;
                endCursor = 2 + n;
            } else {
                endOfFile = true;
                fileStream.Close();
            }
        }

        // 读取一个字节，读取后会移动指针
        public byte GetByte(ref bool EOF)
        {
            byte ret;
            EOF = false;
            if (curCursor >= endCursor) {
                EOF = true;
                return 0;
            }
            ret = buffer[curCursor];
            curCursor++;
            //
            if (curCursor + 2 >= endCursor) {       // 如果只剩下2个或者2个以下字节大小，把剩余字节搬到数组前面，然后再从文件中读取一些数据添加到后面补充
                if (!endOfFile) {
                    int movSize = endCursor - curCursor;
                    if (movSize > 0) {
                        for (int i = 0; i < movSize; i++) {         // 把剩余的字节移到数组头部
                            buffer[i] = buffer[curCursor + i];
                        }
                        curCursor = 0;
                        endCursor = movSize;
                        if (fileStream != null) {
                            int n = fileStream.Read(buffer, movSize, BUFF_SIZE);
                            if (n > 0) {
                                endOfFile = false;
                                endCursor += n;
                            }
                            else {
                                endOfFile = true;
                                fileStream.Close();
                            }
                        }
                    }
                }
            }
            return ret;
        }

        // 预读取下一个字节
        public byte PreGet()
        {
            if (curCursor >= endCursor) {
                return 0;
            }
            return buffer[curCursor];
        }

        // 预读取下一个的下一个字节，不移动文件指针，到了文件尽头返回0
        public byte PreGetNext()
        {
            if (curCursor + 1 >= endCursor) {
                return 0;
            }
            return buffer[curCursor + 1];
        }
    }
}

