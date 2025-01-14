cmake_minimum_required(VERSION 3.19)

set (CMAKE_CXX_STANDARD 17)

project(BitmapTool)

add_executable(BitmapTool main.cpp src/Image.cpp src/Color.cpp)

target_include_directories(BitmapTool PRIVATE include)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(_DEBUG)
endif()