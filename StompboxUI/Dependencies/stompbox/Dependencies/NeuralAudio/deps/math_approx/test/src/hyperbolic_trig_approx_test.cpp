#include "test_helpers.hpp"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include <math_approx/math_approx.hpp>

TEST_CASE ("Sinh Approx Test")
{
#if ! defined(WIN32)
    const auto all_floats = test_helpers::all_32_bit_floats (-3.5f, 3.5f, 1.0e-3f);
#else
    const auto all_floats = test_helpers::all_32_bit_floats (-3.5f, 3.5f, 1.0e-1f);
#endif
    const auto y_exact = test_helpers::compute_all<float> (all_floats, [] (auto x)
                                                    { return std::sinh (x); });

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

    SECTION ("6th-Order simul.")
    {
        test_approx ([] (auto x)
                     { return math_approx::sinh_cosh<6> (x).first; },
                     6.0e-6f,
                     0.00011f,
                     1050);
    }
    SECTION ("6th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::sinh<6> (x); },
                     6.0e-6f,
                     0.00011f,
                     1050);
    }
    SECTION ("5th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::sinh<5> (x); },
                     8.0e-6f,
                     0.00015f,
                     0);
    }
    SECTION ("4th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::sinh<4> (x); },
                     6.0e-5f,
                     0.00025f,
                     0);
    }
    SECTION ("3rd-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::sinh<3> (x); },
                     0.002f,
                     0.0035f,
                     0);
    }
}

TEST_CASE ("Cosh Approx Test")
{
#if ! defined(WIN32)
    const auto all_floats = test_helpers::all_32_bit_floats (-5.0f, 5.0f, 1.0e-3f);
#else
    const auto all_floats = test_helpers::all_32_bit_floats (-5.0f, 5.0f, 1.0e-1f);
#endif
    const auto y_exact = test_helpers::compute_all<float> (all_floats, [] (auto x)
                                                    { return std::cosh (x); });

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

    SECTION ("6th-Order simul.")
    {
        test_approx ([] (auto x)
                     { return math_approx::sinh_cosh<6> (x).second; },
                     2.5e-5f,
                     4.5e-7f,
                     8);
    }
    SECTION ("6th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::cosh<6> (x); },
                     2.5e-5f,
                     4.5e-7f,
                     8);
    }
    SECTION ("5th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::cosh<5> (x); },
                     3.5e-5f,
                     5.5e-7f,
                     10);
    }
    SECTION ("4th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::cosh<4> (x); },
                     0.0003f,
                     4.0e-6f,
                     60);
    }
    SECTION ("3rd-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::cosh<3> (x); },
                     0.0075f,
                     0.00015f,
                     0);
    }
}

TEST_CASE ("Tanh Approx Test")
{
#if ! defined(WIN32)
    const auto all_floats = test_helpers::all_32_bit_floats (-10.0f, 10.0f, 1.0e-2f);
#else
    const auto all_floats = test_helpers::all_32_bit_floats (-10.0f, 10.0f, 1.0e-1f);
#endif
    const auto y_exact = test_helpers::compute_all<float> (all_floats, [] (auto x)
                                                    { return std::tanh (x); });

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

    SECTION ("11th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::tanh<11> (x); },
                     2.5e-7f,
                     4.0e-7f,
                     7);
    }
    SECTION ("9th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::tanh<9> (x); },
                     1.5e-6f,
                     1.5e-6f,
                     20);
    }
    SECTION ("7th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::tanh<7> (x); },
                     1.5e-5f,
                     1.5e-5f,
                     230);
    }
    SECTION ("5th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::tanh<5> (x); },
                     2.5e-4f,
                     2.5e-4f,
                     0);
    }
    SECTION ("3th-Order")
    {
        test_approx ([] (auto x)
                     { return math_approx::tanh<3> (x); },
                     4.0e-3f,
                     4.0e-3f,
                     0);
    }
}
