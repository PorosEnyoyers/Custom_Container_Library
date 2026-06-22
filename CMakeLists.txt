cmake_minimum_required(VERSION 3.20)

project(Custom_Container_Library LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(circular_buffer_demo
    example/Circular_Buffer_demo.cpp
)

target_include_directories(circular_buffer_demo PRIVATE
    include
)