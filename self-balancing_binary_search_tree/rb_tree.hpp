#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>

/*
1. Every node is either red or black.
2. All NIL nodes (figure 1) are considered black.
3. A red node does not have a red child.
4. Every path from a given node to any of its descendant NIL nodes goes through
the same number of black nodes.
*/

namespace wzj {

enum _rb_tree_color { red = true, black = false };

typedef std::pair<std::string, int> _rb_tree_value_type;

struct _rb_tree_node {
  _rb_tree_color color_ = red;  // default red
  _rb_tree_value_type kv_;
  _rb_tree_node *parent_ = nullptr;
  _rb_tree_node *left_ = nullptr;
  _rb_tree_node *right_ = nullptr;

  static _rb_tree_node *left_most(_rb_tree_node *x) {
    while (x->left_) x = x->left_;
    return x;
  }

  static _rb_tree_node *right_most(_rb_tree_node *x) {
    while (x->right_) x = x->right_;
    return x;
  }

  static _rb_tree_node *next(_rb_tree_node *x) {
    if (x->right_) return left_most(x->right_);
    while (x == x->parent_->right_) {
      x = x->parent_;
    }

    if (x->right_ == x->parent_)
      return x;  // 此时x是header_, 且只有一个节点, 则该节点是最大的,
                 // next应该返回end(), 即header_

    return x->parent_;  // 普通情况
  }

  static _rb_tree_node *prev(_rb_tree_node *x) {
    if (x->color_ == red && x->parent_->parent_ == x)
      return x->right_;  // x为header_, 即end(), 则end()--应该返回最大值

    if (x->left_) return right_most(x->left_);
    while (x == x->parent_->left_) {
      x = x->parent_;
    }
    // c++标准中说道:
    // ![--begin()](https://www.apiref.com/cpp/cpp/named_req/BidirectionalIterator.html)
    // The begin iterator is not decrementable and the behavior is undefined if
    // --container.begin() is evaluated. 所以这边和next不一样,
    // 不用对单个节点时特判. 这样的处理会造成--begin()=begin()
    return x->parent_;  // 普通情况
  }
};

struct _rb_tree_iterator {
  typedef _rb_tree_value_type *pointer;
  typedef _rb_tree_value_type &reference;

  typedef _rb_tree_iterator _Self;

  _rb_tree_iterator() = default;
  explicit _rb_tree_iterator(_rb_tree_node *x) : node_(x) {}

  pointer operator->() const { return &node_->kv_; }
  reference operator*() const { return node_->kv_; }

  _Self operator++() {
    node_ = _rb_tree_node::next(node_);
    return *this;
  }
  _Self operator++(int) {
    _Self tmp = *this;
    node_ = _rb_tree_node::next(node_);
    return tmp;
  }

  _Self operator--() {
    node_ = _rb_tree_node::prev(node_);
    return *this;
  }
  _Self operator--(int) {
    _Self tmp = *this;
    node_ = _rb_tree_node::prev(node_);
    return tmp;
  }

  friend bool operator==(const _rb_tree_iterator &x,
                         const _rb_tree_iterator &y) {
    return x.node_ == y.node_;
  }
  friend bool operator!=(const _rb_tree_iterator &x,
                         const _rb_tree_iterator &y) {
    return x.node_ != y.node_;
  }

  _rb_tree_node *node_;
};

class rb_tree {
 public:
  typedef _rb_tree_iterator iterator;

 public:
  rb_tree() {
    header_.color_ = red;
    header_.left_ = header_.right_ = &header_;
    header_.parent_ = nullptr;
  }

  std::pair<iterator, bool> insert(const std::string &key, int value) {
    auto r = locate_(key);
    if (r.second) {
      return std::pair<iterator, bool>(insert_(r.second, key, value), true);
    }

    return std::pair<iterator, bool>(iterator(r.first), false);
  }

  iterator begin() { return iterator(header_.left_); }

  iterator end() { return iterator(&header_); }

 private:
  std::pair<_rb_tree_node *, _rb_tree_node *> locate_(
      const std::string &__key) {
    _rb_tree_node *x = header_.parent_, *y = &header_;
    bool comp = true;
    while (x != nullptr) {
      y = x;
      comp = __key < x->kv_.first;
      x = comp ? x->left_ : x->right_;
    }

    auto may_equal = y;
    if (comp) {
      // 可能key < y.key, 但key == y.prev.key
      if (may_equal ==
          header_.left_)  // begin()没有prev, 特判. 同时如果空树, 也会走这里
        return std::pair<_rb_tree_node *, _rb_tree_node *>(nullptr, y);
      may_equal = _rb_tree_node::prev(may_equal);
    }

    if (may_equal->kv_.first < __key)
      return std::pair<_rb_tree_node *, _rb_tree_node *>(nullptr, y);
    // 相等
    return std::pair<_rb_tree_node *, _rb_tree_node *>(may_equal, nullptr);
  }

  _rb_tree_node *insert_(_rb_tree_node *__p, const std::string &__key,
                         int __value) {
    bool is_left = __p == &header_ || __key < __p->kv_.first;
    _rb_tree_node *x = new _rb_tree_node;
    x->kv_.first = __key;
    x->kv_.second = __value;
    x->parent_ = __p;

    if (is_left) {
      __p->left_ = x;
      if (__p == &header_) {
        // 空树. 根据locate_, 空树时, is_left一定是true
        header_.right_ = __p;
        header_.parent_ = __p;
      } else if (__p == header_.left_)
        header_.left_ = x;
    } else {
      __p->right_ = x;
      if (__p == header_.right_) header_.right_ = x;
    }

    rebalance_(x, __p);
    ++num_;
    return x;
  }

  void rebalance_(_rb_tree_node *__x, _rb_tree_node *__p) {
    _rb_tree_node *g = nullptr, *uncle;
    do {
      // Case 1: parent is black
      if (__p->color_ == black) return;

      if ((g = __p->parent_) == nullptr) {
        // Case 4: parent is root and red
        __p->color_ = black;
        return;
      }

      uncle = g->left_ == __p ? g->right_ : g->left_;
      if (uncle == nullptr || uncle->color_ == black) break;
      // Case 2: parent and uncle are both red
      __p->color_ = black;
      uncle->color_ = black;
      g->color_ = red;
      __x = g;
    } while (__p = __x->parent_);

    // Case_I3: N is the root and red.
    if (__p == nullptr) return;

    // parent is red, uncle is black.
    bool is_left = g->left_ == __p;
    bool is_right = __p->right_ == __x;
    if (is_left == is_right) {
      // Case 5: x, p, g形成了折角
      rotate_(__p, is_left);  // 旋转,将x转到g的外面
      __x = __p;
      __p = is_left ? g->left_ : g->right_;
    }
    // Case 6:
    rotate_(g, !is_left);
    __p->color_ = black;
    g->color_ = red;
  }

  // __left == true表示左旋, false表示右旋
  void rotate_(_rb_tree_node *__p, bool __left) {
    auto g = __p->parent_;
    _rb_tree_node *s = nullptr, *c = nullptr;
    if (__left) {
      s = __p->right_;
      c = s->left_;
      __p->right_ = c;
      if (c) c->parent_ = __p;
      s->left_ = __p;
      s->parent_ = g;
      __p->parent_ = s;
    } else {
      s = __p->left_;
      c = s->right_;
      __p->left_ = c;
      if (c) c->parent_ = __p;
      s->right_ = __p;
      s->parent_ = g;
      __p->parent_ = s;
    }

    if (g)
      __p == g->left_ ? (g->left_ = s) : (g->right_ = s);
    else
      header_.parent_ = s;
  }

  _rb_tree_node header_;  // 方便找最大, 最小. header->left_指向最小值,
                          // header_->right_指向最大值
  size_t num_;
};

}  // namespace wzj