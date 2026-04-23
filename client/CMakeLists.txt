file(GLOB_RECURSE CLIENT_LIB_SOURCES
    src/client_backend.cpp
    src/response_dispatcher.cpp
    src/socket_connection.cpp
)
file(GLOB_RECURSE CLIENT_HEADERS include/*.hpp)

add_library(clientLib STATIC ${CLIENT_LIB_SOURCES} ${CLIENT_HEADERS})

target_include_directories(clientLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(clientLib PUBLIC commonLib)

if(BUILD_CLIENT_BACKEND)
    add_executable(PWChatClient src/main.cpp ${CLIENT_HEADERS})

    target_include_directories(PWChatClient PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_link_libraries(PWChatClient PRIVATE clientLib)

    set_target_properties(PWChatClient PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/client
    )
endif()
