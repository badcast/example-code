pkg_check_modules(OpenCL REQUIRED OpenCL)
add_executable(${EXEC_NAME} main.cpp)
target_link_libraries(${EXEC_NAME} PRIVATE ${OpenCL_LIBRARIES})
