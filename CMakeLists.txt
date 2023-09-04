cmake_minimum_required(VERSION 3.5)

project(examples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp /lm")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /openmp /lm")
elseif(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -lm")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp -lm")
endif()

find_package(PkgConfig REQUIRED)

pkg_check_modules(OpenCL REQUIRED OpenCL)

file(GLOB_RECURSE TARGETS src/*.cpp src/*.c)

foreach(SRC ${TARGETS})
  get_filename_component(EXAMPLE_PROJECT "${SRC}" PATH)
  string(FIND ${EXAMPLE_PROJECT} "/" fnd_start REVERSE)
  string(LENGTH ${EXAMPLE_PROJECT} fnd_length)
  math(EXPR fnd_start "${fnd_start} + 1")
  string(SUBSTRING ${EXAMPLE_PROJECT} ${fnd_start} ${fnd_length}
                   EXAMPLE_PROJECT)
  add_executable(${EXAMPLE_PROJECT} ${SRC})
  target_link_libraries(${EXAMPLE_PROJECT} PRIVATE ${OpenCL_LIBRARIES})
endforeach()
