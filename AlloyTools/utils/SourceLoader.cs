
namespace AlloyTools.utils
{
    class SourceLoader
    {
        private List<string> serachPathList;

        public SourceLoader()
        {
            serachPathList = new List<string> ();
        }

        public SourceLoader(List<string>? paths)
        {
            if (paths == null)
                serachPathList = new List<string> ();
            else
                serachPathList = paths;
        }

        public void AddSearchPath(string path) 
        {
            serachPathList.Add(path);
        }

        public byte[]? LoadFile(string filepath)
        {
            byte[]? buffer = null;
            FileStream fs = new FileStream(filepath, FileMode.Open);
            using (fs) {
                long len = fs.Length;
                if (len > 0) {
                    buffer = new byte[len];
                    fs.Read(buffer, 0, (int)len);
                }
                fs.Close();
            }
            return buffer;
        }
    }
}

