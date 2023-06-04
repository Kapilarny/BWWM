cmake_minimum_required(VERSION 3.10)
project(BWWM VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)

# Set it to be a Win32 application
set(CMAKE_WIN32_EXECUTABLE ON)

# add_subdirectory(external/glad/cmake glad_cmake)
# add_subdirectory(external/glfw glfw)

# glad_add_library(glad_gl_core_mx_33 REPRODUCIBLE MX API gl:core=3.3)

find_library(WebView2Loader_Lib
    NAMES WebView2Loader.dll
    PATHS ${CMAKE_SOURCE_DIR}/external/webview2/build/native/x64
)

# Add the .cpp files recursively from the src directory
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${WebView2Loader_Lib})

# Add include directory for webview2 and wil
include_directories(${CMAKE_SOURCE_DIR}/external/webview2/build/native/include ${CMAKE_SOURCE_DIR}/external/wil/include)

# Copy the webview2 dll to the build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/external/webview2/build/native/x64/WebView2Loader.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)