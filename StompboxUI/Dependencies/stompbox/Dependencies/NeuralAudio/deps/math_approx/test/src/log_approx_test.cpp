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


TEMPLATE_TEST_CASE ("Log Approx Test", "", float, double)
{
    const auto all_floats = test_helpers::all_32_bit_floats<TestType> (0.01f, 10.0f, 1.0e-3f);
    const auto y_exact = test_helpers::compute_all<TestType> (all_floats, [] (auto x)
                                                    { return std::log (x); });

    SECTION ("6th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log<6> (x); },
                     4.5e-6f);
    }
    SECTION ("6th-Order (C1-cont)")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log<6, true> (x); },
                     6.5e-6f);
    }
    SECTION ("5th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log<5> (x); },
                     1.5e-5f);
    }
    SECTION ("5th-Order (C1-cont)")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log<5, true> (x); },
                     3.5e-5f);
    }
    SECTION ("4th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log<4> (x); },
                     8.5e-5f);
    }
    SECTION ("4th-Order (C1-cont)")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log<4, true> (x); },
                     3.0e-4f);
    }
    SECTION ("3th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log<3> (x); },
                     6.5e-4f);
    }
    SECTION ("3th-Order (C1-cont)")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log<3, true> (x); },
                     4.0e-3f);
    }
}

TEMPLATE_TEST_CASE ("Log2 Approx Test", "", float, double)
{
    const auto all_floats = test_helpers::all_32_bit_floats<TestType> (0.01f, 10.0f, 1.0e-3f);
    const auto y_exact = test_helpers::compute_all<TestType> (all_floats, [] (auto x)
                                                    { return std::log2 (x); });

    SECTION ("6th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log2<6> (x); },
                     6.0e-6f);
    }
    SECTION ("6th-Order (C1-cont)")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log2<6, true> (x); },
                     8.5e-6f);
    }
    SECTION ("5th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log2<5> (x); },
                     2.0e-5f);
    }
    SECTION ("5th-Order (C1-cont)")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log2<5, true> (x); },
                     5.0e-5f);
    }
    SECTION ("4th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log2<4> (x); },
                     1.5e-4f);
    }
    SECTION ("4th-Order (C1-cont)")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log2<4, true> (x); },
                     4.5e-4f);
    }
    SECTION ("3th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log2<3> (x); },
                     9.0e-4f);
    }
    SECTION ("3th-Order (C1-cont)")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log2<3, true> (x); },
                     5.5e-3f);
    }
}

TEMPLATE_TEST_CASE ("Log10 Approx Test", "", float, double)
{
    const auto all_floats = test_helpers::all_32_bit_floats<TestType> (0.01f, 10.0f, 1.0e-3f);
    const auto y_exact = test_helpers::compute_all<TestType> (all_floats, [] (auto x)
                                                    { return std::log10 (x); });

    SECTION ("6th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log10<6> (x); },
                     2.0e-6f);
    }
    SECTION ("6th-Order (C1-cont)")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log10<6, true> (x); },
                     3.0e-6f);
    }
    SECTION ("5th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log10<5> (x); },
                     6.0e-6f);
    }
    SECTION ("5th-Order (C1-cont)")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log10<5, true> (x); },
                     1.5e-5f);
    }
    SECTION ("4th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log10<4> (x); },
                     4.0e-5f);
    }
    SECTION ("4th-Order (C1-cont)")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log10<4, true> (x); },
                     1.5e-4f);
    }
    SECTION ("3th-Order")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log10<3> (x); },
                     3.0e-4f);
    }
    SECTION ("3th-Order (C1-cont)")
    {
        test_approx<TestType> (all_floats, y_exact, [] (auto x)
                     { return math_approx::log10<3, true> (x); },
                     2.0e-3f);
    }
}
