# Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.15)

# Define the project name and version
project(multyExample VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include Conan's toolchain and dependencies
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)
#include(./dest/conan_toolchain.cmake)
find_package(Boost REQUIRED COMPONENTS program_options)

# Add the executable for the console application
add_executable(multyExample multy/two_threads_task.cpp)

# Link Boost libraries
target_link_libraries(multyExample Boost::program_options)
