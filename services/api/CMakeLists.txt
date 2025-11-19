project(api_server C)

set(SOURCES src/main.c)
add_executable(api_server ${SOURCES})

# Link pthread for multi-threading
target_link_libraries(api_server pthread)
