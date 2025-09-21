cmake_minimum_required(VERSION 4.0)
project(json_git)

set(CMAKE_CXX_STANDARD 17)

include_directories(code)

add_executable(json_git
        code/json.h
        code/src.cpp
        code/test_main.cpp)
