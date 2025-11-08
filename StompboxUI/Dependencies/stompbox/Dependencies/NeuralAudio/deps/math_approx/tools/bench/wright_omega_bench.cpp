#include <math_approx/math_approx.hpp>
#include <benchmark/benchmark.h>

#include "../../test/src/reference/toms917.hpp"

static constexpr size_t N = 2000;
const auto data = []
{
    std::vector<float> x;
    x.resize (N, 0.0f);
    for (size_t i = 0; i < N; ++i)
        x[i] = -10.0f + 40.0f * (float) i / (float) N;
    return x;
}();

#define WO_BENCH(name, func) \
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
WO_BENCH (wright_omega_toms917, toms917::wrightomega)
WO_BENCH (wright_omega_iter3_poly3_logexp5, (math_approx::wright_omega<3, 3, 5>))
WO_BENCH (wright_omega_iter3_poly3, (math_approx::wright_omega<3, 3>))
WO_BENCH (wright_omega_iter2_poly5, (math_approx::wright_omega<2, 5>))
WO_BENCH (wright_omega_iter2_poly3, (math_approx::wright_omega<2, 3>))
WO_BENCH (wright_omega_iter2_poly3_logexp3, (math_approx::wright_omega<2, 3, 3>))
WO_BENCH (wright_omega_iter1_poly5, (math_approx::wright_omega<1, 5>))
WO_BENCH (wright_omega_iter1_poly3, (math_approx::wright_omega<1, 3>))
WO_BENCH (wright_omega_iter0_poly5, (math_approx::wright_omega<0, 5>))
WO_BENCH (wright_omega_iter0_poly3, (math_approx::wright_omega<0, 3>))
WO_BENCH (wright_omega_dangelo2, (math_approx::wright_omega_dangelo<2>))
WO_BENCH (wright_omega_dangelo1, (math_approx::wright_omega_dangelo<1>))
WO_BENCH (wright_omega_dangelo0, (math_approx::wright_omega_dangelo<0>))

#define WO_SIMD_BENCH(name, func) \
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
WO_SIMD_BENCH (wright_omega_simd_iter3_poly3_logexp5, (math_approx::wright_omega<3, 3, 5>))
WO_SIMD_BENCH (wright_omega_simd_iter3_poly3, (math_approx::wright_omega<3, 3>))
WO_SIMD_BENCH (wright_omega_simd_iter2_poly5, (math_approx::wright_omega<2, 5>))
WO_SIMD_BENCH (wright_omega_simd_iter2_poly3, (math_approx::wright_omega<2, 3>))
WO_SIMD_BENCH (wright_omega_simd_iter2_poly3_logexp3, (math_approx::wright_omega<2, 3, 3>))
WO_SIMD_BENCH (wright_omega_simd_iter1_poly5, (math_approx::wright_omega<1, 5>))
WO_SIMD_BENCH (wright_omega_simd_iter1_poly3, (math_approx::wright_omega<1, 3>))
WO_SIMD_BENCH (wright_omega_simd_iter0_poly5, (math_approx::wright_omega<0, 5>))
WO_SIMD_BENCH (wright_omega_simd_iter0_poly3, (math_approx::wright_omega<0, 3>))

BENCHMARK_MAIN();
