cmake_minimum_required(VERSION 3.16)
project(lab02_variant16 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

option(ENABLE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)
if(ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

add_library(matrix matrix.cpp)
target_include_directories(matrix PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(lab02 main.cpp)
target_link_libraries(lab02 PRIVATE matrix)

include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_Declare(googletest
        URL https://codeload.github.com/google/googletest/tar.gz/refs/tags/v1.17.0
        URL_HASH SHA256=65fab701d9829d38cb77c14acdc431d2108bfdbf8979e40eb8ae567edf10b27c
    )
    FetchContent_MakeAvailable(googletest)
    add_executable(test_matrix test_matrix.cpp)
    target_link_libraries(test_matrix PRIVATE matrix GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(test_matrix)
    foreach(scenario symmetric asymmetric rectangular basic empty invalid incompatible eof eof_create no_newline long_token)
        add_test(NAME Menu.${scenario}
            COMMAND "${CMAKE_COMMAND}" "-DPROGRAM=$<TARGET_FILE:lab02>"
                "-DSCENARIO=${scenario}" -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/check_menu.cmake")
    endforeach()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    foreach(target matrix lab02 test_matrix)
        if(TARGET ${target})
            target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        endif()
    endforeach()
endif()
