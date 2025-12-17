# 查找当前目录下的所有源文件
file(GLOB BIPARTITE_SOURCES "*.cpp" "*.cxx" "*.cc")
# 从源文件列表中移除测试文件
list(FILTER BIPARTITE_SOURCES EXCLUDE REGEX "/test.*\.cpp$")



if(BUILD_TESTING)
    # 创建测试可执行文件
    add_executable(test_bipartite test.cpp ${BIPARTITE_SOURCES})
    # 包含路径
    target_include_directories(test_bipartite PRIVATE ../)
    # 编译选项
    if(MSVC)
        target_compile_options(test_bipartite PRIVATE /W4 /utf-8)
    else()
        target_compile_options(test_bipartite PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_test(NAME bipartite_unit_test COMMAND test_bipartite)

    message(STATUS "Added bipartite_unit_test")
endif()

# 创建接口库 - 包含头文件但不编译源文件
add_library(bipartite INTERFACE)


