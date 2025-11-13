#include <string>

namespace wzj {
/*
 * treap, 2叉平衡树 + 堆
 * 1. 这里的实现, 使用小根堆. node.rank来维护堆序性质. 即父节点rank小于子节点rank
 * 2. treap的单次旋转和splay一样, 都是标准的BST旋转操作 .但条件和意向不用.
 * 前者为了维护堆, 后者为了把访问的节点旋转到根节点
 * */
class treap {
 public:
  void insert(const std::string& key, int value);
  bool erase(const std::string& key);
  bool has(const std::string& key) const;
  int get_rank(const std::string& key) const; // 返回key的排名, 即<=key的节点数
  std::pair<std::string, int> kth(int r) const; // 返回第k小的值, k从1开始
 private:
  struct node {
    node* left = nullptr;
    node* right = nullptr;
    int size = 1;

    int rank;
    std::string key;
    int value;
  };

  void _rotate(node** n, bool left);  // left:true表示旋转n为根的左子树, 是右旋
  void _update(node* n) const;
  void _insert(node** n, const std::string& key, int value);
  bool _erase(node** n, const std::string& key);
  node* _find(node* n, const std::string& key) const;
  int _find_rank(node* n, const std::string& key) const;
  node* _find_value(node* n, int r) const;

  node* root_ = nullptr;
};
}  // namespace wzj