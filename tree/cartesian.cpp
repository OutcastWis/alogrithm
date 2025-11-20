#include <tree/cartesian.h>

#include <cassert>
#include <cmath>

#define ARRAY_NULL(x) \
  if (x) {            \
    delete[] x;       \
    x = nullptr;      \
  }

namespace wzj {
void cartesian::build(const std::vector<int>& data) {
  clear();
  _init_capacity(data.size());
  memcpy(w_ + 1, data.data(), sizeof(int) * data.size());

  int i = 1, k = top_;  // 标号从1开始
  for (auto x : data) {
    k = top_;
    while (k && w_[stack_[k]] > x) --k;
    if (k) rc_[stack_[k]] = i;
    if (k < top_) lc_[i] = stack_[k + 1];

    same_[i] = (k && x == w_[stack_[k]]) ? stack_[k] : i;
    stack_[k + 1] = i++;  // 入栈
    top_ = k + 1;         // 更新栈顶
  }
}

void cartesian::clear() {
  ARRAY_NULL(lc_)
  ARRAY_NULL(rc_)
  ARRAY_NULL(w_);
  ARRAY_NULL(stack_);
  ARRAY_NULL(same_);
  top_ = 0;
}

std::pair<int, int> cartesian::range(int i) const {
  int left = same_[i + 1], right = same_[i + 1];
  while (lc_[left] != 0) left = lc_[left];
  while (rc_[right] != 0) right = rc_[right];
  return {left - 1, right - 1};
}

void cartesian::_init_capacity(size_t s) {
  n_ = s;
  ++s;  // 从1开始
  lc_ = new int[s]();
  rc_ = new int[s]();
  w_ = new int[s]();
  stack_ = new int[s]();
  same_ = new int[s]();
}

}  // namespace wzj