file(GLOB_RECURSE SOURCES *.cpp *.c *.h)

set(OTHER_SOURCES "${STOMPBOX_DEPS_PATH}/r8brain-free-src/r8bbase.cpp"
	"${STOMPBOX_DEPS_PATH}/r8brain-free-src/pffft.cpp")

add_library(stompbox-core OBJECT ${SOURCES} ${OTHER_SOURCES})

target_compile_features(stompbox-core PUBLIC cxx_std_17)

set_property(TARGET stompbox-core PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(stompbox-core SYSTEM PUBLIC ${STOMPBOX_DEPS_PATH}/denormal)
target_include_directories(stompbox-core SYSTEM PUBLIC ${STOMPBOX_DEPS_PATH}/dr_libs)
target_include_directories(stompbox-core SYSTEM PUBLIC ${STOMPBOX_DEPS_PATH}/r8brain-free-src)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

