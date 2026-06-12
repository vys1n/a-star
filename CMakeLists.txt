
cmake_minimum_required(VERSION 3.20)

project(a-star)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

add_executable(a-star main.cpp)

target_link_libraries(a-star PRIVATE raylib)
