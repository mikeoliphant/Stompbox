message(STATUS "math_approx -- Configuring tests...")

CPMAddPackage("gh:catchorg/Catch2@3.2.1")
include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

function(setup_catch_test target)
    add_executable(${target})
    target_sources(${target} PRIVATE src/${target}.cpp)
    target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/tests/test_utils)
    target_link_libraries(${target}
        PRIVATE
            Catch2::Catch2WithMain
            math_approx
    )
    target_compile_features(${target} PUBLIC cxx_std_20)
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )

    add_custom_command(TARGET ${target}
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E echo "Copying $<TARGET_FILE:${target}> to test-binary"
        COMMAND ${CMAKE_COMMAND} -E make_directory test-binary
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${target}>" test-binary
    )

    catch_discover_tests(${target} TEST_PREFIX ${target}_)
endfunction(setup_catch_test)

setup_catch_test(trig_approx_test)
setup_catch_test(inverse_trig_approx_test)
setup_catch_test(pow_approx_test)
setup_catch_test(log_approx_test)
setup_catch_test(hyperbolic_trig_approx_test)
setup_catch_test(inverse_hyperbolic_trig_approx_test)
setup_catch_test(sigmoid_approx_test)
setup_catch_test(wright_omega_approx_test)
setup_catch_test(polylog_approx_test)
