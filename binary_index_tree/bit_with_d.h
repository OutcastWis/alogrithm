#include <vector>

namespace wzj {
/// @brief 使用差分的树状数组
class bit_with_d {
 public:
  bit_with_d(const std::vector<int>& data);
  ~bit_with_d() { clear(); }

  void add(int l, int r, int v);  // 给区间[l,r]的值增加v. l, r从1开始
  int sum(int l, int r) const;    // l和r从1开始
  void clear();

 private:
  int _lowbit(int x) const { return x & -x; }
  int _sum(int x) const;
  int _sum_on(int x, int* target) const;
  void _add(int x, int v, int* target);

  // d1_, d2_皆是树状数组
  int *d1_ = nullptr,  // d1_ 维护 d_i = a_i - a_{i-1},
      *d2_ = nullptr;  // d2_ 维护 d_i * i
  int n_ = 0;
};
}  // namespace wzj