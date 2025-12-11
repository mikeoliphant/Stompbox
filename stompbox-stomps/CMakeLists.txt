add_subdirectory(${STOMPBOX_DEPS_PATH}/NeuralAudio NeuralAudio)

file(GLOB_RECURSE SOURCES *.cpp *.c *.h)

set(NA_SOURCES ${STOMPBOX_DEPS_PATH}/NeuralAudio/NeuralAudio/NeuralModel.h)

set(OTHER_SOURCES "${STOMPBOX_DEPS_PATH}/WDL/WDL/convoengine.cpp"
	"${STOMPBOX_DEPS_PATH}/WDL/WDL/fft.c")

add_library(stompbox-stomps OBJECT ${SOURCES} ${NA_SOURCES} ${OTHER_SOURCES})

target_include_directories(stompbox-stomps SYSTEM PUBLIC ../stompbox-core)
target_include_directories(stompbox-stomps SYSTEM PUBLIC ${STOMPBOX_DEPS_PATH}/NeuralAudio)
target_include_directories(stompbox-stomps SYSTEM PUBLIC ${STOMPBOX_DEPS_PATH}/WDL/WDL)

target_link_libraries(stompbox-stomps PRIVATE NeuralAudio)

set_property(TARGET stompbox-stomps PROPERTY POSITION_INDEPENDENT_CODE ON)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group(NeuralAudio ${CMAKE_CURRENT_SOURCE_DIR} FILES ${NA_SOURCES})

add_definitions(-DWDL_FFT_REALSIZE=4)
add_definitions(-DWDL_CONVO_THREAD)
add_definitions(-DWDL_ALLOW_UNSIGNED_DEFAULT_CHAR)


