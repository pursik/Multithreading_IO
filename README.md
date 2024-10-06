# Multithreading_IO
A console application using two threads for reading from and writing to a file. 

1. Install Conan 2.0.7: Make sure Conan 2.0.7 is installed. If not, you can install it using pip:

pip install conan==2.0.7

2.Install Dependencies: Run Conan to install the Boost dependency using the conanfile.py:

conan install -of bin-release . --build=missing --settings build_type=Release
conan install -of bin-debug . --build=missing --settings build_type=Debug

This will:
Download and install the necessary Boost library (in this case, Boost.Program_Options).
Generate the CMake configuration files (like conan_toolchain.cmake and Boost-config.cmake) to integrate Boost into your project.
