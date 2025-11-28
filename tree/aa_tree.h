#include <string>
#include <utility>

namespace wzj {
/// @brief aa树
///
/// 遵守和rb-tree一样的约束, 同时额外增加一个: 红不能是左孩子
///
/// 每个节点维护level字段, 对应红黑树的黑高度:
/// 1. 叶节点level=1
/// 2. 左孩子是父level-1
/// 3. 右孩子=父level or 父level-1
/// 4. 右孙子<祖父level
/// 5. 每个level > 1的节点, 有俩孩子
/// 
/// 对应红黑树的描述就是(黑高=level):
/// 1. 叶子节点的黑高=1
/// 2. AA树的左孩子只能是黑的, 所以黑高一定比父亲的黑高少1
/// 3. 右孩子可能红或黑, 所以红则黑高相等, 黑则少1
/// 4. 祖父到右孙子路径不可能是连续的红, 所以黑高一定小
/// 5. 黑高>1, 意味着不可能直连nil节点, 所以一定有俩孩子

class aa_tree {
 public:
  void insert(const std::string& key);
  bool has(const std::string& key) const;
  void erase(const std::string& key);

  std::pair<bool, std::string> lower_bound(const std::string& key) const;

 private:
  struct node {
    node* left;
    node* right;
    int level;

    std::string key;
  };

  node* _lower_bound(node* cur, const std::string& key) const;
  node* _succ(node* cur) const;
  /// @brief 标准的bst旋转
  /// @param cur 子树根
  /// @param left true表示旋转cur为根的左子树, 是右旋
  void _rotate(node** cur, bool left);  
  /// @brief 左旋, 出现连续向右的水平方向链, 即右侧连续红
  /// @param cur cur, cur->right, cur->right->right构成水平链
  void _split(node** cur);
  /// @brief 右旋, 出现向左的水平方向链, 即左侧红
  /// @param cur cur, cur->left水平
  void _skew(node** cur);
  /// @brief 插入. 插入的节点level==1, 等价于红黑树插入一个红节点
  void _insert(node** cur, const std::string& key);
  /// @brief 删除. 和BST删除差不多, 也是转化成叶子结点的删除. 回溯时候需要调整level
  void _erase(node** cur, const std::string& key);
  void _decrease_level(node* cur);

  node* root_ = nullptr;
};

}  // namespace wzj