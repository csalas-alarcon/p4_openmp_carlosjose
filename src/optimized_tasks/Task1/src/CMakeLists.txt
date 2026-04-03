# Sets CMake Version
cmake_minimum_required(VERSION 3.15)
# Calls Project 'detect'
project(detect)
# Redirect to utils/
add_subdirectory(utils)
# Including Headers
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PNG_INCLUDE_DIR})
include_directories(${JPEG_INCLUDE_DIR})
# bin/ Location
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
# Make Executable
add_executable(${PROJECT_NAME} main.cc)
# Adds Links
target_link_libraries(${PROJECT_NAME} PNG::PNG JPEG::JPEG utils)