#include "test_helpers.hpp"
#include "catch2/catch_template_test_macros.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include <math_approx/math_approx.hpp>

template <typename T = float>
void test_approx (const auto& all_floats, const auto& y_exact, auto&& f_approx, float err_bound)
{
    const auto y_approx = test_helpers::compute_all<T> (all_floats, f_approx);
    const auto error = test_helpers::compute_error<T> (y_exact, y_approx);
    const auto max_error = test_helpers::abs_max<T> (error);

    std::cout << max_error << std::endl;
    REQUIRE (std::abs (max_error) < err_bound);
}

TEMPLATE_TEST_CASE ("Asinh Approx Test", "", float, double)
{
#if ! defined(WIN32)
    const auto all_floats = test_helpers::all_32_bit_floats<TestType> (-10.0f, 10.0f, 1.0e-2f);
#else
    const auto all_floats = test_helpers::all_32_bit_floats<TestType> (-10.0f, 10.0f, 1.0e-1f);
#endif
    const auto y_exact = test_helpers::compute_all<TestType> (all_floats, [] (auto x)
                                                    { return std::asinh (x); });

    SECTION ("6th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::asinh<6> (x); },
                     5.0e-7f);
    }
    SECTION ("5th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::asinh<5> (x); },
                     6.0e-5f);
    }
    SECTION ("4th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::asinh<4> (x); },
                     3.5e-4f);
    }
    SECTION ("3th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::asinh<3> (x); },
                     2.5e-3f);
    }
}

TEMPLATE_TEST_CASE ("Acosh Approx Test", "", float, double)
{
    const auto all_floats = test_helpers::all_32_bit_floats<TestType> (1.0f, 10.0f, 1.0e-2f);
    const auto y_exact = test_helpers::compute_all<TestType> (all_floats, [] (auto x)
                                                    { return std::acosh (x); });

    SECTION ("6th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::acosh<6> (x); },
                     4.5e-6f);
    }
    SECTION ("5th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::acosh<5> (x); },
                     1.5e-5f);
    }
    SECTION ("4th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::acosh<4> (x); },
                     8.5e-5f);
    }
    SECTION ("3th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::acosh<3> (x); },
                     6.5e-4f);
    }
}

TEMPLATE_TEST_CASE ("Atanh Approx Test", "", float, double)
{
    const auto all_floats = test_helpers::all_32_bit_floats<TestType> (-0.9999f, 0.9999f, 1.0e-2f);
    const auto y_exact = test_helpers::compute_all<TestType> (all_floats, [] (auto x)
                                                    { return std::atanh (x); });

    SECTION ("6th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::atanh<6> (x); },
                     2.5e-6f);
    }
    SECTION ("5th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::atanh<5> (x); },
                     6.5e-6f);
    }
    SECTION ("4th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::atanh<4> (x); },
                     4.5e-5f);
    }
    SECTION ("3th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::atanh<3> (x); },
                     3.5e-4f);
    }
}
