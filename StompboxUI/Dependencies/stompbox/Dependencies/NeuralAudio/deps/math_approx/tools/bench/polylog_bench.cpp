#include <math_approx/math_approx.hpp>
#include <benchmark/benchmark.h>
#include "../test/src/reference/polylogarithm.hpp"

static constexpr size_t N = 2000;
const auto data = []
{
    std::vector<float> x;
    x.resize (N, 0.0f);
    for (size_t i = 0; i < N; ++i)
        x[i] = -10.0f + 20.0f * (float) i / (float) N;
    return x;
}();

#define POLYLOG_BENCH(name, func) \
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
POLYLOG_BENCH (li2_ref, polylogarithm::Li2)
POLYLOG_BENCH (li2_approx3_log6, (math_approx::li2<3,6>))
POLYLOG_BENCH (li2_approx3, math_approx::li2<3>)
POLYLOG_BENCH (li2_approx2, math_approx::li2<2>)
POLYLOG_BENCH (li2_approx1, math_approx::li2<1>)

#define POLYLOG_SIMD_BENCH(name, func) \
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
POLYLOG_SIMD_BENCH (li2_simd_approx3_log6, (math_approx::li2<3,6>))
POLYLOG_SIMD_BENCH (li2_simd_approx3, math_approx::li2<3>)
POLYLOG_SIMD_BENCH (li2_simd_approx2, math_approx::li2<2>)
POLYLOG_SIMD_BENCH (li2_simd_approx1, math_approx::li2<1>)

BENCHMARK_MAIN();
