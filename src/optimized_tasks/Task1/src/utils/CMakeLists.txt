# Sets CMake Version
cmake_minimum_required(VERSION 3.15)
# Including Headers
include_directories(${PNG_INCLUDE_DIR})
include_directories(${JPEG_INCLUDE_DIR})
# Creates Library
add_library(utils image.h image.cc dct.h dct.cc)
# Adds Links
target_link_libraries(utils PNG::PNG JPEG::JPEG)
