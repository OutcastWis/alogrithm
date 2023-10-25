#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "rb_tree.hpp"

using namespace std::chrono;
typedef std::chrono::milliseconds MS;

int epoch=1e6;

void print_info(const std::string& group, const std::vector<MS>& data) {
  int64_t tot = 0;
  for (auto i : data) tot += i.count();
  std::cout << group << "=>\n";
  std::cout << "\ttotal(ms):" << tot << std::endl;
  std::cout << "\taverage(ms) :" << std::fixed << std::setprecision(2)
            << tot * 1.0 / data.size() << std::endl;
}

void t_insert(std::map<std::string, int>* std_map, wzj::rb_tree* my_map) {
  std::vector<MS> std_used;
  std::vector<MS> my_used;


  {  // std
    auto st = high_resolution_clock::now();

    for (auto i = 0; i < epoch; ++i) std_map->insert({std::to_string(i), i});

    auto end = high_resolution_clock::now();
    std_used.push_back(std::chrono::duration_cast<MS>(end - st));
  }

  {  // my
    auto st = high_resolution_clock::now();

    for (auto i = 0; i < epoch; ++i) {
      my_map->insert(std::to_string(i), i);
      assert(my_map->check());
    }

    auto end = high_resolution_clock::now();
    my_used.push_back(std::chrono::duration_cast<MS>(end - st));
  }

  std::cout << "INSERT:\n";
  print_info("std", std_used);
  print_info("my", my_used);
}

void t_iterator(std::map<std::string, int>& std_map, wzj::rb_tree& my_map) {
  std::vector<MS> std_used;
  std::vector<MS> my_used;

  {  // 验证正确性
    auto it = std_map.begin();
    auto it2 = my_map.begin();
    for (; it != std_map.end(); ++it, ++it2) {
      if (it->first != it2->first || it->second != it2->second) {
        std::cout << "Assert failed: {" << it->first << "," << it->second
                  << "} != {" << it2->first << "," << it2->second << "}\n";
        return;
      }
    }
  }

  {  // std
    auto st = high_resolution_clock::now();

    for (auto it = std_map.begin(); it != std_map.end(); ++it)
      ;

    auto end = high_resolution_clock::now();
    std_used.push_back(std::chrono::duration_cast<MS>(end - st));
  }

  {  // my
    auto st = high_resolution_clock::now();

    for (auto it = my_map.begin(); it != my_map.end(); ++it)
      ;
    auto end = high_resolution_clock::now();
    my_used.push_back(std::chrono::duration_cast<MS>(end - st));
  }

  std::cout << "ITERATOR:\n";
  print_info("std", std_used);
  print_info("my", my_used);
}

void t_erase(std::map<std::string, int>* std_map, wzj::rb_tree* my_map) {
  std::vector<MS> std_used;
  std::vector<MS> my_used;

  std::map<std::string, int> comp_map = *std_map;


  {  // std
    auto cit = comp_map.begin();
    auto st = high_resolution_clock::now();
    auto it = std_map->begin();
    for (int i = 0; i < epoch; ++i, ++cit) {
      assert(cit->second == it->second);
      it = std_map->erase(it);
    }

    auto end = high_resolution_clock::now();
    std_used.push_back(std::chrono::duration_cast<MS>(end - st));
  }

  {  // my
    auto cit = comp_map.begin();
    auto st = high_resolution_clock::now();
    auto it = my_map->begin();
    for (int i = 0; i < epoch; ++i, ++cit) {
      assert(cit->second == it->second);
      it = my_map->erase(it);
      assert(my_map->check());
    }

    auto end = high_resolution_clock::now();
    my_used.push_back(std::chrono::duration_cast<MS>(end - st));
  }

  if (std_map->size() != my_map->size()) {
    std::cout << "Assert size failed: std_map.size = " << std_map->size()
              << ", my_map.size = " << my_map->size() << std::endl;
    return;
  }

  std::cout << "ERASE:\n";
  print_info("std", std_used);
  print_info("my", my_used);
}

int main() {

  std::map<std::string, int> std_map;
  wzj::rb_tree my_map;

  t_insert(&std_map, &my_map);
  t_iterator(std_map, my_map);
  t_erase(&std_map, &my_map);

  return 0;
}