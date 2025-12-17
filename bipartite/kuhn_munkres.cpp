#include <bipartite/kuhn_munkres.h>

#include <cmath>
#include <numbers>
#include <queue>

static int INF = std::numeric_limits<int>::max();

namespace wzj {
kuhn_munkres::kuhn_munkres(int n)
    : n_(n), lx_(n), ly_(n), ma_(n, -1), mb_(n, -1) {
  w_ = std::vector<std::vector<int>>(n_, std::vector<int>(n_));
}

std::vector<std::pair<int, int>> kuhn_munkres::maximum_weight(int* weight) {
  // 初始化顶标
  for (int i = 0; i < n_; ++i)
    for (auto w : w_[i]) lx_[i] = std::max(lx_[i], w);
  path_.assign(n_, -1);

  for (int u = 00; u < n_; ++u) {
    slack_.assign(n_, INF);
    vx_.assign(n_, false);
    vy_.assign(n_, false);
    _find_augmenting_path(u);
  }

  //
  std::vector<std::pair<int, int>> matches;
  for (int u = 0; u < n_; ++u)
    if (ma_[u] != -1 && w_[u][ma_[u]] > 0) {
      matches.emplace_back(u, ma_[u]);
      if (weight != nullptr) *weight += w_[u][ma_[u]];
    }
  return matches;
}

void kuhn_munkres::_find_augmenting_path(int u) {
  std::queue<int> q;
  q.push(u);

  vx_[u] = true;
  while (true) {
    if (_tree(q)) return;

    // 没有增广路, 修改顶标.

    int a = INF;
    for (int v = 0; v < n_; ++v)
      if (slack_[v]) a = std::min(a, slack_[v]);  // v ∈ T'
    for (int i = 0; i < n_; ++i) {
      // S - a
      if (vx_[i]) lx_[i] -= a;
      // T + a
      if (vy_[i]) ly_[i] += a;
      // slack是S到T', S-a会使得slack也要-a
      else
        slack_[i] -= a;
    }
    // 因为上面调整了slack, 所以看看有没有==0的, 可以放入相等子图的
    for (int v = 0; v < n_; ++v)
      if (!vy_[v] && slack_[v] == 0 && _check(v, q))
        return;  // (u,v)这条边形成了新的增广路
  }
}

bool kuhn_munkres::_check(int v, std::queue<int>& q) {
  vy_[v] = true;
  if (mb_[v] != -1) {
    q.push(mb_[v]);
    return false;  // v不是未匹配点, 直接把v->u放入交替树中
  }
  // v是未匹配点. 则表示在当前的交替树中, 找到了一条增广路.
  // 在此处记录答案, 反转增广路即可
  while (v != -1) {
    mb_[v] = path_[v];
    std::swap(v, ma_[path_[v]]);
  }
  return true;
}

bool kuhn_munkres::_tree(std::queue<int>& q) {
  while (!q.empty()) {
    int u = q.front();
    q.pop();
    vx_[u] = true;
    for (int v = 0; v < n_; ++v) {
      if (vy_[v]) continue;
      path_[v] = u;  // 记录交替树上到达v的路径
      int delta = lx_[u] + ly_[v] - w_[u][v];
      if (delta <= slack_[v]) {
        if (delta)
          slack_[v] = delta;
        else if (_check(v, q))  // delta=0, 放入相等子图
          return true;          // (u,v)这条边形成了新的增广路
      }
    }
  }

  return false;  // 没找到增广路
}

}  // namespace wzj