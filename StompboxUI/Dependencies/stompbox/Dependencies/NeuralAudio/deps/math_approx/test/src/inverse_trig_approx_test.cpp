#include "test_helpers.hpp"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include <math_approx/math_approx.hpp>

TEST_CASE ("Asin Approx Test")
{
#if ! defined(WIN32)
    const auto all_floats = test_helpers::all_32_bit_floats (-1.0f, 1.0f, 1.0e-2f);
#else
    const auto all_floats = test_helpers::all_32_bit_floats (-1.0f, 1.0f, 1.0e-1f);
#endif
    const auto y_exact = test_helpers::compute_all<float> (all_floats, [] (auto x)
                                                    { return std::asin (x); });

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

    SECTION ("4th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::asin<4> (x); },
                     2.5e-7f,
                     4.0e-7f,
                     4);
    }
    SECTION ("3rd-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::asin<3> (x); },
                     3.0e-7f,
                     5.0e-7f,
                     5);
    }
    SECTION ("2nd-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::asin<2> (x); },
                     2.0e-6f,
                     4.0e-6f,
                     50);
    }
    SECTION ("1st-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::asin<1> (x); },
                     4.0e-5f,
                     6.5e-5f,
                     0);
    }
}

TEST_CASE ("Acos Approx Test")
{
#if ! defined(WIN32)
    const auto all_floats = test_helpers::all_32_bit_floats (-1.0f, 1.0f, 1.0e-2f);
#else
    const auto all_floats = test_helpers::all_32_bit_floats (-1.0f, 1.0f, 1.0e-1f);
#endif
    const auto y_exact = test_helpers::compute_all<float> (all_floats, [] (auto x)
                                                    { return std::acos (x); });

    const auto test_approx = [&all_floats, &y_exact] (auto&& f_approx, float err_bound)
    {
        const auto y_approx = test_helpers::compute_all<float> (all_floats, f_approx);

        const auto error = test_helpers::compute_error<float> (y_exact, y_approx);

        const auto max_error = test_helpers::abs_max<float> (error);

        std::cout << max_error << std::endl;
        REQUIRE (std::abs (max_error) < err_bound);
    };

    SECTION ("5th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::acos<5> (x); },
                     5.0e-7f);
    }
    SECTION ("4th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::acos<4> (x); },
                     1.0e-6f);
    }
    SECTION ("3rd-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::acos<3> (x); },
                     1.5e-5f);
    }
    SECTION ("2nd-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::acos<2> (x); },
                     2.5e-4f);
    }
    SECTION ("1st-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::acos<1> (x); },
                     5.0e-3f);
    }
}

TEST_CASE ("Atan Approx Test")
{
#if ! defined(WIN32)
    const auto all_floats = test_helpers::all_32_bit_floats (-10.0f, 10.0f, 1.0e-2f);
#else
    const auto all_floats = test_helpers::all_32_bit_floats (-10.0f, 10.0f, 1.0e-1f);
#endif
    const auto y_exact = test_helpers::compute_all<float> (all_floats, [] (auto x)
                                                    { return std::atan (x); });

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

    SECTION ("7th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::atan<7> (x); },
                     4.0e-7f,
                     3.0e-6f,
                     45);
    }
    SECTION ("5th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::atan<5> (x); },
                     2.0e-5f,
                     1.5e-4f,
                     0);
    }
    SECTION ("4th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::atan<4> (x); },
                     1.5e-4f,
                     8.5e-4f,
                     0);
    }
}
