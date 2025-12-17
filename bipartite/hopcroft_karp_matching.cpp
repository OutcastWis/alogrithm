#include <bipartite/hopcroft_karp_matching.h>

#include <queue>

namespace wzj {
std::vector<std::pair<int, int>> hopcroft_karp_matching::maximum_match() {
  while (_bfs()) {
    for (int u = 0; u < n1_; ++u)
      if (ma_[u] == -1) _dfs(u);
  }

  std::vector<std::pair<int, int>> matches;
  for (int u = 0; u < n1_; ++u) {
    if (ma_[u] != -1) matches.emplace_back(u, ma_[u]);
  }
  return matches;
}

bool hopcroft_karp_matching::_dfs(int u) {
  for (auto v : graph_[u]) {
    if (mb_[v] == -1 || (level_[mb_[v]] == level_[u] + 1 && _dfs(mb_[v]))) {
      ma_[u] = v;
      mb_[v] = u;
      return true;
    }
  }

  // 加速. u不能形成增广路, 后续不用再向下dfs
  level_[u] = -1;
  return false;
}

bool hopcroft_karp_matching::_bfs() {
  level_.assign(n1_, -1);
  std::queue<int> q;
  for (int u = 0; u < n1_; ++u)
    if (ma_[u] == -1) {
      q.push(u);
      level_[u] = 0;
    }

  bool succ = false;
  while (!q.empty()) {
    int u = q.front();
    q.pop();
    for (auto v : graph_[u]) {
      if (mb_[v] == -1) {
        succ = true;
      } else if (level_[mb_[v]] == -1) {
        level_[mb_[v]] = level_[u] + 1;
        q.push(mb_[v]);
      }
    }
  }

  return succ;
}
}  // namespace wzj