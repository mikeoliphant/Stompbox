if($ENV{CI})
    message(STATUS "Configuring in CI, skipping benchmarks...")
    return()
endif()

CPMAddPackage(
    NAME benchmark
    GITHUB_REPOSITORY google/benchmark
    VERSION 1.5.2
    OPTIONS "BENCHMARK_ENABLE_TESTING Off"
)

function(setup_bench target src)
    add_executable(${target} ${src})
    target_link_libraries(${target} PRIVATE benchmark::benchmark math_approx)
    target_compile_features(${target} PUBLIC cxx_std_20)
endfunction(setup_bench)

setup_bench(trig_approx_bench trig_bench.cpp)
setup_bench(inverse_trig_approx_bench inverse_trig_bench.cpp)
setup_bench(pow_approx_bench pow_bench.cpp)
setup_bench(log_approx_bench log_bench.cpp)
setup_bench(hyperbolic_trig_approx_bench hyperbolic_trig_bench.cpp)
setup_bench(inverse_hyperbolic_trig_approx_bench inverse_hyperbolic_trig_bench.cpp)
setup_bench(sigmoid_approx_bench sigmoid_bench.cpp)
setup_bench(wright_omega_approx_bench wright_omega_bench.cpp)
setup_bench(polylog_approx_bench polylog_bench.cpp)
