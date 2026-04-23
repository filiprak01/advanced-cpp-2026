# Gather all test cpp files recursively
file(GLOB_RECURSE TEST_SOURCES
    models/*.cpp
    repos/*.cpp
    managers/*.cpp
    client/*.cpp
    integration/*.cpp
)

# Build the test executable
add_executable(runTests ${TEST_SOURCES})

# Include source headers
target_include_directories(runTests PRIVATE ../src)

# Link the library + GTest
find_package(GTest REQUIRED)
target_link_libraries(runTests PRIVATE serverLib clientLib GTest::gtest_main)

# Enable CTest discovery
include(GoogleTest)
gtest_discover_tests(runTests)