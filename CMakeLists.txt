cmake_minimum_required(VERSION 3.16)
project(gomoku CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(gomoku ${SOURCES})
target_include_directories(gomoku PRIVATE include)
target_compile_options(gomoku PRIVATE -Wall -Wextra -Wpedantic)
