# 查找当前目录下的所有源文件
file(GLOB GEO_SOURCES "*.cpp" "*.cxx" "*.cc")
# 从源文件列表中移除测试文件
list(FILTER GEO_SOURCES EXCLUDE REGEX "/test.*\.cpp$")



if(BUILD_TESTING)
    # 创建测试可执行文件
    add_executable(test_geo test.cpp ${GEO_SOURCES})
    # 包含路径
    target_include_directories(test_geo PRIVATE ../)
    # 编译选项
    if(MSVC)
        target_compile_options(test_geo PRIVATE /W4 /utf-8)
    else()
        target_compile_options(test_geo PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_test(NAME geo_unit_test COMMAND test_geo)

    message(STATUS "Added geo_unit_test")
endif()

# 创建A的接口库 - 包含头文件但不编译源文件
add_library(geo_interface INTERFACE)

# 创建A的对象库 - 编译源文件但不生成库文件
add_library(geo_objects OBJECT ${GEO_SOURCES})
target_include_directories(geo_objects PRIVATE ../)

# 导出A的对象供其他目录使用
set(GEO_OBJECTS $<TARGET_OBJECTS:geo_objects> PARENT_SCOPE)
