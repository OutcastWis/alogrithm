#include <bipartite/hopcroft_karp_matching.h>
#include <bipartite/kuhn_matching.h>
#include <bipartite/kuhn_munkres.h>

#include <cassert>
#include <iostream>
#include <random>
#include <string>

void test_km() {
  wzj::kuhn_munkres km(3);
  km.add_edge(0, 0, 1);
  km.add_edge(0, 1, 2);
  km.add_edge(0, 2, 3);
  km.add_edge(1, 0, 4);
  km.add_edge(2, 2, 2);

  int w = 0;
  km.maximum_weight(&w);
  assert(w == 8);
}

void test_all() {
  int epoch = 100;
  while (epoch-- > 0) {
    int n1 = std::rand() % 100 + 20, n2 = std::rand() % 100 + 20;
    wzj::kuhn_munkres km(n1, n2);
    wzj::hopcroft_karp_matching hk(n1, n2);
    wzj::kuhn_matching kuhn(n1, n2);

    for (int i = 0; i < n1; ++i) {
      for (int j = 0; j < n2; ++j) {
        if (std::rand() % 3 < 2) continue;  // 66%�ĸ���û�б�

        km.add_edge(i, j, 1);
        hk.add_edge(i, j);
        kuhn.add_edge(i, j);
      }
    }

    int weight = 0;
    auto r1 = km.maximum_weight(&weight);
    auto r2 = hk.maximum_match();
    auto r3 = kuhn.maximum_matching();
    assert(r1.size() == r2.size());
    assert(r1.size() == r3.size());
  }
}

int main() {
  test_km();
  test_all();
  return 0;
}