cmake_minimum_required(VERSION 3.14)
project(FunctionLibrary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Библиотека функций
add_library(functions
    functions.cpp
    operators.cpp
    gradient.cpp
)

target_include_directories(functions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Тесты
add_executable(tests tests.cpp)
target_link_libraries(tests functions GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)
