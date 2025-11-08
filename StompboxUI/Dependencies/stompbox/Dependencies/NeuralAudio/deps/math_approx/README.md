# math_approx

[![Test](https://github.com/Chowdhury-DSP/math_approx/actions/workflows/run_tests.yml/badge.svg)](https://github.com/Chowdhury-DSP/math_approx/actions/workflows/run_tests.yml)


`math_approx` is a C++ library for math approximations.

Currently supported:

- sin/cos/tan
- arcsin/arccos/arctan
- exp/exp2/exp10/expm1
- log/log2/log10/log1p
- sinh/cosh/tanh
- arcsinh/arccosh/arctanh
- [Sigmoid function](https://en.wikipedia.org/wiki/Sigmoid_function)
- [Wright-Omega function](https://en.wikipedia.org/wiki/Wright_omega_function)
- [Dilogarithm function](https://en.wikipedia.org/wiki/Dilogarithm)

At the moment, most of these implementations have been "good enough"
for my own use cases (both in terms of performance and accuracy). That
said, I definitely believe that it's possible to achieve better results
for many of these functions. If you have ideas for improving these
approximations, either by:
- Modifying an approximation to achieve better accuracy with the same (or similar) performance
- Modifying an approximation to achieve better performance with the same (or similar) accuracy

then please get in touch with a GitHub issue or pull request!

## Usage

### With CMake

`math_approx` is set up as a CMake `INTERFACE` library. To use it as
such, you'll need to add the following to your `CMakeLists.txt` file:

```cmake
add_subdirectory(math_approx)
target_link_libraries(<your_target> math_approx)
```

And then in your C++ code, you can use the approximations like so:

```cpp
#include <math_approx/math_approx.hpp>

constexpr auto sin_half = math_approx::sin<5> (0.5f);
```

### Without CMake

To use `math_approx` without CMake, you'll need to add
`/path/to/repo/include` to your include path. If you're
compiling your program with MSVC, you may also need to
add the pre-processor definition `_USE_MATH_DEFINES`.

## A few other thoughts

### Accuracy vs. Performance

Most of the methods in this library are provided with template
arguments which control the "order" of the approximation. The
"order" typically refers to the order of a polynomial used in
the approximation. In general, higher-order approximations will
achieve greater accuracy, while taking longer to compute.

### Showing My Work

Since the approximations in this library are primarily based on
polynomial approximations, I've tried to provide the details
for how those polynomials were derived, by providing a zipped
folder containing the Mathematica notebooks that were used to
derive the polynomials. Since not everyone has access to
Mathematica, the folder also contains a PDF version of each
notebook. At the moment, I'm planning to upload an updated
copy of the zipped folder with each release of the library,
but if I can think of a better method of distribution, that
doesn't involve adding the notebook files to the repository
directly, I'll do that instead.

### Measuring Accuracy

This library uses three approaches for measuring accuracy:
- Absolute error (`Error = |actual - approx|`)
- Relative error (`Error = |(actual - approx) / actual|`)
- [ULP Distance](https://en.wikipedia.org/wiki/Unit_in_the_last_place#:~:text=In%20computer%20science%20and%20numerical,of%20accuracy%20in%20numeric%20calculations.)

### Single vs. Double Precision

At the moment, the approximations in this library have been
primarily tested with single-precision floating-point numbers.
It is possible (maybe even likely) that most of the approximations
do not achieve sufficient accuracy for double-precision computations.

### C++ Standard

The library has been mostly developed and tested with C++20, with
a little bit of effort to provide compatibility with C++17.
Personally, I would rather not extend support to C++14 or earlier.

### SIMD

These approximations are intended to work for both scalar floating-point
data types, as well as SIMD floating-point data types. At the moment,
the library is set up to be compatible with the [XSIMD library](https://github.com/xtensor-stack/xsimd).
That said, I would like to make it as easy as possible to use this
library with other SIMD libraries (or matrix math libraries), so if
anyone has some suggestions, please let me know!

### Constexpr

The majority of the approximations in this library are implemented
so as to be constexpr-compatible. That said, there are some
approximations that are only constexpr if the compiler supports
`std::bit_cast` (typically C++20 and later), and some that cannot
be made constexpr because they depend on `std::sqrt`. If someone
knows of any portable constexpr-compatible implementations of these
methods, I would be happy to add them to the library!

## License

`math_approx` is open source, and is licensed under the
BSD 3-clause license.

Enjoy!
