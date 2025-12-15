set(SOURCES
	SimpleIO.cpp
	SimpleIO.h
)

set(STOMP_SOURCES
	../../stompbox-stomps/Gain.h
	../../stompbox-stomps/Gain.cpp
)

set(SIMPLE_IO_LV2_ID https://github.com/mikeoliphant/simple_io)

add_library(simple_io SHARED ${SOURCES} ${STOMP_SOURCES})

set_property(TARGET simple_io PROPERTY POSITION_INDEPENDENT_CODE ON)

set_target_properties(simple_io
	PROPERTIES
	PREFIX ""
)

target_include_directories(simple_io PUBLIC ../Dependencies/lv2/include)
target_include_directories(simple_io PUBLIC .. ../../stompbox-stomps)

target_link_libraries(simple_io PRIVATE stompbox-lv2)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

configure_file(manifest.ttl.in simple_io.lv2/manifest.ttl)
configure_file(simple_io.ttl.in simple_io.lv2/simple_io.ttl)

add_custom_target(create_lv2 ALL
	${CMAKE_COMMAND} -E copy "$<TARGET_FILE:simple_io>" simple_io.lv2/
	DEPENDS simple_io
)

