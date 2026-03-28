cmake_minimum_required(VERSION 3.22)
project(AdvancedCPP2026 VERSION 1.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use g++ explicitly if needed
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER g++)
endif()

# Include vcpkg toolchain if present
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# ---- SOURCES ----
file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE HEADER_FILES src/*.hpp)

# ---- LIBRARY ----
add_library(coreLib STATIC ${SRC_FILES} ${HEADER_FILES})
target_include_directories(coreLib PUBLIC include src)
target_link_libraries(coreLib PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

# ---- MAIN EXECUTABLE ----
add_executable(PWChat src/main.cpp)
target_link_libraries(PWChat PRIVATE coreLib)

# ---- TESTING ----
enable_testing()
add_subdirectory(tests)