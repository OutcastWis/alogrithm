#pragma once

#include <sstream>
#include <string>
#include <utility>

namespace wzj {

typedef std::pair<std::string, int> _tree_node_value_type;

struct _tree_node {
  _tree_node_value_type kv_;
  _tree_node *parent_;
  _tree_node *left_;
  _tree_node *right_;

  static _tree_node *left_most(_tree_node *x) {
    while (x->left_) x = x->left_;
    return x;
  }

  static _tree_node *right_most(_tree_node *x) {
    while (x->right_) x = x->right_;
    return x;
  }

  static _tree_node *next(_tree_node *x) {
    if (x->right_) return left_most(x->right_);
    while (x == x->parent_->right_) {
      x = x->parent_;
    }

    if (x->right_ == x->parent_)
      return x;  // 此时x是header_, 且只有一个节点, 则该节点是最大的,
                 // next应该返回end(), 即header_

    return x->parent_;  // 普通情况
  }

  static _tree_node *prev(_tree_node *x) {
    if (x == x->parent_->parent_ && x->left_ &&
        (x->left_->parent_ != x || x->left_ == x->parent_))
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

struct _tree_iterator {
  typedef _tree_node_value_type *pointer;
  typedef _tree_node_value_type &reference;

  typedef _tree_iterator _Self;

  _tree_iterator() = default;
  explicit _tree_iterator(_tree_node *x) : node_(x) {}

  pointer operator->() const { return &node_->kv_; }
  reference operator*() const { return node_->kv_; }

  _Self operator++() {
    node_ = _tree_node::next(node_);
    return *this;
  }
  _Self operator++(int) {
    _Self tmp = *this;
    node_ = _tree_node::next(node_);
    return tmp;
  }

  _Self operator--() {
    node_ = _tree_node::prev(node_);
    return *this;
  }
  _Self operator--(int) {
    _Self tmp = *this;
    node_ = _tree_node::prev(node_);
    return tmp;
  }

  friend bool operator==(const _tree_iterator &x, const _tree_iterator &y) {
    return x.node_ == y.node_;
  }
  friend bool operator!=(const _tree_iterator &x, const _tree_iterator &y) {
    return x.node_ != y.node_;
  }

  _tree_node *node_;
};

class _tree_base {
 public:
  typedef _tree_iterator iterator;

 public:
  _tree_base() = default;

  size_t size() const { return num_; }
  iterator begin() { return iterator(header_->left_); }
  iterator end() { return iterator(header_); }

  std::string statistics() const {
    std::stringstream ss;
    ss << "########## tree statistics ##########\nBase:"
       << "\n\tsize: " << num_;
    if (num_ == 0)
      ss << "\n\tmin: {nil}\n\tmax: {nil}" << std::endl;
    else
      ss << "\n\tmin: {" << header_->left_->kv_.first << ","
         << header_->left_->kv_.second << "}\n\tmax: {"
         << header_->right_->kv_.first << "," << header_->right_->kv_.second
         << std::endl;
    ss << "Insert:"
       << "\n\ttimes: " << i_times_ << "\n\trotate: " << ri_times_
       << "\n\taverage rotate:" << std::fixed << std::setprecision(2)
       << (i_times_ == 0 ? 0 : (ri_times_ * 1.0 / i_times_)) << std::endl;
    ss << "Erase: \n"
       << "\ttimes: " << e_times_ << "\n\trotate: " << re_times_
       << "\n\taverage rotate:" << std::fixed << std::setprecision(2)
       << (e_times_ == 0 ? 0 : (re_times_ * 1.0 / e_times_)) << std::endl;
    return ss.str();
  }

 protected:
  void insert_(_tree_node *__x, _tree_node *__p, const std::string &__key) {
#ifndef NDEBUG
    under_insert_ = true;
    ++i_times_;
#endif
    bool is_left = __p == header_ || __key < __p->kv_.first;
    __x->parent_ = __p;

    if (is_left) {
      __p->left_ = __x;
      if (__p == header_) {
        // 空树. 根据locate_, 空树时, is_left一定是true
        header_->right_ = __x;
        header_->parent_ = __x;
      } else if (__p == header_->left_)
        header_->left_ = __x;
    } else {
      __p->right_ = __x;
      if (__p == header_->right_) header_->right_ = __x;
    }

    ++num_;
  }

  // __left == true表示左旋, false表示右旋
  void rotate_(_tree_node *__p, bool __left) {
    auto g = __p->parent_;
    _tree_node *s = nullptr, *c = nullptr;
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

    if (g != header_)
      __p == g->left_ ? (g->left_ = s) : (g->right_ = s);
    else
      header_->parent_ = s;

#ifndef NDEBUG
    under_insert_ ? (++ri_times_) : (++re_times_);
#endif
  }

  // key存在, 则返回{key's node, nil}, 否则返回{nil, key's parent}.
  // 空树时返回{nil, header_}
  std::pair<_tree_node *, _tree_node *> locate_(const std::string &__key) {
    _tree_node *x = header_->parent_, *y = header_;
    bool comp = true;
    while (x != nullptr) {
      y = x;
      comp = __key < x->kv_.first;
      x = comp ? x->left_ : x->right_;
    }

    auto may_equal = y;
    if (comp) {
      // 可能key < y.key, 但key == y.prev.key
      if (may_equal == header_->left_)
        // begin()没有prev, 特判. 同时如果空树, 也会走这里
        return std::pair<_tree_node *, _tree_node *>(nullptr, y);
      may_equal = _tree_node::prev(may_equal);
    }

    if (may_equal->kv_.first < __key)
      return std::pair<_tree_node *, _tree_node *>(nullptr, y);
    // 相等
    return std::pair<_tree_node *, _tree_node *>(may_equal, nullptr);
  }

  //
  void delete_() {
#ifndef NDEBUG
    under_insert_ = false;
    ++e_times_;
#endif
  }


template <typename _Node>
void remove_node_(_tree_node* __x)  {
   while (__x) {
      auto y = __x->right_;
      remove_node_<_Node>(__x->left_);
      delete static_cast<_Node*>(__x);
      __x = y;
    }
}

  _tree_node *header_ = nullptr;

  size_t num_ = 0;

#ifndef NDEBUG
  // 统计信息
  bool under_insert_ = false;  // true for insert operation, false for erase
  size_t ri_times_ = 0;  // the total times of rotate when you called insert
  size_t i_times_ = 0;   // the times you called insert
  size_t re_times_ = 0;  // the total times of rotate when you called erase
  size_t e_times_ = 0;   // the times you called erase
#endif
};

}  // namespace wzj