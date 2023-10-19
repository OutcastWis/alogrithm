## REQUIRE
You need c++11 to use it

## USE
```c++
// auto bm = wzj::boyer_moore(pattern.begin(), pattern.end()) // for C++17 or higher
auto bm = wzj::boyer_moore<decltype(pattern.begin())>(pattern.begin(), pattern.end());
auto it = bm(target.begin(), target.end());
if (it.first != target.end())
    std::cout << std::string(it.first, it.second) << std::endl;
else
    std::cout << "not found" << std::endl;
```

## BUILD
> you need c++17 to build main.cpp. Because it uses std::boyer_moore_searcher
```
cd boyer_moore
g++ main.cpp -std=c++17
```
> main.cpp compares wzj::boyer_moore with std::boyer_moore_searcher
