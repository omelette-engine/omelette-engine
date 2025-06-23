cmake_minimum_required(VERSION 3.10)
project(omelette-engine)

set(CMAKE_CXX_STANDARD 17)


find_package(glfw3 REQUIRED)

set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
)

set(SOURCES
    main.cpp
    EBO.cpp
    shaderClass.cpp
    VAO.cpp
    VBO.cpp
    Camera.cpp
    Mesh.cpp
    editor/Editor.cpp
    primitive_generator.cpp
    glad/src/glad.c
    ${IMGUI_SOURCES}
)

add_executable(omelette ${SOURCES})

target_include_directories(omelette PRIVATE
    glad/include/
    imgui/
)

target_link_libraries(omelette
    glfw
    GL
    dl
)

file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})