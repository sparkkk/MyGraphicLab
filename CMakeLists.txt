cmake_minimum_required (VERSION 3.9)

project (MyGraphicLab)

add_custom_command(
    OUTPUT "${CMAKE_SOURCE_DIR}/modules/glew/src/glew.c"
    DEPENDS "${CMAKE_SOURCE_DIR}/modules/glew/auto/Makefile"
    COMMAND make -j8
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/modules/glew/auto"
)

add_custom_command(
    OUTPUT "${CMAKE_SOURCE_DIR}/modules/glew/lib/libGLEW.a"
    DEPENDS "${CMAKE_SOURCE_DIR}/modules/glew/src/glew.c"
    COMMAND make -j8 glew.lib.static
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/modules/glew"
)

add_custom_command(
    OUTPUT "${CMAKE_SOURCE_DIR}/modules/glfw/src/libglfw3.a"
    COMMAND make -j8
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/modules/glfw/src"
)

add_custom_target(
    TargetGLFW
    SOURCES "${CMAKE_SOURCE_DIR}/modules/glfw/src/libglfw3.a"
)

add_custom_target(
    TargetGLEW
    SOURCES "${CMAKE_SOURCE_DIR}/modules/glew/lib/libGLEW.a"
)

file(
    GLOB source_files
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/modules/lodepng/lodepng.cpp"
)

include_directories(
    "${CMAKE_SOURCE_DIR}/modules/glew/include"
    "${CMAKE_SOURCE_DIR}/modules/glfw/include"
    "${CMAKE_SOURCE_DIR}/modules/json/include"
    "${CMAKE_SOURCE_DIR}/modules/glm"
    "${CMAKE_SOURCE_DIR}/modules/lodepng"
)

link_directories(
    "${CMAKE_SOURCE_DIR}/modules/glew/lib"
    "${CMAKE_SOURCE_DIR}/modules/glfw/src"
)

add_compile_options(-std=c++17 -g -O0)

add_executable(MyGraphicLab ${source_files})

add_dependencies(
    MyGraphicLab
    TargetGLFW
    TargetGLEW
)

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
    GLEW
)

endif()
