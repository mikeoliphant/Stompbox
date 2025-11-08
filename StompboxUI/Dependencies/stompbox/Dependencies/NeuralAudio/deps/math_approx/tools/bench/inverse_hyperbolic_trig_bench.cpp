#include <math_approx/math_approx.hpp>
#include <benchmark/benchmark.h>

static constexpr size_t N = 2000;
const auto data_asinh = []
{
    std::vector<float> x;
    x.resize (N, 0.0f);
    for (size_t i = 0; i < N; ++i)
        x[i] = -10.0f + 20.0f * (float) i / (float) N;
    return x;
}();

const auto data_acosh = []
{
    std::vector<float> x;
    x.resize (N, 0.0f);
    for (size_t i = 0; i < N; ++i)
        x[i] = 1.0f + 9.0f * (float) i / (float) N;
    return x;
}();

const auto data_atanh = []
{
    std::vector<float> x;
    x.resize (N, 0.0f);
    for (size_t i = 0; i < N; ++i)
        x[i] = -1.0f + 2.0f * (float) i / (float) N;
    return x;
}();

#define INV_HTRIG_BENCH(name, func, data) \
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

INV_HTRIG_BENCH (asinh_std, std::asinh, data_asinh)
INV_HTRIG_BENCH (asinh_approx7, math_approx::asinh<7>, data_asinh)
INV_HTRIG_BENCH (asinh_approx6, math_approx::asinh<6>, data_asinh)
INV_HTRIG_BENCH (asinh_approx5, math_approx::asinh<5>, data_asinh)
INV_HTRIG_BENCH (asinh_approx4, math_approx::asinh<4>, data_asinh)
INV_HTRIG_BENCH (asinh_approx3, math_approx::asinh<3>, data_asinh)

INV_HTRIG_BENCH (acosh_std, std::acosh, data_acosh)
INV_HTRIG_BENCH (acosh_approx6, math_approx::acosh<6>, data_acosh)
INV_HTRIG_BENCH (acosh_approx5, math_approx::acosh<5>, data_acosh)
INV_HTRIG_BENCH (acosh_approx4, math_approx::acosh<4>, data_acosh)
INV_HTRIG_BENCH (acosh_approx3, math_approx::acosh<3>, data_acosh)

INV_HTRIG_BENCH (atanh_std, std::atanh, data_atanh)
INV_HTRIG_BENCH (atanh_approx6, math_approx::atanh<6>, data_atanh)
INV_HTRIG_BENCH (atanh_approx5, math_approx::atanh<5>, data_atanh)
INV_HTRIG_BENCH (atanh_approx4, math_approx::atanh<4>, data_atanh)
INV_HTRIG_BENCH (atanh_approx3, math_approx::atanh<3>, data_atanh)

#define INV_HTRIG_SIMD_BENCH(name, func, data) \
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

INV_HTRIG_SIMD_BENCH (asinh_xsimd, xsimd::asinh, data_asinh)
INV_HTRIG_SIMD_BENCH (asinh_simd_approx7, math_approx::asinh<7>, data_asinh)
INV_HTRIG_SIMD_BENCH (asinh_simd_approx6, math_approx::asinh<6>, data_asinh)
INV_HTRIG_SIMD_BENCH (asinh_simd_approx5, math_approx::asinh<5>, data_asinh)
INV_HTRIG_SIMD_BENCH (asinh_simd_approx4, math_approx::asinh<4>, data_asinh)
INV_HTRIG_SIMD_BENCH (asinh_simd_approx3, math_approx::asinh<3>, data_asinh)

INV_HTRIG_SIMD_BENCH (acosh_xsimd, xsimd::acosh, data_acosh)
INV_HTRIG_SIMD_BENCH (acosh_simd_approx6, math_approx::acosh<6>, data_acosh)
INV_HTRIG_SIMD_BENCH (acosh_simd_approx5, math_approx::acosh<5>, data_acosh)
INV_HTRIG_SIMD_BENCH (acosh_simd_approx4, math_approx::acosh<4>, data_acosh)
INV_HTRIG_SIMD_BENCH (acosh_simd_approx3, math_approx::acosh<3>, data_acosh)

INV_HTRIG_SIMD_BENCH (atanh_xsimd, xsimd::atanh, data_atanh)
INV_HTRIG_SIMD_BENCH (atanh_simd_approx6, math_approx::atanh<6>, data_atanh)
INV_HTRIG_SIMD_BENCH (atanh_simd_approx5, math_approx::atanh<5>, data_atanh)
INV_HTRIG_SIMD_BENCH (atanh_simd_approx4, math_approx::atanh<4>, data_atanh)
INV_HTRIG_SIMD_BENCH (atanh_simd_approx3, math_approx::atanh<3>, data_atanh)

BENCHMARK_MAIN();
