pkg_check_modules(libudev REQUIRED libudev)

add_executable(${EXEC_NAME} main.cpp)

target_include_directories(${EXEC_NAME} PRIVATE "${libudev_INCLUDE_DIRS}")

target_link_libraries(${EXEC_NAME} PRIVATE "${libudev_LIBRARIES}")

