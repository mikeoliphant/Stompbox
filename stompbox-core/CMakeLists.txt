file(GLOB_RECURSE SOURCES *.cpp *.c *.h)

set(OTHER_SOURCES "${STOMPBOX_DEPS_PATH}/r8brain-free-src/r8bbase.cpp"
	"${STOMPBOX_DEPS_PATH}/r8brain-free-src/pffft.cpp")

add_library(stompbox-core OBJECT ${SOURCES} ${OTHER_SOURCES})

target_include_directories(stompbox-core SYSTEM PUBLIC ${STOMPBOX_DEPS_PATH}/denormal)
target_include_directories(stompbox-core SYSTEM PUBLIC ${STOMPBOX_DEPS_PATH}/dr_libs)
target_include_directories(stompbox-core SYSTEM PUBLIC ${STOMPBOX_DEPS_PATH}/r8brain-free-src)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

