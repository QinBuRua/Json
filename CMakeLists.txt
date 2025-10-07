cmake_minimum_required(VERSION 3.14)
project(json_git VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

include_directories(code/include)

add_executable(json_git
        code/include/json.h
        code/src/src.cpp
        code/src/test_main.cpp)

add_library(json_lib code/src/src.cpp)
target_include_directories(json_lib PUBLIC code/include)
target_compile_features(json_lib INTERFACE cxx_std_17)