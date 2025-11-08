cmake_minimum_required(VERSION 3.18)
project(math_approx VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)

if(PROJECT_IS_TOP_LEVEL)
    include(CPM)
    CPMAddPackage(
        NAME xsimd
        GIT_REPOSITORY https://github.com/xtensor-stack/xsimd
        GIT_TAG master
    )
endif()

add_library(math_approx INTERFACE)
target_include_directories(math_approx INTERFACE include)
if(MSVC)
    target_compile_definitions(math_approx INTERFACE _USE_MATH_DEFINES=1)
endif()
if (TARGET xsimd)
    message(STATUS "math_approx -- Linking with XSIMD...")
    target_link_libraries(math_approx INTERFACE xsimd)
    target_compile_definitions(math_approx INTERFACE MATH_APPROX_XSIMD_TARGET=1)
endif()

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
    add_subdirectory(test)
    add_subdirectory(tools)
endif()
