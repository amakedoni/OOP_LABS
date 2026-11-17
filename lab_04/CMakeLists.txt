cmake_minimum_required(VERSION 3.20)
project(lab04_variant6 LANGUAGES CXX)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(googletest
    URL https://codeload.github.com/google/googletest/tar.gz/refs/tags/v1.17.0
    URL_HASH SHA256=65fab701d9829d38cb77c14acdc431d2108bfdbf8979e40eb8ae567edf10b27c
)
FetchContent_MakeAvailable(googletest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(electronics STATIC component.cpp collection.cpp)
add_executable(lab04 main.cpp)
target_link_libraries(lab04 PRIVATE electronics)

enable_testing()
add_executable(lab_04_tests test_electronics.cpp)
target_link_libraries(lab_04_tests PRIVATE electronics GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(lab_04_tests)
