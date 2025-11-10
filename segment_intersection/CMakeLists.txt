file(GLOB SEGMENT_INTERSECTION_SOURCES "*.cpp" "*.cxx" "*.cc")

# 从源文件列表中移除测试文件
list(FILTER SEGMENT_INTERSECTION_SOURCES EXCLUDE REGEX "/test.*\.cpp$")


if(BUILD_TESTING)
    
    # 创建测试可执行文件
    add_executable(test_segment_intersection test_segment_intersection.cpp ${SEGMENT_INTERSECTION_SOURCES})
    # 包含路径
    target_include_directories(test_segment_intersection PRIVATE ../)
    # 编译选项
    if(MSVC)
        target_compile_options(test_segment_intersection PRIVATE /W4 /utf-8)
    else()
        target_compile_options(test_segment_intersection PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    # 
    if(TARGET geo_interface)
        # 链接geo的接口库（用于包含路径）
        target_link_libraries(test_segment_intersection geo_interface)
        # 添加geo的对象文件
        target_sources(test_segment_intersection PRIVATE 
            ${GEO_OBJECTS}  # 从父作用域获取A的对象文件
        )
    endif()

    add_test(NAME segment_intersection_unit_test COMMAND test_segment_intersection)

    message(STATUS "Added segment_intersection_unit_test")
endif()


