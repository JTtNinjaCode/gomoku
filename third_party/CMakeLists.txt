add_subdirectory(glfw)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
add_library(imgui STATIC
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
    "${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp"
)
target_include_directories(imgui PUBLIC "${IMGUI_DIR}")
target_include_directories(imgui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/glfw/include")
target_include_directories(imgui INTERFACE "${IMGUI_DIR}/misc/cpp")
target_link_libraries(imgui PUBLIC glfw)
