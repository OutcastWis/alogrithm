#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <string>
#include <type_traits>

#include "base_tree.hpp"

namespace wzj {

struct _avl_tree_node : public _tree_node {
  size_t height_;  // 0 for nil, 1 for leaf
};

class avl_tree : public _tree_base {
 public:
  typedef _tree_base::iterator iterator;

 public:
  avl_tree() {
    auto x = new _avl_tree_node;
    x->parent_ = nullptr;
    x->left_ = x->right_ = x;
    x->height_ = 0;

    header_ = x;
  }

  ~avl_tree() {
    _tree_base::remove_node_<_avl_tree_node>(header_->parent_);
    delete header_;
  }

  std::pair<iterator, bool> insert(const std::string& key, int value) {
    auto r = _tree_base::locate_(key);
    if (r.second) {
      return std::pair<iterator, bool>(iterator(_insert(r.second, key, value)),
                                       true);
    }
    return std::pair<iterator, bool>(iterator(r.first), false);
  }

  iterator erase(iterator it) {
    auto r = it;
    ++r;
    delete_(static_cast<wzj::_avl_tree_node*>(it.node_));
    --num_;
    return r;
  }

  size_t erase(const std::string& key) {
    auto r = _tree_base::locate_(key);
    if (r.first) {
      delete_(static_cast<wzj::_avl_tree_node*>(r.first));
      --num_;
      return 1;
    }
    return 0;
  }

  void clear() {
    _tree_base::remove_node_<_avl_tree_node>(header_->parent_);
    num_ = 0;
    header_->parent_ = nullptr;
    header_->left_ = header_->right_ = header_;
  }

  bool check() {
    return check_(header_->parent_);
  }

 private:
  _tree_node* _insert(_tree_node* __p, const std::string& __key, int __value) {
    auto x = new _avl_tree_node;
    x->left_ = x->right_ = nullptr;
    x->kv_.first = __key, x->kv_.second = __value;
    x->height_ = 1;

    _tree_base::insert_(x, __p, __key);

    rebalance_(__p);

    return x;
  }
  // left's height - right's height
  int balance_(_tree_node* __x) const {
    if (!__x) return 0;
    return static_cast<int>(height_(__x->left_) - height_(__x->right_));
  }
  // 0 for nil, 1 for leaf
  size_t height_(_tree_node* __x) const {
    return __x ? static_cast<_avl_tree_node*>(__x)->height_ : 0;
  }
  // calculate height
  void calc_height_(_tree_node* __x) const {
    static_cast<_avl_tree_node*>(__x)->height_ =
        1 + std::max(height_(__x->left_), height_(__x->right_));
  }

  // __x为新插入, __p为__x父亲
  void rebalance_(_tree_node* __p) {
    while (__p != header_) {
      auto g = __p->parent_;
      // 旋转
      auto b = balance_(__p);
      if (b == 2) {
        auto x = __p->left_;
        assert(x);
        auto bb = balance_(x);
        if (bb >= 0) {
          // left left case
          _tree_base::rotate_(__p, false);
          calc_height_(__p);
          calc_height_(x);
        } else {
          // left right case
          _tree_base::rotate_(x, true);
          _tree_base::rotate_(__p, false);
          calc_height_(__p);
          calc_height_(x);
          calc_height_(__p->parent_);
        }
      } else if (b == -2) {
        auto x = __p->right_;
        assert(x);
        auto bb = balance_(x);
        if (bb <= 0) {
          // right right case
          _tree_base::rotate_(__p, true);
          calc_height_(__p);
          calc_height_(x);
        } else {
          // right left case
          _tree_base::rotate_(x, false);
          _tree_base::rotate_(__p, true);
          calc_height_(__p);
          calc_height_(x);
          calc_height_(__p->parent_);
        }
      } else {
        assert(b == 0 || b == -1 || b == 1);
        calc_height_(__p);
      }
      __p = g;
    }
  }

  // remove node from avl tree
  void delete_(_avl_tree_node* __x) {
    _tree_base::delete_();
    _tree_node *n = nullptr, *y = __x;  // n表示继承x位置的节点
    _tree_node* p = nullptr;            // 因为n可能为叶节点(nullptr),
                              // 无法取得n->parent_, 所以需要保存它的父亲. 
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
    } else {
      p = __x->parent_;
      // x has at least one nil child
      if (n) {
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

  // 此时p表示高度发生变化的点
    rebalance_(p);
  }

  bool check_(_tree_node* __x) {
    if (!__x) return true;
    auto b = balance_(__x);
    if (b > 1 || b < -1) return false;

    return check_(__x->left_) && check_(__x->right_);
  } 
};
}  // namespace wzj
