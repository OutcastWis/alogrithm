# rb_tree
## REQUIRE
You need c++11 to use it. Just like std::map but more simple. It only doesn't have const iterator and is not a template. The type of key is std::string, and the type of value is int.

## USE
```c++
wzj::rb_tree my_map; 
my_map.insert("key", 1); // insert value
for (auto it = my_map.begin(); it != my_map.end();++it) {...} // traversal
// erase
my_map.erase("key")
my_map.erase(my_map.begin())
```

## BUILD
```
cd self-balancing_binary_search_tree
g++ main.cpp -std=c++11 -DNDEBUG
```
> main.cpp compares wzj::rb_tree with std::map\<std::string, int\>
