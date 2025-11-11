#include <cmath>
#include <string>
#include <utility>
#include <vector>

namespace wzj {
// 替罪羊树
// 1. 二叉树
// 2. 总能保证宽松的alpha高度平衡, 即高度h <= log_{1/α}(n)
// 3. 不会旋转, 只在必要时候进行子树重构
class scapegoat_tree {
 public:
  // α越大插入速度就越快，而访问和删除速度就会降低。反之则插入变慢
  scapegoat_tree(double alpha = 0.75) : alpha_(alpha) {
    ha_base_ = 1.0 / std::log2(1.0 / alpha_);
  }
  ~scapegoat_tree() = default;

 public:
  void insert(const std::string& key, int value);
  bool erase(const std::string& key);
  bool has(const std::string& key) const;

 private:
  struct node {
    node* left = nullptr;
    node* right = nullptr;
    bool deleted = false;
    int size = 1;  // 子树大小, 含自身

    std::string key;
    int value;
  };
  // 重构以cur为根节点的子树
  void _rebuild(node** cur);
  bool _insert(const std::string& key, int value, node** cur, int depth);
  void _inorder(node* cur, std::vector<node*>& nodes);
  void _build(node** cur, std::vector<node*>& nodes, int left, int right);
  node* _find(const std::string& key) const;
  double _ha(int n) const {
    // ha = log_{1/α}(n)
    return std::log2(n) * ha_base_;
  }

  node* root_ = nullptr;
  int n_ = 0;        // 当前有效节点数
  double alpha_;
  double ha_base_;
};
}  // namespace wzj