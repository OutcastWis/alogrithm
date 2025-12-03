cmake_minimum_required(VERSION 3.10)
project(my_algorithm_proj)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用测试
option(BUILD_TESTING "Build tests" ON)

# 包含子目录
add_subdirectory(boyer_moore)
add_subdirectory(geo)
add_subdirectory(segment_intersection)
add_subdirectory(self-balancing_binary_search_tree)
add_subdirectory(tree)
add_subdirectory(binary_index_tree)



# 或者如果你想要创建一个包含所有库的汇总目标
add_custom_target(project_all DEPENDS 
    boyer_moore 
    geo 
    segment_intersection 
    self-balancing_binary_search_tree
    tree
    binary_index_tree
    )