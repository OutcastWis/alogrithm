#include <string>
#include <utility>

namespace wzj {
class splay {
 public:
  void insert(const std::string& key, int value);
  bool erase(const std::string& key);
  bool has(const std::string& key);  // 因为会调整树结构, 所以不是 const 函数
  std::pair<std::string, int> kth(int k);  // 返回第k小的元素, k从1开始计数

 private:
  struct node {
    node* parent = nullptr;
    node* left = nullptr;
    node* right = nullptr;
    int size = 1;  // kth函数需要该值

    std::string key;
    int value;
  };

  void _splay(node** z, node* x);  // 把z子树中的x节点旋转到z. z会被更新为x
  void _rotate(node* x);
  void _zig(node* x);                     // 左旋, x是p的左子节点
  void _zag(node* x);                     // 右旋, x是p的右子节点
  node* _merge(node* left, node* right);  // 合并两棵子树, 返回合并后的树根
  void _update(node* x);                  // 旋转后, 更新节点x的数据, 例如size

  // 在r为根的树中查找key, 返回对应节点指针, 若不存在则返回最后访问的节点指针
  void _find(node** r, const std::string& key);
  // 返回以cur为根的子树中第k小的节点, 并移动到根. k从1开始计数.
  void _kth(node** r, int k);

  node* root_ = nullptr;
};
}  // namespace wzj