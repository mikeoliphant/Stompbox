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

#define POW_BENCH(name, func) \
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
POW_BENCH (exp_std, std::exp)
POW_BENCH (exp_approx6, math_approx::exp<6>)
POW_BENCH (exp_approx5, math_approx::exp<5>)
POW_BENCH (exp_approx4, math_approx::exp<4>)
POW_BENCH (exp_approx3, math_approx::exp<3>)

POW_BENCH (exp2_std, std::exp2)
POW_BENCH (exp2_approx6, math_approx::exp2<6>)
POW_BENCH (exp2_approx5, math_approx::exp2<5>)
POW_BENCH (exp2_approx4, math_approx::exp2<4>)
POW_BENCH (exp2_approx3, math_approx::exp2<3>)

float stdpow_exp10 (float x)
{
    return std::pow (10.0f, x);
}
POW_BENCH (exp10_std, stdpow_exp10)
POW_BENCH (exp10_approx6, math_approx::exp10<6>)
POW_BENCH (exp10_approx5, math_approx::exp10<5>)
POW_BENCH (exp10_approx4, math_approx::exp10<4>)
POW_BENCH (exp10_approx3, math_approx::exp10<3>)

#define POW_SIMD_BENCH(name, func) \
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
POW_SIMD_BENCH (exp_xsimd, xsimd::exp)
POW_SIMD_BENCH (exp_simd_approx6, math_approx::exp<6>)
POW_SIMD_BENCH (exp_simd_approx5, math_approx::exp<5>)
POW_SIMD_BENCH (exp_simd_approx4, math_approx::exp<4>)
POW_SIMD_BENCH (exp_simd_approx3, math_approx::exp<3>)

POW_SIMD_BENCH (exp2_xsimd, xsimd::exp2)
POW_SIMD_BENCH (exp2_simd_approx6, math_approx::exp2<6>)
POW_SIMD_BENCH (exp2_simd_approx5, math_approx::exp2<5>)
POW_SIMD_BENCH (exp2_simd_approx4, math_approx::exp2<4>)
POW_SIMD_BENCH (exp2_simd_approx3, math_approx::exp2<3>)

POW_SIMD_BENCH (exp10_xsimd, xsimd::exp10)
POW_SIMD_BENCH (exp10_simd_approx6, math_approx::exp10<6>)
POW_SIMD_BENCH (exp10_simd_approx5, math_approx::exp10<5>)
POW_SIMD_BENCH (exp10_simd_approx4, math_approx::exp10<4>)
POW_SIMD_BENCH (exp10_simd_approx3, math_approx::exp10<3>)

BENCHMARK_MAIN();
