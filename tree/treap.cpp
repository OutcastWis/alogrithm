#include <tree/treap.h>

#include <cassert>
#include <random>

namespace wzj {
void treap::insert(const std::string& key, int value) {
  _insert(&root_, key, value);
}

bool treap::erase(const std::string& key) { return _erase(&root_, key); }

bool treap::has(const std::string& key) const {
  return _find(root_, key) != nullptr;
}

int treap::get_rank(const std::string& key) const {
  return _find_rank(root_, key);
}

std::pair<std::string, int> treap::kth(int r) const {
  node* n = _find_value(root_, r);
  assert(n != nullptr);
  return {n->key, n->value};
}

void treap::_rotate(node** n, bool left) {
  node* kid = left ? (*n)->left : (*n)->right;
  if (left) {
    (*n)->left = kid->right;
    kid->right = *n;
  } else {
    (*n)->right = kid->left;
    kid->left = *n;
  }
  _update(*n);
  _update(kid);

  *n = kid;
}

void treap::_update(node* n) const {
  n->size = 1;
  if (n->left) n->size += n->left->size;
  if (n->right) n->size += n->right->size;
}

void treap::_insert(node** n, const std::string& key, int value) {
  if (*n == nullptr) {
    *n = new node{nullptr, nullptr, 1, std::rand(), key, value};
    return;
  }

  if (key == (*n)->key) {
    (*n)->value = value;  // 更新值
    return;
  } else if (key < (*n)->key) {
    _insert(&(*n)->left, key, value);
    if ((*n)->left->rank < (*n)->rank) _rotate(n, true);
    _update(*n);
  } else {
    _insert(&(*n)->right, key, value);
    if ((*n)->right->rank < (*n)->rank) _rotate(n, false);
    _update(*n);
  }
}

bool treap::_erase(node** n, const std::string& key) {
  if (n == nullptr) return false;

  if (key < (*n)->key) {
    bool r = _erase(&(*n)->left, key);
    _update(*n);
    return r;
  } else if (key > (*n)->key) {
    bool r = _erase(&(*n)->right, key);
    _update(*n);
    return r;
  }
  // 找到要删除的节点
  if ((*n)->left == nullptr && (*n)->right == nullptr) {
    delete *n;
    *n = nullptr;
    return true;
  } else if ((*n)->left == nullptr || (*n)->right == nullptr) {
    node* to_delete = *n;
    *n = (*n)->left == nullptr ? (*n)->right : (*n)->left;
    delete to_delete;
    return true;
  } else {
    // 两个子节点都存在, 选择rank较小的子节点旋转上来
    if ((*n)->left->rank < (*n)->right->rank) {
      _rotate(n, true);
      _erase(&(*n)->right, key);
    } else {
      _rotate(n, false);
      _erase(&(*n)->left, key);
    }
    _update(*n);
    return true;
  }
}

treap::node* treap::_find(node* n, const std::string& key) const {
  if (n == nullptr) return nullptr;

  if (key == n->key) {
    return n;
  } else if (key < n->key) {
    return _find(n->left, key);
  } else {
    return _find(n->right, key);
  }
}

int treap::_find_rank(node* n, const std::string& key) const {
  if (n == nullptr) return 0;
  int left_rank = n->left ? n->left->size : 0;
  if (key == n->key)
    return left_rank + 1;
  else if (key < n->key)
    return _find_rank(n->left, key);
  else
    return left_rank + 1 + _find_rank(n->right, key);
}

treap::node* treap::_find_value(node* n, int r) const {
  if (n == nullptr) return nullptr;

  int left_size = n->left ? n->left->size : 0;
  if (r == left_size + 1)
    return n;
  else if (r <= left_size)
    return _find_value(n->left, r);
  else
    return _find_value(n->right, r - 1 - left_size);
}

}  // namespace wzj