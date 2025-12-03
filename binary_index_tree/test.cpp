#include <binary_index_tree/bit.h>
#include <binary_index_tree/bit_with_d.h>

#include <cassert>
#include <iostream>
#include <random>

int get_sum(int l, int j, const std::vector<int>& data) {
  int tot = 0;
  for (int i = l; i <= j; ++i) tot += data[i];
  return tot;
}

void update(int l, int j, std::vector<int>& data, int v) {
  for (int i = l; i <= j; ++i) data[i] += v;
}

void test_bit() {
  std::vector<int> data;
  for (int i = 0; i < 100; ++i) data.push_back(std::rand() % 100);

  wzj::bit tree(data);
  //
  for (int i = 0; i < data.size(); ++i) {
    for (int j = i; j < data.size(); ++j) {
      int ans = get_sum(i, j, data);
      assert(ans == tree.sum(i + 1, j + 1));
    }
  }
  //
  tree.add(5, 10);
  assert(tree.sum(5, 5) == data[4] + 10);
  tree.add(17, -44);
  assert(tree.sum(17, 18) == data[16] + data[17] - 44);
}

void test_bit_with_d() {
  std::vector<int> data;
  for (int i = 0; i < 100; ++i) data.push_back(std::rand() % 100);

  wzj::bit_with_d tree(data);
  //
  for (int i = 0; i < data.size(); ++i) {
    for (int j = i; j < data.size(); ++j) {
      int ans = 0;
      for (int k = i; k <= j; ++k) ans += data[k];
      assert(ans == tree.sum(i + 1, j + 1));
    }
  }
  //
  tree.add(5, 68, 10);
  update(4, 67, data, 10);
  assert(tree.sum(1, 6) == get_sum(0, 5, data));
  assert(tree.sum(33, 69) == get_sum(32, 68, data));

  tree.add(17, 88, -44);
  update(16, 87, data, -44);
  assert(tree.sum(17, 18) == get_sum(16, 17, data));
}

int main() {
  test_bit();
  test_bit_with_d();
}