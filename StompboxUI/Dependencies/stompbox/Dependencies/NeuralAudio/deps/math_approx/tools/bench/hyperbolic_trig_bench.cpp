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

#define HTRIG_BENCH(name, func) \
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
HTRIG_BENCH (sinh_std, std::sinh)
HTRIG_BENCH (sinh_approx6, math_approx::sinh<6>)
HTRIG_BENCH (sinh_approx5, math_approx::sinh<5>)
HTRIG_BENCH (sinh_approx4, math_approx::sinh<4>)
HTRIG_BENCH (sinh_approx3, math_approx::sinh<3>)

HTRIG_BENCH (cosh_std, std::sinh)
HTRIG_BENCH (cosh_approx6, math_approx::cosh<6>)
HTRIG_BENCH (cosh_approx5, math_approx::cosh<5>)
HTRIG_BENCH (cosh_approx4, math_approx::cosh<4>)
HTRIG_BENCH (cosh_approx3, math_approx::cosh<3>)

HTRIG_BENCH (tanh_std, std::tanh)
HTRIG_BENCH (tanh_approx11, math_approx::tanh<11>)
HTRIG_BENCH (tanh_approx9, math_approx::tanh<9>)
HTRIG_BENCH (tanh_approx7, math_approx::tanh<7>)
HTRIG_BENCH (tanh_approx5, math_approx::tanh<5>)

#define HTRIG_SIMD_BENCH(name, func) \
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
HTRIG_SIMD_BENCH (sinh_xsimd, xsimd::tanh)
HTRIG_SIMD_BENCH (sinh_simd_approx6, math_approx::sinh<6>)
HTRIG_SIMD_BENCH (sinh_simd_approx5, math_approx::sinh<5>)
HTRIG_SIMD_BENCH (sinh_simd_approx4, math_approx::sinh<4>)
HTRIG_SIMD_BENCH (sinh_simd_approx3, math_approx::sinh<3>)

HTRIG_SIMD_BENCH (cosh_xsimd, xsimd::tanh)
HTRIG_SIMD_BENCH (cosh_simd_approx6, math_approx::cosh<6>)
HTRIG_SIMD_BENCH (cosh_simd_approx5, math_approx::cosh<5>)
HTRIG_SIMD_BENCH (cosh_simd_approx4, math_approx::cosh<4>)
HTRIG_SIMD_BENCH (cosh_simd_approx3, math_approx::cosh<3>)

HTRIG_SIMD_BENCH (tanh_xsimd, xsimd::tanh)
HTRIG_SIMD_BENCH (tanh_simd_approx11, math_approx::tanh<11>)
HTRIG_SIMD_BENCH (tanh_simd_approx9, math_approx::tanh<9>)
HTRIG_SIMD_BENCH (tanh_simd_approx7, math_approx::tanh<7>)
HTRIG_SIMD_BENCH (tanh_simd_approx5, math_approx::tanh<5>)

BENCHMARK_MAIN();
