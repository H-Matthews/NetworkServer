cmake_minimum_required(VERSION 3.30)
project ( TestDriver-Server LANGUAGES CXX)

set(BINARY_NAME TestDriver-Server)

# Set Source Files
set(SRC_FILES main.cpp)

add_executable( ${BINARY_NAME} ${SRC_FILES} )

# Linker Options
target_link_libraries(${BINARY_NAME} PRIVATE Network-Server)

# Turn warnings on for Unix
target_compile_options( ${BINARY_NAME} PRIVATE -Wall -Wextra -Wpedantic )

# Set include directories for pathing
target_include_directories( ${BINARY_NAME} PRIVATE ${TEST_DRIVER_SERVER_SRC} ${NETWORK_SRC_LIB} ${CLIENT_SRC_LIB} )