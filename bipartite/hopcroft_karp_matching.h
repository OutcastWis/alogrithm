#include <utility>
#include <vector>

namespace wzj {
/// @brief hopcroft_karp.  $O(\sqrt{n}m)$  
class hopcroft_karp_matching {
 public:
  hopcroft_karp_matching(int n1, int n2) : n1_(n1), n2_(n2), ma_(n1, -1), mb_(n2, -1), graph_(n1) {}
  void add_edge(int u, int v) { graph_[u].push_back(v); }
  std::vector<std::pair<int, int>> maximum_match();

 private:
  bool _dfs(int u);
  bool _bfs();  // build level

 private:
  int n1_, n2_;
  std::vector<std::vector<int>> graph_;
  std::vector<int> ma_, mb_;  // Y to X
  std::vector<int> level_;
};
}  // namespace wzj