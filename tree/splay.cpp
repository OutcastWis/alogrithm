#include <tree/splay.h>

#include <cassert>

namespace wzj {
void splay::insert(const std::string& key, int value) {
  node *cur = root_, *p = nullptr;
  for (; cur != nullptr;) {
    p = cur;
    if (cur->key == key)
      break;
    else if (cur->key < key)
      cur = cur->right;
    else
      cur = cur->left;
  }

  if (cur != nullptr) {
    // key已存在, 更新value
    cur->value = value;
    _splay(&root_, cur);
    return;
  }
  // 插入新节点
  node* new_node = new node{p, nullptr, nullptr, 1, key, value};
  if (p != nullptr) {
    if (p->key < key)
      p->right = new_node;
    else
      p->left = new_node;
  }
  // 移到根部
  _splay(&root_, new_node);
}

bool splay::erase(const std::string& key) {
  if (!has(key)) return false;
  // 合并左右子树
  node *left = root_->left, *right = root_->right;
  delete root_;
  if (left) left->parent = nullptr;
  if (right) right->parent = nullptr;
  root_ = _merge(left, right);
  return true;
}

bool splay::has(const std::string& key) {
  _find(&root_, key);
  return root_ != nullptr && root_->key == key;
}

std::pair<std::string, int> splay::kth(int k) {
  _kth(&root_, k);
  return {root_->key, root_->value};
}

void splay::_splay(node** z, node* x) {
  node* pz = *z == nullptr ? nullptr : (*z)->parent;
  for (auto p = x->parent; p != pz; _rotate(x), p = x->parent) {
    if (p->parent != pz) {
      if ((p->left == x) == (p->parent->left == p))
        _rotate(p);  // zig-zig or zag-zag
      else
        _rotate(x);  // zir-zag or zag-zig
    }
  }
  *z = x;
}

void splay::_rotate(node* x) {
  node* p = x->parent;
  if (p->left == x) {
    _zig(x);
  } else {
    _zag(x);
  }
}

void splay::_zig(node* x) {
  node *p = x->parent, *pp = p->parent;
  if (pp != nullptr) {
    x->parent = pp;
    if (pp->left == p)
      pp->left = x;
    else
      pp->right = x;
  }
  p->parent = x;
  x->parent = pp;
  if (x->right) x->right->parent = p;
  p->left = x->right;
  x->right = p;

  _update(p);
  _update(x);
}

void splay::_zag(node* x) {
  node *p = x->parent, *pp = p->parent;
  if (pp != nullptr) {
    x->parent = pp;
    if (pp->left == p)
      pp->left = x;
    else
      pp->right = x;
  }
  p->parent = x;
  x->parent = pp;
  if (x->left) x->left->parent = p;
  p->right = x->left;
  x->left = p;

  _update(p);
  _update(x);
}

splay::node* splay::_merge(node* left, node* right) {
  if (!left) return right;
  if (!right) return left;

  _kth(&right, 1);
  right->left = left;
  left->parent = right;
  _update(right);

  return right;
}

void splay::_update(node* x) {
  x->size = 1;
  if (x->left) x->size += x->left->size;
  if (x->right) x->size += x->right->size;
}

void splay::_find(node** r, const std::string& key) {
  node* cur = *r;
  node* y = cur->parent;
  for (; cur != nullptr; cur = key < y->key ? y->left : y->right) {
    y = cur;
    if (cur->key == key) break;
  }
  // 应注意，经常存在树中不存在相应的节点的情形。对于这种情形，要记录最后一个访问的节点并上移至根部
  _splay(r, cur ? cur : y);
}

void splay::_kth(node** r, int k) {
  assert(k >= 1 && k <= (*r)->size);

  node* cur = *r;
  for (;;) {
    int left_size = cur->left ? cur->left->size : 0;
    int right_size = cur->right ? cur->right->size : 0;
    if (left_size >= k)
      cur = cur->left;
    else if (k == left_size + 1)
      break;
    else {
      k -= left_size + 1;
      cur = cur->right;
    }
  }
  // 上移到根部
  _splay(r, cur);
}

}  // namespace wzj