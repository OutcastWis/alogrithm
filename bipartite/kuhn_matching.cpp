#include <bipartite/kuhn_matching.h>

#include <ctime>
#include <random>

static std::mt19937_64 rng(
    static_cast<std::mt19937_64::result_type>(std::time(nullptr)));

namespace wzj {

std::vector<std::pair<int, int>> kuhn_matching::maximum_matching() {
  // random shuffle edges
  for (auto& e : graph_) std::shuffle(e.begin(), e.end(), rng);

  // 这里可见, 每个X的顶点只会处理一次, 每次dfs是O(m).
  // 所以总复杂度是O(nm)
  while (true) {
    bool succ = false;
    visited_.assign(n1_, false);
    for (int u = 0; u < n1_; ++u) succ |= ma_[u] == -1 && _dfs(u);
    if (!succ) break;
  }

  std::vector<std::pair<int, int>> matches;
  for (int u = 0; u < n1_; ++u) {
    if (ma_[u] != -1) matches.emplace_back(u, ma_[u]);
  }
  return matches;
}

bool kuhn_matching::_dfs(int u) {
  visited_[u] = true;

  for (auto v : graph_[u]) {
    if (mb_[v] == -1) {
      ma_[u] = v;
      mb_[v] = u;
      return true;
    }
  }

  for (auto v : graph_[u]) {
    if (!visited_[mb_[v]] && _dfs(mb_[v])) {
      ma_[u] = v;
      mb_[v] = u;
      return true;
    }
  }
  return false;
}
}  // namespace wzj