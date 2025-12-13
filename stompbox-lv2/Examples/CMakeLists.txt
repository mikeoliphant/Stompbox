set(SOURCES
	SimpleIO.cpp
	SimpleIO.h
)

set(STOMP_SOURCES
	../../stompbox-stomps/Gain.h
	../../stompbox-stomps/Gain.cpp
)

add_library(simple_io SHARED ${SOURCES} ${STOMP_SOURCES})

set_property(TARGET simple_io PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(simple_io PUBLIC ../Dependencies/lv2/include)
target_include_directories(simple_io PUBLIC .. ../../stompbox-stomps)

target_link_libraries(simple_io PRIVATE stompbox-lv2)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

