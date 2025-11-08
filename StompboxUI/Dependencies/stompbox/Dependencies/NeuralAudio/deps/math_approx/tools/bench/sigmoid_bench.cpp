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

#define SIGMOID_BENCH(name, func) \
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
SIGMOID_BENCH (sigmoid_std, [] (auto x) { return 1.0f / (1.0f + std::exp (-x)); })
SIGMOID_BENCH (sigmoid_approx9, math_approx::sigmoid<9>)
SIGMOID_BENCH (sigmoid_approx7, math_approx::sigmoid<7>)
SIGMOID_BENCH (sigmoid_approx5, math_approx::sigmoid<5>)
SIGMOID_BENCH (sigmoid_exp_approx6, math_approx::sigmoid_exp<6>)
SIGMOID_BENCH (sigmoid_exp_approx5, math_approx::sigmoid_exp<5>)
SIGMOID_BENCH (sigmoid_exp_approx4, math_approx::sigmoid_exp<4>)

#define SIGMOID_SIMD_BENCH(name, func) \
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
SIGMOID_SIMD_BENCH (sigmoid_xsimd, [] (auto x) { return 1.0f / (1.0f + xsimd::exp (-x)); })
SIGMOID_SIMD_BENCH (sigmoid_simd_approx9, math_approx::tanh<9>)
SIGMOID_SIMD_BENCH (sigmoid_simd_approx7, math_approx::tanh<7>)
SIGMOID_SIMD_BENCH (sigmoid_simd_approx5, math_approx::tanh<5>)
SIGMOID_SIMD_BENCH (sigmoid_exp_simd_approx6, math_approx::sigmoid_exp<6>)
SIGMOID_SIMD_BENCH (sigmoid_exp_simd_approx5, math_approx::sigmoid_exp<5>)
SIGMOID_SIMD_BENCH (sigmoid_exp_simd_approx4, math_approx::sigmoid_exp<4>)

BENCHMARK_MAIN();
