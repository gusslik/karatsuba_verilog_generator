cmake_minimum_required(VERSION 3.14)
project(karatsuba_verilog_generator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(generator
    src/main.cpp
    src/generator.cpp
    src/tb_generator.cpp
)

target_include_directories(generator PRIVATE src)