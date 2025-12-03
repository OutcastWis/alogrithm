#include <binary_index_tree/bit.h>

#include <memory>

namespace wzj {
    bit::bit(int n) : n_(n) {
        c_ = new int[n_+1]();
    }

    bit::bit(const std::vector<int>& data) : n_(data.size()) {
        c_ = new int[n_ + 1]();
        // 每一个节点的值是由所有与自己直接相连的儿子的值求和得到的。
        // 因此可以倒着考虑贡献，即每次确定完儿子的值后，用自己的值更新自己的直接父亲。
        int i = 1;
        for(auto x : data) {
            c_[i] += x;
            int j = i + _lowbit(i);
            if (j <= n_) c_[j] += c_[i];
            ++i;
        }
    }

    void bit::clear() {
        delete []c_;
        n_ = 0;
    }

    int bit::sum(int l, int r) const {
        return _sum(r) - _sum(l-1);
    }

    void bit::add(int x, int v) {
        // 根据区间特性4和5, x处的改变, 会影响自身和x+lowbit(x)
        while (x <= n_) {
            c_[x] += v;
            x += _lowbit(x);
        }
    }

    int bit::_sum(int x) const {
        int tot = 0;
        while (x > 0) {
            tot += c_[x];
            x -= _lowbit(x);
        }
        return tot;
    }
}