cmake_minimum_required(VERSION 3.0)
project(pr1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(TARGET_NAME "${PROJECT_NAME}")

add_executable(${TARGET_NAME})
target_sources(
    ${TARGET_NAME}
    PRIVATE
        src/main.cpp
        src/color.cpp
        src/scene.cpp
        src/ray.cpp
        src/camera.cpp
        src/object.cpp
        src/quat.cpp
)

target_include_directories(${TARGET_NAME} PRIVATE inc)
