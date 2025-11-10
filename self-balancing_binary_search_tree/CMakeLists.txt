file(GLOB SELF_BLANCEING_BINARY_SEARCH_TREE_SOURCE "*.cpp" "*.cxx" "*.cc")

# 从源文件列表中移除测试文件
list(FILTER SELF_BLANCEING_BINARY_SEARCH_TREE_SOURCE EXCLUDE REGEX "/test.*\.cpp$")


if(BUILD_TESTING)
    # 创建测试可执行文件
    add_executable(test_sbbst test.cpp ${SELF_BLANCEING_BINARY_SEARCH_TREE_SOURCE})
    # 包含路径
    target_include_directories(test_sbbst PRIVATE ../)
    # 编译选项
    if(MSVC)
        target_compile_options(test_sbbst PRIVATE /W4 /utf-8)
    else()
        target_compile_options(test_sbbst PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_test(NAME self_balancing_binary_search_tree_unit_test COMMAND test_sbbst)

    message(STATUS "Added self_balancing_binary_search_tree_unit_test")
endif()

# 接口目标
add_library(sbbst INTERFACE)
