#include "test_helpers.hpp"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include <math_approx/math_approx.hpp>
#include "reference/toms917.hpp"

TEST_CASE ("Wright-Omega Approx Test")
{
#if ! defined(WIN32)
    const auto all_floats = test_helpers::all_32_bit_floats (-10.0f, 30.0f, 1.0e-1f);
#else
    const auto all_floats = test_helpers::all_32_bit_floats (-10.0f, 30.0f, 5.0e-1f);
#endif
    const auto y_exact = test_helpers::compute_all<float> (all_floats, [] (auto x)
                                                    { return toms917::wrightomega (x); });

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

    SECTION ("Iter-3_Poly-3_LogExp-5")
    {
        test_approx ([] (auto x)
                     { return math_approx::wright_omega<3, 3, 5> (x); },
                     2.0e-6f,
                     1.5e-6f,
                     20);
    }
    SECTION ("Iter-3_Poly-3")
    {
        test_approx ([] (auto x)
                     { return math_approx::wright_omega<3, 3> (x); },
                     4.0e-6f,
                     4.5e-6f,
                     70);
    }
    SECTION ("Iter-2_Poly-5")
    {
        test_approx ([] (auto x)
                     { return math_approx::wright_omega<2, 5> (x); },
                     7.0e-6f,
                     1.5e-4f,
                     0);
    }
    SECTION ("Iter-2_Poly-3")
    {
        test_approx ([] (auto x)
                     { return math_approx::wright_omega<2, 3> (x); },
                     1.5e-5f,
                     2.0e-4f,
                     0);
    }
    SECTION ("Iter-2_Poly-3_LogExp-3")
    {
        test_approx ([] (auto x)
                     { return math_approx::wright_omega<2, 3, 3> (x); },
                     1.0e-4f,
                     3.0e-4f,
                     0);
    }
    SECTION ("Iter-1_Poly-5")
    {
        test_approx ([] (auto x)
                     { return math_approx::wright_omega<1, 5> (x); },
                     3.0e-3f,
                     5.1e-2f,
                     0);
    }
    SECTION ("Iter-1_Poly-3")
    {
        test_approx ([] (auto x)
                     { return math_approx::wright_omega<1, 3> (x); },
                     3.5e-3f,
                     5.5e-2f,
                     0);
    }
    SECTION ("Iter-0_Poly-5")
    {
        test_approx ([] (auto x)
                     { return math_approx::wright_omega<0, 5> (x); },
                     5.5e-2f,
                     2.0f,
                     0);
    }
    SECTION ("Iter-0_Poly-3")
    {
        test_approx ([] (auto x)
                     { return math_approx::wright_omega<0, 3> (x); },
                     6.0e-2f,
                     2.0f,
                     0);
    }
}
