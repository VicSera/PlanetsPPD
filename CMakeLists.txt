cmake_minimum_required(VERSION 3.21)
project(Planets)

set(CMAKE_CXX_STANDARD 23)
add_compile_options("-pthread")
add_link_options("-static-libgcc" "-static-libstdc++")

add_executable(Planets main.cpp physics.cpp graphics.cpp)

