#include <binary_index_tree/bit_with_d.h>

namespace wzj {

bit_with_d::bit_with_d(const std::vector<int>& data) : n_(data.size()) {
  d1_ = new int[n_ + 1]();
  d2_ = new int[n_ + 1]();

  int i = 1, pre = 0;
  for (auto x : data) {
    int v = x - pre;
    d1_[i] += v;
    d2_[i] += v * i;
    int j = i + _lowbit(i);
    if (j <= n_) {
      d1_[j] += d1_[i];
      d2_[j] += d2_[i];
    }
    ++i;
    pre = x;
  }
}

void bit_with_d::clear() {
  delete[] d1_;
  d1_ = nullptr;
  delete[] d2_;
  d2_ = nullptr;
  n_ = 0;
}

void bit_with_d::add(int l, int r, int v) {
  // 对d1_而言: l => l处的值+v, r => r+1处的值-v. 中间保持不变
  _add(l, v, d1_);
  _add(r + 1, -v, d1_);
  // 对d2_而言: l => l处的值+v*l, r => r+1处的值-v*(r+1)
  _add(l, v * l, d2_);
  _add(r + 1, -v * (r + 1), d2_);
}

int bit_with_d::sum(int l, int r) const { return _sum(r) - _sum(l - 1); }

int bit_with_d::_sum(int x) const {
  // \sum_{i=1}^x a_i = \sum_{i=1}^x \sum_{j=1}^i d_j
  //                  = (x+1) \times \sum_{i=1}^x d_i - \sum_{i=1}^x d_i\times i
  return _sum_on(x, d1_) * (x + 1) - _sum_on(x, d2_);
}

int bit_with_d::_sum_on(int x, int* target) const {
  int tot = 0;
  while (x > 0) {
    tot += target[x];
    x -= _lowbit(x);
  }
  return tot;
}

void bit_with_d::_add(int x, int v, int* target) {
  while (x <= n_) {
    target[x] += v;
    x += _lowbit(x);
  }
}
}  // namespace wzj