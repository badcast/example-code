cmake_minimum_required(VERSION 3.5)

project(examples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(PkgConfig REQUIRED)

file(GLOB_RECURSE TARGETS src/*.cpp src/*.c)

foreach(SRC ${TARGETS})

  get_filename_component(EXAMPLE_PROJECT "${SRC}" PATH)


  string(FIND ${EXAMPLE_PROJECT} "/" fnd_start REVERSE)
  string(LENGTH ${EXAMPLE_PROJECT} fnd_length)
  math(EXPR fnd_start "${fnd_start} + 1")
  string(SUBSTRING ${EXAMPLE_PROJECT} ${fnd_start} ${fnd_length}
                    EXEC_NAME)

  if(EXISTS "${EXAMPLE_PROJECT}/CMakeLists.txt")

    add_subdirectory("${EXAMPLE_PROJECT}")

  else()

    add_executable(${EXEC_NAME} ${SRC})

  endif()

endforeach()
