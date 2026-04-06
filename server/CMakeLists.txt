file(GLOB_RECURSE SERVER_SOURCES src/*.cpp)
file(GLOB_RECURSE SERVER_HEADERS include/*.hpp)

add_library(serverLib STATIC ${SERVER_SOURCES} ${SERVER_HEADERS})

target_include_directories(serverLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(serverLib PUBLIC commonLib)

find_package(OpenSSL REQUIRED)
target_link_libraries(serverLib PUBLIC OpenSSL::SSL OpenSSL::Crypto)
