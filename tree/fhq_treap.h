#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace wzj {
// 范浩强 Treap/无旋 Treap
// 1. 使用分裂, 合并来实现插入删除查找. 为了演示, 这里key是int
// 2. 无旋好处是可以实现区间操作, 例如插入区间, 翻转
class fhq_treap {
 public:
  void insert(int key);
  bool has(int key);
  bool erase(int key); 
  int kth(int k) ; 

  // 和区间相关的操作. 传参表示的都是排名,而不是key. 
  // 一般而言, 区间操作后, 上面普通BST行为的函数就失效了

  // kl从0开始, 在kl位置插入key
  void insert_range(int kl, int key); 
  // 翻转[kl, kr], kl从0开始
  void reverse(int kl, int kr);
 private:
  struct node {
    node* left = nullptr;
    node* right = nullptr;
    int size = 1;
    bool reverse = false; // 翻转标记, 用于区间操作

    int rank;
    int key;
  };

  void _down(node* x) const; // 向下传递reverse标记
  void _update(node* x) const;
  // 按值分裂. {左treap, 右treap}. 左<=key, 右>key
  std::tuple<node*, node*> _split(node* cur, int key) const;
  // 按排名分裂. {<, =, >}. k从1开始. =是节点. <和>是树
  std::tuple<node*, node*, node*> _split_kth(node* cur, int k) const;
  // 按排名分裂. {<=, >}, k从1开始.
  std::tuple<node*, node*> _split_kth_2(node* cur, int k) const;
  // 合并. l,r是分裂出来的左右子树, 即保证了l.key<r.key
  node* _merge(node* l, node* r) const;


  node* root_ = nullptr;
};
}  // namespace wzj