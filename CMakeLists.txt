cmake_minimum_required(VERSION 3.10)
project(BWWM VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/glad/cmake glad_cmake)
add_subdirectory(external/glfw glfw)

# Add the .cpp files recursively from the src directory
file(GLOB_RECURSE SOURCES "src/*.cpp")

glad_add_library(glad_gl_core_mx_33 REPRODUCIBLE MX API gl:core=3.3)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad_gl_core_mx_33)