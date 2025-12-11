set(SOURCES
	StompboxLV2.cpp
	StompboxLV2Plugin.cpp
	StompboxLV2Plugin.h
)

add_library(stompbox-lv2 STATIC ${SOURCES})

target_include_directories(stompbox-lv2 PUBLIC ../stompbox/stompbox-core)
target_include_directories(stompbox-lv2 PUBLIC Dependencies/lv2/include)

target_link_libraries(stompbox-lv2 PUBLIC stompbox-core)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_subdirectory(Examples)