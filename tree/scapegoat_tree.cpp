#include <tree/scapegoat_tree.h>

#include <algorithm>

namespace wzj {

void scapegoat_tree::insert(const std::string& key, int value) {
  _insert(key, value, &root_, 1);
  ++n_;
}

bool scapegoat_tree::erase(const std::string& key) {
  node* res = _find(key);
  if (res != nullptr && !res->deleted) {
    res->deleted = true;
    --n_;
    if (n_ < alpha_ * root_->size) {
      // 重构整棵树
      _rebuild(&root_);
    }
  }
  return res != nullptr && !res->deleted;
}

bool scapegoat_tree::has(const std::string& key) const {
  node* res = _find(key);
  return res != nullptr && !res->deleted;
}

void scapegoat_tree::_rebuild(node** cur) {
  std::vector<node*> nodes;
  nodes.reserve((*cur)->size);
  // 中序遍历收集节点
  _inorder(*cur, nodes);
  // 递归构建平衡树
  _build(cur, nodes, 0, nodes.size() - 1);
}

bool scapegoat_tree::_insert(const std::string& key, int value, node** cur,
                             int depth) {
  if (*cur == nullptr) {
    *cur = new node{nullptr, nullptr, false, 1, key, value};
    return depth > _ha(root_->size + 1);  // 高度h不满足α高度平衡
  }

  bool need_rebuild = false;
  if ((*cur)->key == key) {
    (*cur)->value = value;
    (*cur)->deleted = false;
    return false;
  } else if (key < (*cur)->key) {
    need_rebuild = _insert(key, value, &((*cur)->left), depth + 1);
  } else {
    need_rebuild = _insert(key, value, &((*cur)->right), depth + 1);
  }
  // update size
  (*cur)->size = ((*cur)->left ? (*cur)->left->size : 0) +
                 ((*cur)->right ? (*cur)->right->size : 0) +
                 ((*cur)->deleted ? 0 : 1);

  if (need_rebuild) {
    // 找到替罪羊节点. 第一个不满足α权重平衡的结点即为替罪羊, 即子节点大小超过α
    // * 父节点大小
    if ((*cur)->left && ((*cur)->left->size > alpha_ * (*cur)->size) ||
        (*cur)->right && ((*cur)->right->size > alpha_ * (*cur)->size)) {
      _rebuild(cur);
      return false;
    }
  }

  return need_rebuild;
}

void scapegoat_tree::_inorder(node* cur, std::vector<node*>& nodes) {
  if (cur == nullptr) return;
  _inorder(cur->left, nodes);
  if (!cur->deleted) nodes.push_back(cur);
  _inorder(cur->right, nodes);
}

void scapegoat_tree::_build(node** cur, std::vector<node*>& nodes, int left,
                            int right) {
  int mid = left + ((right - left) >> 1);
  *cur = nodes[mid];
  (*cur)->left = nullptr;
  (*cur)->right = nullptr;
  (*cur)->size = right - left + 1;
  if (mid > left) _build(&((*cur)->left), nodes, left, mid - 1);
  if (mid < right) _build(&((*cur)->right), nodes, mid + 1, right);
}

scapegoat_tree::node* scapegoat_tree::_find(const std::string& key) const {
  node* cur = root_;
  while (cur != nullptr) {
    if (cur->key == key) {
      return cur;
    } else if (key < cur->key) {
      cur = cur->left;
    } else {
      cur = cur->right;
    }
  }

  return nullptr;
}
}  // namespace wzj