# 查找当前目录下的所有源文件
file(GLOB BOYER_MOORE_SOURCES "*.cpp" "*.cxx" "*.cc")

# 从源文件列表中移除测试文件
list(FILTER BOYER_MOORE_SOURCES EXCLUDE REGEX "/test.*\.cpp$")

if(BUILD_TESTING)
    # 创建测试可执行文件
    add_executable(test_boyer_moore test.cpp ${BOYER_MOORE_SOURCES})   
    # 包含路径
    target_include_directories(test_boyer_moore PRIVATE ../) 
    # 编译选项
    if(MSVC)
        target_compile_options(test_boyer_moore PRIVATE /W4 /utf-8)
    else()
        target_compile_options(test_boyer_moore PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_test(NAME boyer_moore_unit_test COMMAND test_boyer_moore)

    message(STATUS "Added boyer_moore_unit_test")
endif()

# 接口目标
add_library(boyer_moore INTERFACE)