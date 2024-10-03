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

add_executable(json_eval src/main.cpp
        src/JSON.cpp
        src/JSON.h
        src/value.h
        src/value.cpp
        src/expression.h
        src/expression.cpp
        src/execute.cpp
        src/execute.h)

add_executable(tests
        src/JSON.cpp
        src/JSON.h
        src/value.h
        src/value.cpp
        tests/parseJSONTest.cpp
        src/expression.h
        src/expression.cpp
        tests/parseExpressionTest.cpp)

target_compile_definitions(tests PUBLIC TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/resources/parseJSON")

target_link_libraries(tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)

add_link_options(-static -static-libgcc -static-libstdc++)