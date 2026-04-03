# Sets CMake Version
cmake_minimum_required(VERSION 3.15)
# Calls Project 'detect'
project(detect)
# Sets CPP Versions
set(CMAKE_CXX_STANDARD 17)
# FIND DEPENDENCIES
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(OpenMP) # Optional
# Adds '-fopenmp'
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
# Redirect to src/
add_subdirectory(src)

