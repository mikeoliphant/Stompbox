#include "test_helpers.hpp"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include <math_approx/math_approx.hpp>

#include "reference/polylogarithm.hpp"

TEST_CASE ("Li2 Approx Test")
{
#if ! defined(WIN32)
    const auto all_floats = test_helpers::all_32_bit_floats (-10.0f, 10.0f, 1.0e-2f);
#else
    const auto all_floats = test_helpers::all_32_bit_floats (-10.0f, 10.0f, 1.0e-1f);
#endif
    const auto y_exact = test_helpers::compute_all<float> (all_floats, [] (auto x)
                                                           { return polylogarithm::Li2 (x); });

    const auto test_approx = [&all_floats, &y_exact] (auto&& f_approx, float err_bound, float rel_err_bound, uint32_t ulp_bound)
    {
        const auto y_approx = test_helpers::compute_all<float> (all_floats, f_approx);

        const auto error = test_helpers::compute_error<float> (y_exact, y_approx);
        const auto rel_error = test_helpers::compute_rel_error<float> (y_exact, y_approx);
        const auto ulp_error = test_helpers::compute_ulp_error (y_exact, y_approx);

        const auto max_error = test_helpers::abs_max<float> (error);
        const auto max_rel_error = test_helpers::abs_max<float> (rel_error);
        const auto max_ulp_error = *std::max_element (ulp_error.begin(), ulp_error.end());

        std::cout << max_error << ", " << max_rel_error << ", " << max_ulp_error << std::endl;
        REQUIRE (std::abs (max_error) < err_bound);
        REQUIRE (std::abs (max_rel_error) < rel_err_bound);
        if (ulp_bound > 0)
            REQUIRE (max_ulp_error < ulp_bound);
    };

    SECTION ("3rd-Order_Log-6")
    {
        test_approx ([] (auto x)
                     { return math_approx::li2<3, 6> (x); },
                     2.5e-5f,
                     1.5e-5f,
                     200);
    }
    SECTION ("3rd-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::li2<3> (x); },
                     8.0e-5f,
                     1.5e-4f,
                     0);
    }
    SECTION ("2nd-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::li2<2> (x); },
                     3.0e-4f,
                     3.0e-4f,
                     0);
    }
    SECTION ("1st-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::li2<1> (x); },
                     2.5e-3f,
                     4.0e-3f,
                     0);
    }
}
