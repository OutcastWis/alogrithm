#include <utility>
#include <vector>

namespace wzj {
/// @brief Kuhn匹配算法
class kuhn_matching {
 public:
  kuhn_matching(int n1, int n2)
      : n1_(n1), n2_(n2), graph_(n1), ma_(n1, -1), mb_(n2, -1) {}
  void add_edge(int u, int v) { graph_[u].push_back(v); }
  std::vector<std::pair<int, int>> maximum_matching();

 private:
  bool dfs(int u);

 private:
  int n1_, n2_;  // X, Y 的节点数
  std::vector<std::vector<int>> graph_;
  std::vector<int> ma_, mb_;   // X to Y, Y to X
  std::vector<bool> visited_;  // 用于dfs
};
}  // namespace wzj