cmake_minimum_required(VERSION 3.16)
project(gomoku CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (WIN32)
  set(LIBTORCH_URL https://download.pytorch.org/libtorch/cu126/libtorch-win-shared-with-deps-2.11.0%2Bcu126.zip)
elseif (UNIX AND NOT APPLE)
  set(LIBTORCH_URL https://download.pytorch.org/libtorch/cu126/libtorch-shared-with-deps-2.11.0%2Bcu126.zip)
else()
  message(FATAL_ERROR "Unsupported platform for LibTorch download")
endif()

include(FetchContent)
FetchContent_Declare(
  libtorch
  URL ${LIBTORCH_URL}
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_GetProperties(libtorch)
if (NOT libtorch_POPULATED)
  message(STATUS "LibTorch is not available locally. Downloading from: ${LIBTORCH_URL}")
endif()
FetchContent_MakeAvailable(libtorch)
find_package(Torch REQUIRED HINTS "${libtorch_SOURCE_DIR}")

add_subdirectory(third_party)

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(gomoku ${SOURCES})
target_include_directories(gomoku PRIVATE include)
# LibTorch headers use GNU extensions incompatible with -Wpedantic
target_compile_options(gomoku PRIVATE -Wall -Wextra)
target_link_libraries(gomoku PRIVATE imgui glfw OpenGL::GL "${TORCH_LIBRARIES}")
