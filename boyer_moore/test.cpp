#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "Boyer_Moore.hpp"

using namespace std::chrono;
typedef std::chrono::milliseconds MS;

void print_info(const std::string& group, const std::vector<MS>& data) {
  int64_t tot = 0;
  for (auto i : data) tot += i.count();
  std::cout << group << "=>\n";
  std::cout << "\ttotal(ms):" << tot << std::endl;
  std::cout << "\taverage(ms) :" << std::setprecision(2)
            << tot * 1.0 / data.size() << std::endl;
}

int main() {
  std::vector<MS> std_used;
  std::vector<MS> my_used;

  for (int epoch = 0; epoch < 20; ++epoch) {
    std::string target;
    for (int i = 0; i < 1000000; ++i) target += std::to_string(rand() % 10);
    std::string pattern;
    for (int i = 0; i < 10; ++i) pattern += std::to_string(rand() % 10);

    std::string::const_iterator it1, it2;
    {
      auto st = high_resolution_clock::now();
        it1 = std::search(
          target.begin(), target.end(),
          std::boyer_moore_searcher(pattern.begin(), pattern.end()));
      auto end = high_resolution_clock::now();
      std_used.push_back(std::chrono::duration_cast<MS>(end - st));
    }

    {
      auto st = high_resolution_clock::now();
      it2 = wzj::boyer_moore<decltype(pattern.begin())>(
                pattern.begin(), pattern.end())(target.begin(), target.end())
                .first;
      auto end = high_resolution_clock::now();
      my_used.push_back(std::chrono::duration_cast<MS>(end - st));
    }

    assert(it1 == it2);
  }

  print_info("std", std_used);
  print_info("my", my_used);
}