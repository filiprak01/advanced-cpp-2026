file(GLOB_RECURSE COMMON_SOURCES src/*.cpp)
file(GLOB_RECURSE COMMON_HEADERS include/*.hpp)

add_library(commonLib STATIC ${COMMON_SOURCES} ${COMMON_HEADERS})

target_include_directories(commonLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_package(nlohmann_json REQUIRED)
target_link_libraries(commonLib PUBLIC nlohmann_json::nlohmann_json)
