#include <tree/scapegoat_tree.h>
#include <tree/splay.h>

#include <iostream>
#include <cassert>
#include <map>
#include <chrono>

using namespace std::chrono;
typedef std::chrono::milliseconds MS;

void test_scapegoat_tree() {
    wzj::scapegoat_tree tree(0.7);

    tree.insert("apple", 1);
    tree.insert("banana", 2);
    tree.insert("cherry", 3);
    tree.insert("date", 4);
    tree.insert("elderberry", 5);

    assert(tree.has("apple") == true);
    assert(tree.has("banana") == true);
    assert(tree.has("cherry") == true);
    assert(tree.has("date") == true);
    assert(tree.has("elderberry") == true);
    assert(tree.has("fig") == false);

    tree.erase("cherry");
    assert(tree.has("cherry") == false);

    tree.erase("apple");
    assert(tree.has("apple") == false);

    tree.insert("cherry", 33);
    assert(tree.has("cherry") == true);
}

void test_time_compare_with_std_map() {
    int n = 1e7;
    wzj::scapegoat_tree tree;
    std::map<std::string, int> std_map;
    // insert test
    auto start = high_resolution_clock::now();
    for (int i = 0; i < n; ++i) {
        tree.insert("key" + std::to_string(i), i);
    }
    auto end = high_resolution_clock::now();
    std::cout << "Scapegoat Tree Insert Time: " << std::chrono::duration_cast<MS>(end - start).count() << " ms\n";

    start = high_resolution_clock::now();
    for (int i = 0; i < n; ++i) {
        std_map.insert({"key" + std::to_string(i), i});
    }
    end = high_resolution_clock::now();
    std::cout << "std::map Insert Time: " << std::chrono::duration_cast<MS>(end - start).count() << " ms\n";
    // search test
    start = high_resolution_clock::now();
    for (int i = 0; i < n; ++i) {
        tree.has("key" + std::to_string(i));
    }
    end = high_resolution_clock::now();
    std::cout << "Scapegoat Tree Search Time: " << std::chrono::duration_cast<MS>(end - start).count() << " ms\n";
    start = high_resolution_clock::now();
    for (int i = 0; i < n; ++i) {
        std_map.find("key" + std::to_string(i));
    }
    end = high_resolution_clock::now();
    std::cout << "std::map Search Time: " << std::chrono::duration_cast<MS>(end - start).count() << " ms\n";
}

void test_splay() {
    wzj::splay tree;

    tree.insert("apple", 1);
    tree.insert("banana", 2);
    tree.insert("cherry", 3);
    tree.insert("date", 4);
    tree.insert("elderberry", 5);

    assert(tree.has("apple") == true);
    assert(tree.has("banana") == true);
    assert(tree.has("cherry") == true);
    assert(tree.has("date") == true);
    assert(tree.has("elderberry") == true);
    assert(tree.has("fig") == false);

    assert(tree.kth(1).second == 1);
    assert(tree.kth(2).second == 2);
    assert(tree.kth(3).second == 3);
    assert(tree.kth(4).second == 4);
    assert(tree.kth(5).second == 5);

    tree.erase("cherry");
    assert(tree.has("cherry") == false);
    assert(tree.kth(3).first == "date");

    tree.erase("apple");
    assert(tree.has("apple") == false);
    assert(tree.kth(1).first == "banana");
}

int main() {
    test_scapegoat_tree();
    test_time_compare_with_std_map();

    test_splay();
    return 0;
}
