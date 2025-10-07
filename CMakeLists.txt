# 项目
cmake_minimum_required(VERSION 3.14)
project(json_git VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# 配置库
add_library(json_lib code/src/src.cpp)
target_include_directories(json_lib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/code/include)
target_compile_features(json_lib PUBLIC cxx_std_17)

# 命名空间别名，方便使用
add_library(json_git::json_lib ALIAS json_lib)

# 测试文件
add_executable(json_test code/src/test_main.cpp)
target_link_libraries(json_test PRIVATE json_lib)