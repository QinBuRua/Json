cmake_minimum_required(VERSION 4.0)
project(json_git)

set(CMAKE_CXX_STANDARD 17)

include_directories(code/include)

add_executable(json_git
        code/include/json.h
        code/src/src.cpp
        code/src/test_main.cpp)
