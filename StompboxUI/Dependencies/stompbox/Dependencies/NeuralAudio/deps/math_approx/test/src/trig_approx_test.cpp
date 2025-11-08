#include "test_helpers.hpp"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include <math_approx/math_approx.hpp>

TEST_CASE ("Sine Approx Test")
{
#if ! defined(WIN32)
    const auto all_floats = test_helpers::all_32_bit_floats (-10.0f, 10.0f, 1.0e-3f);
#else
    const auto all_floats = test_helpers::all_32_bit_floats (-10.0f, 10.0f, 1.0e-1f);
#endif
    const auto y_exact = test_helpers::compute_all<float> (all_floats, [] (auto x)
                                                    { return std::sin (x); });

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
                     { return math_approx::sin<9> (x); },
                     8.5e-7f);
    }
    SECTION ("7th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::sin<7> (x); },
                     1.8e-5f);
    }
    SECTION ("5th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::sin<5> (x); },
                     7.5e-4f);
    }
}

TEST_CASE ("Cosine Approx Test")
{
#if ! defined(WIN32)
    const auto all_floats = test_helpers::all_32_bit_floats (-10.0f, 10.0f, 1.0e-3f);
#else
    const auto all_floats = test_helpers::all_32_bit_floats (-10.0f, 10.0f, 1.0e-1f);
#endif
    const auto y_exact = test_helpers::compute_all<float> (all_floats, [] (auto x)
                                                    { return std::cos (x); });

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
                     { return math_approx::cos<9> (x); },
                     7.5e-7f);
    }
    SECTION ("7th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::cos<7> (x); },
                     1.8e-5f);
    }
    SECTION ("5th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::cos<5> (x); },
                     7.5e-4f);
    }
}

TEST_CASE ("Tan Approx Test")
{
#if ! defined(WIN32)
    const auto all_floats = test_helpers::all_32_bit_floats (-1.5f, 1.5f, 1.0e-3f);
#else
    const auto all_floats = test_helpers::all_32_bit_floats (-1.5f, 1.5f, 1.0e-1f);
#endif
    const auto y_exact = test_helpers::compute_all<float> (all_floats, [] (auto x)
                                                    { return std::tan (x); });

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

    SECTION ("13th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::tan<13> (x); },
                     5.5e-5f,
                     6.0e-5f,
                     520);
    }
    SECTION ("11th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::tan<11> (x); },
                     9.5e-5f,
                     6.0e-5f,
                     520);
    }
    SECTION ("9th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::tan<9> (x); },
                     0.0009f,
                     6.0e-5f,
                     900);
    }
    SECTION ("7th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::tan<7> (x); },
                     0.015f,
                     0.0009f,
                     0);
    }
    SECTION ("5th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::tan<5> (x); },
                     0.14f,
                     0.01f,
                     0);
    }
    SECTION ("3rd-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::tan<3> (x); },
                     1.5f,
                     0.09f,
                     0);
    }
}
