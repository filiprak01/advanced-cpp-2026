cmake_minimum_required(VERSION 3.22)
project(PWChat VERSION 1.0.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_SERVER_BACKEND "Build the server backend executable." ON)
option(BUILD_CLIENT_BACKEND "Build the client backend smoke-test executable." ON)
option(BUILD_BACKEND_TESTS "Build the backend test suite." OFF)

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
add_subdirectory(client)

# ---- GŁÓWNY PLIK WYKONYWALNY ----
if(BUILD_SERVER_BACKEND)
    add_executable(PWChat src/main.cpp)
    target_include_directories(PWChat PRIVATE src)
    target_link_libraries(PWChat PRIVATE serverLib)
    target_compile_definitions(PWChat PRIVATE
        PWCHAT_SERVER_CONFIG_PATH="${CMAKE_SOURCE_DIR}/config/server.json"
    )
    set_target_properties(PWChat PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/server
    )
endif()

# ---- TESTY ----
if(BUILD_BACKEND_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
