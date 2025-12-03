#include <vector>

namespace wzj {



/// 树状数组
/// 能力是线段树的子集, 但写起来更加方便
/// 
/// 对维护信息:
/// 1. 满足交换律: (x@y)@z = x@(y@z)
/// 2. 运算可逆. 即已知 x@y和x, 可以求y
///
/// 区间特性:
/// 1. 下标从1开始
/// 2. c[x]管理[x-2^k+1, x]的区间. k是x二进制最右侧1的位置, 即2^k=lowbit(x)
/// 3. 对于x<=y, c[x]和c[y]区间要么不交, 要么c[x]区间被c[y]区间包含
/// 4. c[x]区间被真包含于c[x+lowbit[x]]
/// 5. 任意x<y<x+lowbit(x), c[x]和c[y]区间不相交
class bit {
    public:
        bit(int n); // 构建长度n, 初始全为0的树
        bit(const std::vector<int>& data); // O(n)建树
        ~bit() {clear();}

        void clear();
        int sum(int l, int r) const; // l和r从1开始
        void add(int x, int v); // 给索引x处的值增加v. x从1开始
    private:
    int _lowbit(int x) const {
        return x & -x;
    }
    int _sum(int x) const; // x从1开始

    int* c_ = nullptr; 
    int n_; // c_[1..n]对应数据. 实际c_长度是n+1
};
}  // namespace wzj