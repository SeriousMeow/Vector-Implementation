cmake_minimum_required(VERSION 3.10)

project(Test_vector)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(test_vector test.cpp)

target_compile_options(test_vector PRIVATE
    -g
    -Wall
    -fsanitize=address
    )
target_link_options(test_vector PRIVATE
    -fsanitize=address
    )


