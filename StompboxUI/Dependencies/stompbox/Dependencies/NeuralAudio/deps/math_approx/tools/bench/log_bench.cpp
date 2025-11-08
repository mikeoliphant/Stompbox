#include <math_approx/math_approx.hpp>
#include <benchmark/benchmark.h>

static constexpr size_t N = 2000;
const auto data = []
{
    std::vector<float> x;
    x.resize (N, 0.0f);
    for (size_t i = 0; i < N; ++i)
        x[i] = -10.0f + 20.0f * (float) i / (float) N;
    return x;
}();

#define LOG_BENCH(name, func) \
void name (benchmark::State& state) \
{ \
for (auto _ : state) \
{ \
for (auto& x : data) \
{ \
auto y = func (x); \
benchmark::DoNotOptimize (y); \
} \
} \
} \
BENCHMARK (name);
LOG_BENCH (log_std, std::log)
LOG_BENCH (log_approx6, math_approx::log<6>)
LOG_BENCH (log_approx5, math_approx::log<5>)
LOG_BENCH (log_approx4, math_approx::log<4>)
LOG_BENCH (log_approx3, math_approx::log<3>)

LOG_BENCH (log2_std, std::log2)
LOG_BENCH (log2_approx6, math_approx::log2<6>)
LOG_BENCH (log2_approx5, math_approx::log2<5>)
LOG_BENCH (log2_approx4, math_approx::log2<4>)
LOG_BENCH (log2_approx3, math_approx::log2<3>)

LOG_BENCH (log10_std, std::log10)
LOG_BENCH (log10_approx6, math_approx::log10<6>)
LOG_BENCH (log10_approx5, math_approx::log10<5>)
LOG_BENCH (log10_approx4, math_approx::log10<4>)
LOG_BENCH (log10_approx3, math_approx::log10<3>)

#define LOG_SIMD_BENCH(name, func) \
void name (benchmark::State& state) \
{ \
for (auto _ : state) \
{ \
for (auto& x : data) \
{ \
auto y = func (xsimd::broadcast (x)); \
static_assert (std::is_same_v<xsimd::batch<float>, decltype(y)>); \
benchmark::DoNotOptimize (y); \
} \
} \
} \
BENCHMARK (name);
LOG_SIMD_BENCH (log_xsimd, xsimd::log)
LOG_SIMD_BENCH (log_simd_approx6, math_approx::log<6>)
LOG_SIMD_BENCH (log_simd_approx5, math_approx::log<5>)
LOG_SIMD_BENCH (log_simd_approx4, math_approx::log<4>)
LOG_SIMD_BENCH (log_simd_approx3, math_approx::log<3>)

LOG_SIMD_BENCH (log2_xsimd, xsimd::log2)
LOG_SIMD_BENCH (log2_simd_approx6, math_approx::log2<6>)
LOG_SIMD_BENCH (log2_simd_approx5, math_approx::log2<5>)
LOG_SIMD_BENCH (log2_simd_approx4, math_approx::log2<4>)
LOG_SIMD_BENCH (log2_simd_approx3, math_approx::log2<3>)

LOG_SIMD_BENCH (log10_xsimd, xsimd::log10)
LOG_SIMD_BENCH (log10_simd_approx6, math_approx::log10<6>)
LOG_SIMD_BENCH (log10_simd_approx5, math_approx::log10<5>)
LOG_SIMD_BENCH (log10_simd_approx4, math_approx::log10<4>)
LOG_SIMD_BENCH (log10_simd_approx3, math_approx::log10<3>)

BENCHMARK_MAIN();
