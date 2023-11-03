#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>

#include "base_tree.hpp"

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

struct _rb_tree_node : public _tree_node {
  _rb_tree_color color_ = red;  // default red
};

class rb_tree : public _tree_base {
 public:
  typedef _tree_base::iterator iterator;

 public:
  rb_tree() {
    _rb_tree_node *x = new _rb_tree_node;
    x->color_ = red;
    x->left_ = x->right_ = x;
    x->parent_ = nullptr;
    header_ = x;
  }

  ~rb_tree() {
    _tree_base::remove_node_<_rb_tree_node>(header_->parent_);
    delete header_;
  }

  std::pair<iterator, bool> insert(const std::string &key, int value) {
    auto r = _tree_base::locate_(key);
    if (r.second) {
      return std::pair<iterator, bool>(
          insert_(static_cast<_rb_tree_node *>(r.second), key, value), true);
    }

    return std::pair<iterator, bool>(iterator(r.first), false);
  }

  size_t erase(const std::string &key) {
    auto r = _tree_base::locate_(key);
    if (r.first) {
      delete_(static_cast<_rb_tree_node *>(r.first));
      --num_;
      return 1;
    }
    return 0;
  }

  iterator erase(iterator it) {
    auto r = it;
    ++r;
    delete_(static_cast<_rb_tree_node *>(it.node_));
    --num_;
    return r;
  }

  void clear() {
    _tree_base::remove_node_<_rb_tree_node>(header_->parent_);
    num_ = 0;
    header_->parent_ = nullptr;
    header_->left_ = header_->right_ = header_;
  }

  // 检查是否满足红黑树特性
  bool check() {
    auto x = header_->parent_;
    if (!x) return true;
    auto sl = count_black_(x->left_);
    if (sl == -1) return false;
    auto sr = count_black_(x->right_);
    return sl == sr;
  }

 private:
  _rb_tree_node *insert_(_rb_tree_node *__p, const std::string &__key,
                         int __value) {
    _rb_tree_node *x = new _rb_tree_node;
    x->left_ = x->right_ = nullptr;
    x->kv_.first = __key;
    x->kv_.second = __value;
    x->color_ = red;

    _tree_base::insert_(x, __p, __key);

    if (__p != header_) rebalance_(x, __p);

    return x;
  }

  void rebalance_(_rb_tree_node *__x, _rb_tree_node *__p) {
    do {
      // Case 1: parent is black
      if (__p->color_ == black) break;

      auto g = static_cast<_rb_tree_node *>(__p->parent_);
      assert(g);
      if (g == header_) {
        // Case 4: parent is root and red
        __p->color_ = black;
        return;
      }

      bool is_left = g->left_ == __p;
      bool is_right = __p->right_ == __x;
      auto uncle = static_cast<_rb_tree_node *>(is_left ? g->right_ : g->left_);
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
          __p = static_cast<_rb_tree_node *>(__x->parent_);
        }
        // Case 6: x, p, g一条线
        rotate_(g, !is_left);
        __p->color_ = black;
        g->color_ = red;
      }
    } while ((__p = static_cast<_rb_tree_node *>(__x->parent_)) !=
             header_);  // Case 3: __p is root

    static_cast<_rb_tree_node *>(header_->parent_)->color_ = black;
  }

  void delete_(_rb_tree_node *__x) {
    _tree_base::delete_();

    _tree_node *n = nullptr, *y = __x;  // n表示继承x位置的节点
    _tree_node *p = nullptr;            // 因为n可能为叶节点(nullptr),
                              // 无法取得n->parent_, 所以需要保存它的父亲
    if (__x->left_ == nullptr)
      n = __x->right_;
    else if (__x->right_ == nullptr)
      n = __x->left_;
    else {
      n = _tree_node::left_most(__x->right_);
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
      std::swap(static_cast<_rb_tree_node *>(n)->color_, __x->color_);
    } else {
      p = __x->parent_;
      // x has at least one nil child
      if (n) {
        assert(static_cast<_rb_tree_node *>(n)->color_ == red);
        n->parent_ = p;
      }
      // 最小和最大一定在这里, 因为它们最多一个儿子
      if (header_->left_ == __x) {
        header_->left_ = __x->right_
                             ? (assert(n == __x->right_),
                                n ? _tree_node::left_most(n) : nullptr)
                             : p;
      }
      if (header_->right_ == __x) {
        header_->right_ = __x->left_
                              ? (assert(n == __x->left_),
                                 n ? _tree_node::right_most(n) : nullptr)
                              : p;
      }
    }
    // 设置parent
    if (n) n->parent_ = __x->parent_;
    if (__x == header_->parent_)
      header_->parent_ = n;
    else if (__x == __x->parent_->left_)
      __x->parent_->left_ = n;
    else
      __x->parent_->right_ = n;

    if (__x->color_ == black)
      delete_fixup_(static_cast<_rb_tree_node *>(y == __x ? n : y),
                    static_cast<_rb_tree_node *>(p));

    delete __x;
  }

  void delete_fixup_(_rb_tree_node *__x, _rb_tree_node *__p) {
    while (__x != header_->parent_ && (!__x || __x->color_ == black)) {
      if (__x == __p->left_) {
        auto s = static_cast<_rb_tree_node *>(__p->right_);  // sibling of __x
        if (s->color_ == red) {
          // Case 1
          s->color_ = black;
          __p->color_ = red;
          rotate_(__p, true);
          s = static_cast<_rb_tree_node *>(__p->right_);
        }

        if ((!s->left_ ||
             static_cast<_rb_tree_node *>(s->left_)->color_ == black) &&
            (!s->right_ ||
             static_cast<_rb_tree_node *>(s->right_)->color_ == black)) {
          // Case 2
          s->color_ = red;
          __x = __p;
          __p = static_cast<_rb_tree_node *>(__x->parent_);
        } else {
          if (!s->right_ ||
              static_cast<_rb_tree_node *>(s->right_)->color_ == black) {
            // Case 3
            static_cast<_rb_tree_node *>(s->left_)->color_ = black;
            s->color_ = red;
            rotate_(s, false);
            s = static_cast<_rb_tree_node *>(__p->right_);
          }

          // Case 4
          s->color_ = __p->color_;
          __p->color_ = black;
          if (s->right_)
            static_cast<_rb_tree_node *>(s->right_)->color_ = black;
          rotate_(__p, true);
          break;
        }
      } else {
        // same as above, just swap left and right
        auto s = static_cast<_rb_tree_node *>(__p->left_);  // sibling of __x
        if (s->color_ == red) {
          // Case 1
          s->color_ = black;
          __p->color_ = red;
          rotate_(__p, false);
          s = static_cast<_rb_tree_node *>(__p->left_);
        }

        if ((!s->left_ ||
             static_cast<_rb_tree_node *>(s->left_)->color_ == black) &&
            (!s->right_ ||
             static_cast<_rb_tree_node *>(s->right_)->color_ == black)) {
          // Case 2
          s->color_ = red;
          __x = __p;
          __p = static_cast<_rb_tree_node *>(__x->parent_);
        } else {
          if (!s->left_ ||
              static_cast<_rb_tree_node *>(s->left_)->color_ == black) {
            // Case 3
            static_cast<_rb_tree_node *>(s->right_)->color_ = black;
            s->color_ = red;
            rotate_(s, true);
            s = static_cast<_rb_tree_node *>(__p->left_);
          }

          // Case 4
          s->color_ = __p->color_;
          __p->color_ = black;
          if (s->left_) static_cast<_rb_tree_node *>(s->left_)->color_ = black;
          rotate_(__p, false);
          break;
        }
      }
    }
    if (__x) __x->color_ = black;
  }

  int count_black_(_tree_node *__x) {
    // all nil node is black
    if (__x == nullptr) return 1;

    // red node can't have red child
    if (static_cast<_rb_tree_node *>(__x)->color_ == red &&
        ((__x->left_ &&
          static_cast<_rb_tree_node *>(__x->left_)->color_ == red) ||
         (__x->right_ &&
          static_cast<_rb_tree_node *>(__x->right_)->color_ == red)))
      return -1;

    auto sl = count_black_(__x->left_);
    if (sl == -1) return -1;
    auto sr = count_black_(__x->right_);
    if (sl != sr) return -1;  // must have same number of block nodes

    return sl + (static_cast<_rb_tree_node *>(__x)->color_ == black);
  }
};

}  // namespace wzj
