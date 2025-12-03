# 查找当前目录下的所有源文件
file(GLOB BIT_SOURCES "*.cpp" "*.cxx" "*.cc")

# 从源文件列表中移除测试文件
list(FILTER BIT_SOURCES EXCLUDE REGEX "/test.*\.cpp$")

if(BUILD_TESTING)
    # 创建测试可执行文件
    add_executable(test_bit test.cpp ${BIT_SOURCES})   
    # 包含路径
    target_include_directories(test_bit PRIVATE ../) 
    # 编译选项
    if(MSVC)
        target_compile_options(test_bit PRIVATE /W4 /utf-8)
    else()
        target_compile_options(test_bit PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_test(NAME binary_index_tree_test COMMAND test_bit)

    message(STATUS "Added binary_index_tree_test")
endif()

# 接口目标
add_library(binary_index_tree INTERFACE)