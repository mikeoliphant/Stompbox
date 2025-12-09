add_subdirectory(stompbox-core)
add_subdirectory(stompbox-stomps)

add_library(stompbox STATIC)

target_link_libraries(stompbox PUBLIC stompbox-core)
target_link_libraries(stompbox PUBLIC stompbox-stomps)
