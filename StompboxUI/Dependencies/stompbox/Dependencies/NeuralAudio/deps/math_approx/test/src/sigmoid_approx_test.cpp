#include "test_helpers.hpp"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include <math_approx/math_approx.hpp>

TEST_CASE ("Sigmoid Approx Test")
{
#if ! defined(WIN32)
    const auto all_floats = test_helpers::all_32_bit_floats (-10.0f, 10.0f, 1.0e-3f);
#else
    const auto all_floats = test_helpers::all_32_bit_floats (-10.0f, 10.0f, 1.0e-1f);
#endif
    const auto y_exact = test_helpers::compute_all<float> (all_floats, [] (auto x)
                                                           { return 1.0f / (1.0f + std::exp (-x)); });

    const auto test_approx = [&all_floats, &y_exact] (auto&& f_approx, float err_bound)
    {
        const auto y_approx = test_helpers::compute_all<float> (all_floats, f_approx);

        const auto error = test_helpers::compute_error<float> (y_exact, y_approx);
        const auto max_error = test_helpers::abs_max<float> (error);

        std::cout << max_error << std::endl;
        REQUIRE (std::abs (max_error) < err_bound);
    };

    SECTION ("9th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::sigmoid<9> (x); },
                     6.5e-7f);
    }
    SECTION ("7th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::sigmoid<7> (x); },
                     7.0e-6f);
    }
    SECTION ("5th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::sigmoid<5> (x); },
                     1.0e-4f);
    }
    SECTION ("3th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::sigmoid<3> (x); },
                     2.0e-3f);
    }
}

TEST_CASE ("Sigmoid (Exp) Approx Test")
{
#if ! defined(WIN32)
    const auto all_floats = test_helpers::all_32_bit_floats (-10.0f, 10.0f, 1.0e-3f);
#else
    const auto all_floats = test_helpers::all_32_bit_floats (-10.0f, 10.0f, 1.0e-1f);
#endif
    const auto y_exact = test_helpers::compute_all<float> (all_floats, [] (auto x)
                                                           { return 1.0f / (1.0f + std::exp (-x)); });

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

    SECTION ("6th-Order (Exp)")
    {
        test_approx ([] (auto x)
                     { return math_approx::sigmoid_exp<6> (x); },
                     1.5e-7f,
                     6.5e-7f,
                     12);
    }

    SECTION ("5th-Order (Exp)")
    {
        test_approx ([] (auto x)
                     { return math_approx::sigmoid_exp<5> (x); },
                     1.5e-7f,
                     7.5e-7f,
                     12);
    }

    SECTION ("4th-Order (Exp)")
    {
        test_approx ([] (auto x)
                     { return math_approx::sigmoid_exp<4> (x); },
                     9.5e-7f,
                     4.5e-6f,
                     65);
    }

    SECTION ("3rd-Order (Exp)")
    {
        test_approx ([] (auto x)
                     { return math_approx::sigmoid_exp<3> (x); },
                     3.0e-4f,
                     1.5e-4f,
                     0);
    }
}
