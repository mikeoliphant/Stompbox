add_executable(stompbox-jack stompbox-jack.cpp)

# Should remove all of these - PluginProcessor.h pulls them in
include_directories(stompbox-jack ${STOMPBOX_DEPS_PATH}/eigen)
include_directories(stompbox-jack ${STOMPBOX_DEPS_PATH}/NeuralAmpModelerCore/NAM)
include_directories(stompbox-jack ${STOMPBOX_DEPS_PATH}/json)
include_directories(stompbox-jack ${STOMPBOX_DEPS_PATH}/denormal)
include_directories(stompbox-jack ../stompbox ../stompbox/Stomps)

target_link_libraries(stompbox-jack PUBLIC ${JACK_LIB} PRIVATE stompbox)

target_compile_features(stompbox-jack PUBLIC cxx_std_17)

set_target_properties(stompbox-jack
	PROPERTIES
	CXX_VISIBILITY_PRESET hidden
	INTERPROCEDURAL_OPTIMIZATION TRUE
	PREFIX ""
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    include_directories(stompbox-jack C:/Program\ Files/JACK2/include)
	target_compile_definitions(stompbox-jack PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()

if (MSVC)
	target_compile_options(stompbox-jack PRIVATE
		"$<$<CONFIG:DEBUG>:/W4>"
		"$<$<CONFIG:RELEASE>:/O2>"
	)
else()
	target_compile_options(stompbox-jack PRIVATE
		-Wall -Wextra -Wpedantic -Wshadow -Wstrict-aliasing
		-Wunreachable-code -Wdouble-promotion -Weffc++ -Wconversion
		-Wsign-conversion
		"$<$<CONFIG:DEBUG>:-Og;-ggdb;-Werror>"
		"$<$<CONFIG:RELEASE>:-Ofast>"
	)
endif()
