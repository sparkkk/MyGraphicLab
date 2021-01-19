cmake_minimum_required (VERSION 3.9)


if (APPLE)
    set(CMAKE_C_COMPILER "clang")
    set(CMAKE_CXX_COMPILER "clang++")
else()
    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")
endif()

project (MyGraphicLab)

file(
    GLOB source_files
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/lodepng/lodepng.cpp"
    "${CMAKE_SOURCE_DIR}/src/glad/src/glad.c"
)

include_directories(
    "${CMAKE_SOURCE_DIR}/src/lodepng"
    "${CMAKE_SOURCE_DIR}/src/glad/include"
)

add_compile_options(-std=c++17 -g -O0)

include_directories(
    "${MY_LOCAL_PATH}/include"
)

link_directories(
    "${MY_LOCAL_PATH}/lib"
)

add_executable(MyGraphicLab ${source_files})

if (APPLE)

target_link_libraries(
    MyGraphicLab
    glfw
    "-framework Foundation"
    "-framework OpenGL"
    "-framework AppKit"
    "-framework IOKit"
)

else()

target_link_libraries(
    MyGraphicLab
    glfw3
    stdc++
    opengl32
)

endif()
