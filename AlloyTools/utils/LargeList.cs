
namespace AlloyTools.utils
{
    public class LargeList<T>
    {
        private const uint BLOCKSIZE = (4 * 1024 * 1024);
        private ulong _count;
        private List<List<T>> allList;

        public LargeList()
        {
            _count = 0;
            allList = new List<List<T>>();
        }

        public ulong Count { get { return _count; } }

        public void Add(T e)
        {
            if (_count == 0) {
                allList.Add(new List<T>());
            }
            int blockCount = allList.Count;
            int lastBlockIndex = blockCount - 1;
            if (allList[lastBlockIndex].Count >= BLOCKSIZE) {
                allList.Add(new List<T>());
                lastBlockIndex = blockCount - 1;
            }
            allList[lastBlockIndex].Add(e);
        }

        public T this[ulong index] {
            get {
                ulong blockIndex = index / BLOCKSIZE;
                int indexInBlock = (int)(index % BLOCKSIZE);
                return allList[(int)blockIndex][indexInBlock];
            }
            set {
                ulong blockIndex = index / BLOCKSIZE;
                int indexInBlock = (int)(index % BLOCKSIZE);
                allList[(int)blockIndex][indexInBlock] = value;
            }
        }
    }
}
