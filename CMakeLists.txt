cmake_minimum_required (VERSION 3.9)

project (MyGraphicLab)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

file(
    GLOB source_files
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/lodepng/lodepng.cpp"
)

include_directories(
    "${CMAKE_SOURCE_DIR}/src/lodepng"
)

add_compile_options(-std=c++17 -g -O0)

add_executable(MyGraphicLab ${source_files})

if (APPLE)

target_link_libraries(
    MyGraphicLab
    glfw3
    GLEW
    "-framework Foundation"
    "-framework OpenGL"
    "-framework AppKit"
    "-framework IOKit"
)

else()

target_link_libraries(
    MyGraphicLab
    glfw3
    glew32
    c++
    opengl32
)

endif()
