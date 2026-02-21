cmake_minimum_required(VERSION 3.20)
project(PWChat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find Qt6 modules
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

# Gather source files
file(GLOB SRC_FILES src/*.cpp)

# Main executable
add_executable(PWChat ${SRC_FILES})
target_include_directories(PWChat PRIVATE ../include)
target_link_libraries(PWChat Qt6::Core Qt6::Widgets Qt6::Gui)

# Enable testing
enable_testing()
add_subdirectory(tests)