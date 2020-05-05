cmake_minimum_required(VERSION 3.14)
project(spac2 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src/Common)
add_subdirectory(src/Server)
add_subdirectory(src/Client)