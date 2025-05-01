# Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.15)

# Define the project name and version
project(FileTransfer VERSION 1.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include Conan's toolchain and dependencies
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/multy/include)

find_package(GTest REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

# Add the executable for the console application
add_executable(FileTransfer
    "multy/main.cpp"
    "multy/src/Threads/MultiThreadTask.cpp"
    "multy/src/MultiProcess/MultiProcessTask.cpp"
    "multy/src/Network/NetworkTask.cpp"
    "multy/src/Factory/TaskFactory.cpp" 
    "multy/src/Factory/CommandFactory.cpp" 
    "multy/src/Factory/FileStreamFactory.cpp" 
    )

# Link Boost libraries
target_link_libraries(FileTransfer Boost::program_options)


# Enable testing and include the GoogleTest module
enable_testing()
include(GoogleTest)

add_executable(test_FileTransfer 
"multy/test/IntegrationTest.cpp" 
"multy/src/Threads/MultiThreadTask.cpp"
"multy/src/MultiProcess/MultiProcessTask.cpp"
"multy/src/Network/NetworkTask.cpp" 
"multy/src/Factory/TaskFactory.cpp"
"multy/src/Factory/CommandFactory.cpp" 
"multy/src/Factory/FileStreamFactory.cpp" 
)

target_link_libraries(test_FileTransfer
    PRIVATE
    GTest::gmock
    GTest::gtest_main
    Boost::program_options
)
gtest_discover_tests(test_FileTransfer)
