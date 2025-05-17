# Root CMake File
cmake_minimum_required(VERSION 3.30)
project(HTTPServer LANGUAGES CXX)

enable_language(C CXX)

# Server & Client Libs
set(NETWORK_SRC_LIB ${PROJECT_SOURCE_DIR}/src/NetworkServer)
set(CLIENT_SRC_LIB ${PROJECT_SOURCE_DIR}/src/Client)

# TestDriver Binary
set(TESTDRIVER_SRC ${PROJECT_SOURCE_DIR}/src/TestDriver)

add_subdirectory(src/NetworkServer)
add_subdirectory(src/Client)
add_subdirectory(src/TestDriver)