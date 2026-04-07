cmake_minimum_required(VERSION 3.22)
project(PWChat VERSION 1.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use g++ explicitly if needed
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER g++)
endif()

# Include vcpkg toolchain if present
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# ---- BIBLIOTEKI ----
add_subdirectory(common)
add_subdirectory(server)

# ---- GŁÓWNY PLIK WYKONYWALNY ----
add_executable(PWChat src/main.cpp)
target_include_directories(PWChat PRIVATE src)
target_link_libraries(PWChat PRIVATE serverLib)
set_target_properties(PWChat PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/server
)

# ---- TESTY ----
enable_testing()
add_subdirectory(tests)