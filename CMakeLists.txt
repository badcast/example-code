cmake_minimum_required(VERSION 3.5)

project(examples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-fopenmp")

find_package(OpenMP REQUIRED)
file(GLOB_RECURSE TARGETS src/*.cpp)

foreach(SRC  ${TARGETS})
    get_filename_component(EXAMPLE_PROJECT "${SRC}" PATH)
    string(FIND ${EXAMPLE_PROJECT} "/" fnd_start REVERSE)
    string(LENGTH ${EXAMPLE_PROJECT} fnd_length)
    math(EXPR fnd_start "${fnd_start} + 1")
    string(SUBSTRING ${EXAMPLE_PROJECT} ${fnd_start} ${fnd_length} EXAMPLE_PROJECT)
    add_executable(${EXAMPLE_PROJECT} ${SRC})
endforeach()

