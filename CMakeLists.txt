cmake_minimum_required(VERSION 3.28)
project(gomoku CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(USE_GPU "Download CUDA version of LibTorch" ON)

if(USE_GPU)
  set(TORCH_VERSION "2.11.0%2Bcu126")
  set(TORCH_PREFIX "cu126")
else()
  set(TORCH_VERSION "2.11.0%2Bcpu")
  set(TORCH_PREFIX "cpu")
endif()

if (WIN32)
  set(LIBTORCH_URL "https://download.pytorch.org/libtorch/${TORCH_PREFIX}/libtorch-win-shared-with-deps-${TORCH_VERSION}.zip")
elseif (UNIX AND NOT APPLE)
  set(LIBTORCH_URL "https://download.pytorch.org/libtorch/${TORCH_PREFIX}/libtorch-shared-with-deps-${TORCH_VERSION}.zip")
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
  message(STATUS "LibTorch is not available. Downloading from: ${LIBTORCH_URL}")
endif()
FetchContent_MakeAvailable(libtorch)
find_package(Torch REQUIRED HINTS "${libtorch_SOURCE_DIR}")

add_subdirectory(third_party)

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(gomoku ${SOURCES})
target_include_directories(gomoku PRIVATE include)

if(MSVC)
  # Windows / MSVC
  target_compile_options(gomoku PRIVATE /W4)
else()
  # Linux (GCC/Clang)
  target_compile_options(gomoku PRIVATE -Wall -Wextra)
endif()

target_link_libraries(gomoku PRIVATE imgui glfw OpenGL::GL "${TORCH_LIBRARIES}")

if (WIN32)
  file(GLOB TORCH_DLLS "${libtorch_SOURCE_DIR}/lib/*.dll")
  add_custom_command(TARGET gomoku POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${TORCH_DLLS}
    $<TARGET_FILE_DIR:gomoku>
  )
endif()
