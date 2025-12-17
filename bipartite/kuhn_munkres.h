#include <utility>
#include <vector>

namespace wzj {
/// @brief 二分图最大权匹配. 匈牙利算法又称为 KM 算法.
///
/// 复杂度O(n^3), n是点数
///
/// 令S, T是交替树上属于X,Y的点. S', T'则是没在交替树上的点
class kuhn_munkres {
 public:
  kuhn_munkres(int n);
  // X,Y节点数变成一样, 把最大权匹配变成最大权完美匹配
  kuhn_munkres(int n1, int n2) : kuhn_munkres(n1 > n2 ? n1 : n2) {}
  void add_edge(int u, int v, int w) { w_[u][v] = w; }
  std::vector<std::pair<int, int>> maximum_weight(int* weight);

 private:
  void _bfs(int u);
  bool _check(int u, std::queue<int>& q);
  bool _tree(std::queue<int>& q); // 交替树上寻找增广路. q用于bfs遍历交替树

 private:
  int n_;
  std::vector<std::vector<int>> w_;  // 邻接表
  std::vector<int> ma_, mb_;
  std::vector<int> lx_, ly_;
  // slack[v] = min{ lx[u] + ly[v] - w[u,v] | u在S, v在T'}
  std::vector<int> slack_;
  std::vector<bool> vx_, vy_; // 是否在交替树上
  std::vector<int> path_; // path_[v]=u, 表示交替树上是从u走到v
};
}  // namespace wzj