#include <tree/fhq_treap.h>

#include <cassert>
#include <random>

namespace wzj {

void fhq_treap::insert(int key) {
  node *l, *e, *r;
  std::tie(l, r) = _split(root_, key);  // l<=key , r>key
  std::tie(l, e) = _split(l, key - 1);  // l<=key-1, key>=e>key -1

  if (e == nullptr)
    e = new node{nullptr, nullptr, 1, false, std::rand(), key};  // 新节点
  // 合并
  node* t = _merge(l, e);
  root_ = _merge(t, r);
}

bool fhq_treap::has(int key) {
  node *l, *e, *r;
  std::tie(l, r) = _split(root_, key);  // l<=key , r>key
  std::tie(l, e) = _split(l, key - 1);  // l<=key-1, key>=e>key -1
  bool ans = e != nullptr;

  root_ = _merge(_merge(l, e), r);

  return ans;
}

bool fhq_treap::erase(int key) {
  node *l, *e, *r;
  std::tie(l, r) = _split(root_, key);  // l<=key , r>key
  std::tie(l, e) = _split(l, key - 1);  // l<=key-1, key>=e>key -1
  bool ans = e != nullptr;
  if (ans) delete e;

  root_ = _merge(l, r);
  return ans;
}

int fhq_treap::kth(int k) {
  node *l, *e, *r;
  std::tie(l, e, r) = _split_kth(root_, k);
  assert(e);

  root_ = _merge(_merge(l, e), r);
  return e->key;
}

void fhq_treap::insert_range(int kl, int key) {
  node *l, *e, *r;
  std::tie(l, e, r) =
      _split_kth(root_, kl + 1);  // kl从0开始, 但_split_kth从1开始
  node* new_node = new node{nullptr, nullptr, 1, false, std::rand(), key};
  root_ = _merge(l, _merge(_merge(new_node, e), r));
}

void fhq_treap::reverse(int kl, int kr) {
  node *l, *r, *mid;
  // 区间[kl, kr]对应排名[kl+1, kr+1]
  std::tie(l, r) =
      _split_kth_2(root_, kl);  // l = 排名[1, kl]， r = 排名[kl+1, :]
  std::tie(mid, r) = _split_kth_2(r, kr + 1 - kl);  // mid= 排名[kl+1, kr+1]
  assert(mid->size == kr - kl + 1);
  mid->reverse = true;
  root_ = _merge(_merge(l, mid), r);
}

void fhq_treap::_down(node* n) const {
  if (!n->reverse) return;
  n->reverse = false;
  std::swap(n->left, n->right);
  if (n->left) n->left->reverse ^= 1;
  if (n->right) n->right->reverse ^= 1;
}

void fhq_treap::_update(node* n) const {
  n->size = 1;
  if (n->left) n->size += n->left->size;
  if (n->right) n->size += n->right->size;
}

std::tuple<fhq_treap::node*, fhq_treap::node*> fhq_treap::_split(
    node* cur, int key) const {
  if (cur == nullptr) return {nullptr, nullptr};

  // 分割, 使得cur和左子树<=key, 右子树>key
  node *sl, *sr;
  if (key < cur->key) {
    std::tie(sl, sr) = _split(cur->left, key);
    cur->left = sr;
    _update(cur);
    return {sl, cur};
  } else {
    std::tie(sl, sr) = _split(cur->right, key);
    cur->right = sl;
    _update(cur);
    return {cur, sr};
  }
}

std::tuple<fhq_treap::node*, fhq_treap::node*, fhq_treap::node*>
fhq_treap::_split_kth(node* cur, int k) const {
  if (cur == nullptr) return {nullptr, nullptr, nullptr};

  _down(cur);

  int left_size = cur->left ? cur->left->size : 0;
  node *less, *equal, *more;
  if (k == left_size + 1) {
    less = cur->left, more = cur->right;
    cur->left = cur->right = nullptr;
    return {less, cur, more};
  } else if (k <= left_size) {
    std::tie(less, equal, more) = _split_kth(cur->left, k);
    cur->left = more;
    _update(cur);
    return {less, equal, cur};
  } else {
    std::tie(less, equal, more) = _split_kth(cur->right, k - left_size - 1);
    cur->right = less;
    _update(cur);
    return {cur, equal, more};
  }
}

std::tuple<fhq_treap::node*, fhq_treap::node*> fhq_treap::_split_kth_2(
    node* cur, int k) const {
  if (cur == nullptr) return {nullptr, nullptr};
  _down(cur);

  int left_size = cur->left ? cur->left->size : 0;
  node *left, *right;
  if (k <= left_size) {
    std::tie(left, right) = _split_kth_2(cur->left, k);
    cur->left = right;
    _update(cur);
    return {left, cur};
  } else {
    std::tie(left, right) = _split_kth_2(cur->right, k - left_size - 1);
    cur->right = left;
    _update(cur);
    return {cur, right};
  }
}

fhq_treap::node* fhq_treap::_merge(node* l, node* r) const {
  if (l == nullptr) return r;
  if (r == nullptr) return l;

  _down(l), _down(r);
  if (l->rank < r->rank) {
    l->right = _merge(l->right, r);
    _update(l);
    return l;
  } else {
    r->left = _merge(l, r->left);
    _update(r);
    return r;
  }
}
}  // namespace wzj
