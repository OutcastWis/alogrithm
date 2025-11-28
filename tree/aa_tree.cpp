#include <tree/aa_tree.h>

namespace wzj {

void aa_tree::insert(const std::string& key) { _insert(&root_, key); }

bool aa_tree::has(const std::string& key) const {
  node* r = root_;
  while (r) {
    if (r->key == key)
      break;
    else if (r->key < key)
      r = r->right;
    else
      r = r->left;
  }

  return r != nullptr;
}

void aa_tree::erase(const std::string& key) { _erase(&root_, key); }

std::pair<bool, std::string> aa_tree::lower_bound(
    const std::string& key) const {
  auto r = _lower_bound(root_, key);
  return r ? std::make_pair(true, r->key) : std::make_pair(false, "");
}

aa_tree::node* aa_tree::_lower_bound(node* cur, const std::string& key) const {
  if (cur == nullptr) return nullptr;
  if (cur->key == key) return cur;

  node* r = nullptr;
  if (cur->key < key)
    r = _lower_bound(cur->right, key);
  else
    r = _lower_bound(cur->left, key);

  if (r == nullptr && key < cur->key) return cur;

  return r;
}

aa_tree::node* aa_tree::_succ(node* cur) const {
  if (cur->right) {
    cur = cur->right;
    while (cur->left) cur = cur->left;
  }
  return cur;
}

void aa_tree::_rotate(node** cur, bool left) {
  node* kid = left ? (*cur)->left : (*cur)->right;
  if (left) {
    (*cur)->left = kid->right;
    kid->right = *cur;
  } else {
    (*cur)->right = kid->left;
    kid->left = *cur;
  }
  *cur = kid;
}

void aa_tree::_split(node** cur) {
  if (!(*cur)->right || !(*cur)->right->right) return;

  if ((*cur)->level == (*cur)->right->right->level) {
    _rotate(cur, false);  // 左旋. 如此, 原cur变左孩子, 原cur->right变根
    (*cur)->level += 1;   // 此时cur指向原cur->right. 增加level, 维持节点约束3:
                          // 右孩子=父level or 父level-1
  }
}

void aa_tree::_skew(node** cur) {
  if ((*cur)->left && (*cur)->level == (*cur)->left->level) {
    _rotate(cur, true);  // 右旋. 如此, 原cur->left变根, 原cur变成右孩子
  }
}

void aa_tree::_insert(node** cur, const std::string& key) {
  if ((*cur) == nullptr) {
    *cur = new node{nullptr, nullptr, 1, key};
    return;
  }

  if (key < (*cur)->key)
    _insert(&(*cur)->left, key);
  else if (key > (*cur)->key)
    _insert(&(*cur)->right, key);
  else {
    // nothing to do
  }
  // 调整
  _skew(cur);
  _split(cur);
}

void aa_tree::_erase(node** cur, const std::string& key) {
  if ((*cur)->key < key)
    _erase(&(*cur)->right, key);
  else if ((*cur)->key > key)
    _erase(&(*cur)->left, key);
  else {
    node* swap = nullptr;
    if ((*cur)->left != nullptr) {
      node* swap = (*cur)->left;
      while (swap->right) swap = swap->right;  // swap是左子树最大值
      (*cur)->key = swap->key;
      _erase(&(*cur)->left, swap->key);
    } else if ((*cur)->right != nullptr) {
      node* swap = (*cur)->right;
      while (swap->left) swap = swap->left;  // swap是右子树最小值
      (*cur)->key = swap->key;
      _erase(&(*cur)->right, swap->key);
    } else {
      swap = *cur;
      *cur = nullptr;
    }

    delete swap;
  }

  if (*cur == nullptr) return;

  // re-balance. 和红黑树比, 删除简单很多.
  // 1. 减低level
  // 2. 3次skew. x, x.right, x.right.right
  // 3. 2次split. x, x.right
  _decrease_level(*cur);  // 子树中有节点被删除, 尝试降低level
  _skew(cur);
  if ((*cur)->right) {
    _skew(&(*cur)->right);
    if ((*cur)->right->right) _skew(&(*cur)->right->right);
  }
  _split(cur);
  if ((*cur)->right) _split(&(*cur)->right);
}

void aa_tree::_decrease_level(node* cur) {
  if (cur == nullptr) return;

  int should_be = std::min(cur->left ? cur->left->level : 0,
                           cur->right ? cur->right->level : 0) +
                  1;

  if (should_be < cur->level) {
    cur->level = should_be;
    if (cur->right && cur->right->level > should_be)
      cur->right->level = should_be;
  }
}
}  // namespace wzj