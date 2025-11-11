# 查找当前目录下的所有源文件
file(GLOB TREE_SOURCES "*.cpp" "*.cxx" "*.cc")
# 从源文件列表中移除测试文件
list(FILTER TREE_SOURCES EXCLUDE REGEX "/test.*\.cpp$")



if(BUILD_TESTING)
    # 创建测试可执行文件
    add_executable(test_tree test.cpp ${TREE_SOURCES})
    # 包含路径
    target_include_directories(test_tree PRIVATE ../)
    # 编译选项
    if(MSVC)
        target_compile_options(test_tree PRIVATE /W4 /utf-8)
    else()
        target_compile_options(test_tree PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_test(NAME tree_unit_test COMMAND test_tree)

    message(STATUS "Added tree_unit_test")
endif()

# 创建接口库 - 包含头文件但不编译源文件
add_library(tree_interface INTERFACE)

# 创建A的对象库 - 编译源文件但不生成库文件
add_library(tree_objects OBJECT ${TREE_SOURCES})
target_include_directories(tree_objects PRIVATE ../)

# 导出A的对象供其他目录使用
set(TREE_OBJECTS $<TARGET_OBJECTS:tree_objects> PARENT_SCOPE)
