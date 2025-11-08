if($ENV{CI})
    message(STATUS "Configuring in CI, skipping plotter tool...")
    return()
endif()

CPMAddPackage(
    NAME matplotlib-cpp
    GIT_REPOSITORY https://github.com/jatinchowdhury18/matplotlib-cpp
    GIT_TAG main
)

add_executable(math_approx_plotter plotter.cpp)
target_link_libraries(math_approx_plotter PRIVATE matplotlib-cpp math_approx)
target_compile_features(math_approx_plotter PUBLIC cxx_std_20)
