cmake_minimum_required(VERSION 3.16)
project(lab_01 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(lab01_lib array_ops.cpp)
add_executable(lab01_main main.cpp)
target_link_libraries(lab01_main PRIVATE lab01_lib)

enable_testing()
include(FetchContent)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()
FetchContent_Declare(googletest
    URL https://codeload.github.com/google/googletest/tar.gz/refs/tags/v1.14.0
    URL_HASH SHA256=8ad598c73ad796e0d8280b082cebd82a630d73e73cd3c70057938a6501bba5d7)
FetchContent_MakeAvailable(googletest)

add_executable(lab01_tests tests.cpp)
target_link_libraries(lab01_tests PRIVATE lab01_lib GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(lab01_tests)
