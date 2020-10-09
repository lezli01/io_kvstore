project(server)
cmake_minimum_required(VERSION 3.0)

add_library(map
        src/server/map.c
)

add_executable(server
    src/server/main.c
)

target_link_libraries(server
    map
    pthread
)

add_executable(unit
    test/main.c
    test/test_map.c
)

target_link_libraries(unit
    map
)