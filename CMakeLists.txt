cmake_minimum_required(VERSION 3.10)
project(BWWM VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)

# Add the .cpp files recursively from the src directory
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})