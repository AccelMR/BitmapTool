cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 17)

project(FundamentosGraficas)

add_executable(FundamentosGraficas main.cpp src/Image.cpp src/Color.cpp)

target_include_directories(FundamentosGraficas PRIVATE include)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
endif()