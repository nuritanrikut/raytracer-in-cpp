cmake_minimum_required(VERSION 3.5)

project(raytracer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

set(SOURCES
    src/main.cpp
    src/utils.cpp
    src/vec3.cpp
)

add_executable(raytracer ${SOURCES})

target_link_libraries(raytracer PRIVATE OpenMP::OpenMP_CXX pthread fmt)
target_link_libraries(raytracer PUBLIC OpenMP::OpenMP_CXX)
