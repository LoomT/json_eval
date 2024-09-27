cmake_minimum_required(VERSION 3.29)
project(json_eval)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/b514bdc898e2951020cbdca1304b75f5950d1f59.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(json_eval main.cpp
        parse.cpp
        parse.h
        value.h
        value.cpp)

target_link_libraries(json_eval gtest gtest_main)

add_executable(tests
        parse.cpp
        parse.h
        value.h
        value.cpp
        tests/parseTest.cpp)

target_compile_definitions(tests PUBLIC TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/resources/parseJSON")

target_link_libraries(tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)