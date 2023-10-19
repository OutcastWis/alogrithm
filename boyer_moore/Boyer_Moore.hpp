#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace wzj {

template <typename _It>
class boyer_moore {
 public:
  using _diff_type = typename std::iterator_traits<_It>::difference_type;

 public:
  boyer_moore(_It first, _It last) : st_(first), en_(last), good_char_(last - first) {
    make_delta1(first, last - first);
    make_delta2(first, last - first);
  }
  // 返回第一次匹配的区间. 如果不匹配, 则返回(last,last)
  template <typename _TargetIt>
  std::pair<_TargetIt, _TargetIt> operator()(_TargetIt first,
                                             _TargetIt last) const {
    auto pat_len = en_ - st_;
    if (pat_len == 0) return std::make_pair(first, first);

    _diff_type i = pat_len - 1;
    auto stringlen = last - first;
    while (i < stringlen) {
      _diff_type j = pat_len - 1;
      while (j >= 0 && first[i] == st_[j]) {
        --i;
        --j;
      }
      if (j < 0) {
        // match
        const auto match = first + i + 1;
        return std::make_pair(match, match + pat_len);
      }
      // not match, jump
      i += std::max(bad_char_[first[i]], good_char_[j]);
    }
    return std::make_pair(last, last);
  }

 private:
  // 判断字串[pos, end)是否是前缀
  bool is_prefix(_It pat, _diff_type pat_len, _diff_type pos) {
    _diff_type suffixlen = pat_len - pos;
    for (_diff_type i = 0; i < suffixlen; ++i)
      if (pat[i] != pat[pos + i]) return false;
    return true;
  }
  // 返回[0, pos]和[0, pat_len)的共同后缀长度+1
  _diff_type suffix_length(_It pat, _diff_type pat_len, _diff_type pos) {
    _diff_type i = 0;
    for (; pat[pos - i] == pat[pat_len - 1 - i] && i < pos; ++i)
      ;
    return i;
  }

  void make_delta1(_It pat, _diff_type pat_len) {
    bad_char_.fill(pat_len);
    if (pat_len > 0) {
      for (_diff_type i = 0; i < pat_len; ++i) {
        auto ch = pat[i];
        auto uch = static_cast<typename std::make_unsigned<decltype(ch)>::type>(ch);
        bad_char_[uch] = pat_len - 1 - i;
      }
    }
  }

  void make_delta2(_It pat, _diff_type pat_len) {
    if (pat_len == 0) return;
    // loop 1, part match. 例如: lsp000alsp, 后缀alsp没匹配项, 但有部分匹配,
    // ([a]lsp)000alsp这样的形式
    _diff_type last_prefix_index = 1;
    for (_diff_type i = pat_len - 1; i >= 0; --i) {
      if (is_prefix(pat, pat_len, i + 1)) last_prefix_index = i + 1;
      good_char_[i] = last_prefix_index + (pat_len - 1 - i);
    }
    // loop 2. full match. 例如: 00alspxalsp中, 后缀alsp匹配00(alsp)xalsp
    for (_diff_type i = 0; i < pat_len - 1; ++i) {
      auto slen = suffix_length(pat, pat_len, i);
      auto pos = pat_len - 1 - slen;
      if (pat[i - slen] != pat[pos]) good_char_[pos] = pat_len - 1 - i + slen;
    }
  }

 private:
  // BAD-CHARACTER RULE
  // delta1 table: delta1[c] contains the distance between the last
  // character of pattern and the rightmost occurrence of c in pattern.
  std::array<_diff_type, 256> bad_char_;

  // GOOD-SUFFIX RULE.
  // delta2 table: given a mismatch at pat[pos], we want to align
  // with the next possible full match could be based on what we
  // know about pat[pos+1] to pat[patlen-1].
  std::vector<_diff_type> good_char_;

  _It st_;  // start of pattern
  _It en_;  // end of pattern
};
}  // namespace wzj