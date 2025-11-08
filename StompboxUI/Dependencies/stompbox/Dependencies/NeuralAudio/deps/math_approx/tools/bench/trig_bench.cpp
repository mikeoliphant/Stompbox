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

#define TRIG_BENCH(name, func) \
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

TRIG_BENCH (cos_std, std::cos)
TRIG_BENCH (cos_approx9, math_approx::cos<9>)
TRIG_BENCH (cos_approx7, math_approx::cos<7>)
TRIG_BENCH (cos_approx5, math_approx::cos<5>)

TRIG_BENCH (sin_std, std::sin)
TRIG_BENCH (sin_approx9, math_approx::sin<9>)
TRIG_BENCH (sin_approx7, math_approx::sin<7>)
TRIG_BENCH (sin_approx5, math_approx::sin<5>)

TRIG_BENCH (tan_std, std::tan)
TRIG_BENCH (tan_approx13, math_approx::tan<13>)
TRIG_BENCH (tan_approx11, math_approx::tan<11>)
TRIG_BENCH (tan_approx9, math_approx::tan<9>)
TRIG_BENCH (tan_approx7, math_approx::tan<7>)
TRIG_BENCH (tan_approx5, math_approx::tan<5>)
TRIG_BENCH (tan_approx3, math_approx::tan<3>)

#define TRIG_SIMD_BENCH(name, func) \
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

TRIG_SIMD_BENCH (sin_xsimd, xsimd::sin)
TRIG_SIMD_BENCH (sin_simd_approx9, math_approx::sin<9>)
TRIG_SIMD_BENCH (sin_simd_approx7, math_approx::sin<7>)
TRIG_SIMD_BENCH (sin_simd_approx5, math_approx::sin<5>)

TRIG_SIMD_BENCH (cos_xsimd, xsimd::cos)
TRIG_SIMD_BENCH (cos_simd_approx9, math_approx::cos<9>)
TRIG_SIMD_BENCH (cos_simd_approx7, math_approx::cos<7>)
TRIG_SIMD_BENCH (cos_simd_approx5, math_approx::cos<5>)

TRIG_SIMD_BENCH (tan_xsimd, xsimd::tan)
TRIG_SIMD_BENCH (tan_simd_approx13, math_approx::tan<13>)
TRIG_SIMD_BENCH (tan_simd_approx11, math_approx::tan<11>)
TRIG_SIMD_BENCH (tan_simd_approx9, math_approx::tan<9>)
TRIG_SIMD_BENCH (tan_simd_approx7, math_approx::tan<7>)
TRIG_SIMD_BENCH (tan_simd_approx5, math_approx::tan<5>)
TRIG_SIMD_BENCH (tan_simd_approx3, math_approx::tan<3>)

BENCHMARK_MAIN();
