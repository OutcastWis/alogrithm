#include <binary_index_tree/bit.h>

#include <cassert>
#include <iostream>
#include <random>

void test_bit() {
  std::vector<int> data;
  for (int i = 0; i < 100; ++i) data.push_back(std::rand() % 100);

  wzj::bit tree(data);
  // 
  for (int i = 0; i < data.size(); ++i) {
    for (int j = i; j < data.size(); ++j) {
      int ans = 0;
      for (int k = i; k <= j; ++k) ans += data[k];
      assert(ans == tree.sum(i + 1, j + 1));
    }
  }
  //
  tree.add(5, 10);
  assert(tree.sum(5,5) == data[4] + 10);
  tree.add(17, -44);
  assert(tree.sum(17, 18) == data[16] + data[17] - 44);
}

int main() { test_bit(); }