cmake_minimum_required(VERSION 3.29)
project(json_eval)

set(CMAKE_CXX_STANDARD 20)

add_executable(json_eval main.cpp
        parse.cpp
        parse.h)
