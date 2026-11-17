cmake_minimum_required(VERSION 3.20)
project(lab03_variant12 LANGUAGES CXX)

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

add_library(book_set STATIC book.cpp book_set.cpp)
add_executable(lab03 main.cpp)
target_link_libraries(lab03 PRIVATE book_set)

enable_testing()
add_executable(lab_03_tests test_book_set.cpp)
target_link_libraries(lab_03_tests PRIVATE book_set GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(lab_03_tests)
