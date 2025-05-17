cmake_minimum_required(VERSION 3.30)
project ( Network-Server LANGUAGES CXX)

set(LIBRARY_NAME Network-Server)

# Set Source Files
set(INC_FILES NetworkServer.h)
set(SRC_FILES NetworkServer.cpp)

add_library( ${LIBRARY_NAME} STATIC ${INC_FILES} ${SRC_FILES} )

# Turn warnings on for Unix
target_compile_options( ${LIBRARY_NAME} PRIVATE -Wall -Wextra -Wpedantic )

# Set include directories for pathing
target_include_directories( ${LIBRARY_NAME} PRIVATE ${NETWORK_SRC_LIB} )