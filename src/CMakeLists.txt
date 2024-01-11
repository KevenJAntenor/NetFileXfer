add_library(core
    core.c
    core.h

    xfer.c
    xfer.h

    utils.c
    utils.h
)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../include)

add_executable(client client.c)
target_link_libraries(client PRIVATE core)

add_executable(server server.c)
target_link_libraries(server PRIVATE core)

#add_executable(benchmark benchmark.c)
#target_link_libraries(benchmark PRIVATE core)
