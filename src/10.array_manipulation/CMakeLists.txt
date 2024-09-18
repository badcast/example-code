add_executable(${EXEC_NAME} main.cpp)
target_compile_options(${EXEC_NAME} PRIVATE -fopenmp)
target_link_libraries(${EXEC_NAME} PRIVATE -fopenmp)
