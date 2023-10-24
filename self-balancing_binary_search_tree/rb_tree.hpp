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
  rb_tree() : num_(0) {
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

  size_t erase(const std::string &key) {
    auto r = locate_(key);
    if (r.first) {
      delete_(r.first);
      delete r.first;
      --num_;
      return 1;
    }
    return 0;
  }

  iterator erase(iterator it) {
    auto r = it;
    ++r;
    delete_(it.node_);
    delete it.node_;
    --num_;
    return r;
  }

  iterator begin() { return iterator(header_.left_); }

  iterator end() { return iterator(&header_); }

  size_t size() const { return num_; }

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
      if (may_equal == header_.left_)
        // begin()没有prev, 特判. 同时如果空树, 也会走这里
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
    x->color_ = red;

    if (is_left) {
      __p->left_ = x;
      if (__p == &header_) {
        // 空树. 根据locate_, 空树时, is_left一定是true
        header_.right_ = x;
        header_.parent_ = x;
      } else if (__p == header_.left_)
        header_.left_ = x;
    } else {
      __p->right_ = x;
      if (__p == header_.right_) header_.right_ = x;
    }

    if (__p != &header_) rebalance_(x, __p);

    ++num_;
    return x;
  }

  void rebalance_(_rb_tree_node *__x, _rb_tree_node *__p) {
    do {
      // Case 1: parent is black
      if (__p->color_ == black) break;

      auto g = __p->parent_;
      assert(g);
      if (g == &header_) {
        // Case 4: parent is root and red
        __p->color_ = black;
        return;
      }

      bool is_left = g->left_ == __p;
      bool is_right = __p->right_ == __x;
      auto uncle = is_left ? g->right_ : g->left_;
      if (uncle && uncle->color_ == red) {
        // Case 2: parent and uncle are both red
        __p->color_ = black;
        uncle->color_ = black;
        g->color_ = red;
        __x = g;
      } else {
        if (is_left == is_right) {
          // Case 5: x, p, g形成了折角
          rotate_(__p, is_left);  // 旋转,将x转到g的外面
          __x = __p;
          __p = __x->parent_;
        }
        // Case 6: x, p, g一条线
        rotate_(g, !is_left);
        __p->color_ = black;
        g->color_ = red;
      }
    } while ((__p = __x->parent_) != &header_);  // Case 3: __p is root

    header_.parent_->color_ = black;
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

    if (g != &header_)
      __p == g->left_ ? (g->left_ = s) : (g->right_ = s);
    else
      header_.parent_ = s;
  }

  void delete_(_rb_tree_node *__x) {
    _rb_tree_node *n = nullptr, *y = __x;  // n表示继承x位置的节点
    _rb_tree_node *p = nullptr;  // 因为n可能为叶节点(nullptr),
                                 // 无法取得n->parent_, 所以需要保存它的父亲
    if (__x->left_ == nullptr)
      n = __x->right_;
    else if (__x->right_ == nullptr)
      n = __x->left_;
    else {
      n = _rb_tree_node::left_most(__x->right_);
      y = n->right_;
    }

    if (y != __x) {  // n is x's successor
      // x has two non-nil children, replace x with n
      assert(n->left_ == nullptr);
      n->left_ = __x->left_;
      __x->left_->parent_ = n;
      if (n != __x->right_) {
        // n和x之间有其他节点
        if (y) y->parent_ = n->parent_;
        n->parent_->left_ = y;
        n->right_ = __x->right_;
        __x->right_->parent_ = n;
        p = n->parent_;
      } else {
        p = n;
      }
      std::swap(n->color_, __x->color_);
    } else {
      p = __x->parent_;
      // x has at least one nil child
      if (n) {
      //  assert(n->color_ == red);
        n->parent_ = p;
      }
      // 最小和最大一定在这里, 因为它们最多一个儿子
      if (header_.left_ == __x) {
        header_.left_ =
            __x->right_ ? (n ? _rb_tree_node::left_most(n) : nullptr) : p;
      }
      if (header_.right_ == __x) {
        header_.right_ =
            __x->left_ ? (n ? _rb_tree_node::right_most(n) : nullptr) : p;
      }
    }
    // 设置parent
    if (n) n->parent_ = __x->parent_;
    if (__x == header_.parent_)
      header_.parent_ = n;
    else if (__x == __x->parent_->left_)
      __x->parent_->left_ = n;
    else
      __x->parent_->right_ = n;

    if (__x->color_ == black) delete_fixup_(y == __x ? n : y, p);
  }

  void delete_fixup_(_rb_tree_node *__x, _rb_tree_node *__p) {
    while (__x != header_.parent_ && (!__x || __x->color_ == black)) {
      if (__x == __p->left_) {
        auto s = __p->right_;  // sibling of __x
        if (s->color_ == red) {
          // Case 1
          s->color_ == black;
          __p->color_ = red;
          rotate_(__p, true);
          s = __p->right_;
        }

        if ((!s->left_ || s->left_->color_ == black) &&
            (!s->right_ || s->right_->color_ == black)) {
          // Case 2
          s->color_ = red;
          __x = __p;
          __p = __x->parent_;
        } else {
          if (!s->right_ || s->right_->color_ == black) {
            // Case 3
            s->left_->color_ = black;
            s->color_ = red;
            rotate_(s, false);
            s = __p->right_;
          }

          // Case 4
          s->color_ = __p->color_;
          __p->color_ = black;
          if (s->right_) s->right_->color_ = black;
          rotate_(__p, true);
          break;
        }
      } else {
        // same as above, just swap left and right
        auto s = __p->left_;  // sibling of __x
        if (s->color_ == red) {
          // Case 1
          s->color_ == black;
          __p->color_ = red;
          rotate_(__p, false);
          s = __p->left_;
        }

        if ((!s->left_ || s->left_->color_ == black) &&
            (!s->right_ || s->right_->color_ == black)) {
          // Case 2
          s->color_ = red;
          __x = __p;
          __p = __x->parent_;
        } else {
          if (!s->left_ || s->left_->color_ == black) {
            // Case 3
            s->right_->color_ = black;
            s->color_ = red;
            rotate_(s, true);
            s = __p->left_;
          }

          // Case 4
          s->color_ = __p->color_;
          __p->color_ = black;
          if (s->left_) s->left_->color_ = black;
          rotate_(__p, false);
          break;
        }
      }
    }
    if (__x) __x->color_ = black;
  }

 private:
  _rb_tree_node header_;  // 方便找最大, 最小. header->left_指向最小值,
                          // header_->right_指向最大值
  size_t num_;
};

}  // namespace wzj
