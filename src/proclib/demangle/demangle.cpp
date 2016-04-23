// ===-- src/psystem/demangle/demangle.cpp ---------------- -*- C++ -*- --=== //
// Copyright (c) 2016 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Defines methods for C++ name de-mangling.
 *
 * @todo Replace this with a custom implementation.  This version exists to save
 *       time, and is so old that it predates anything from C++11 forward.
 */

#include "proclib/demangle/demangle.hpp"

#ifdef __GNUC__
#   include <cxxabi.h>
#endif // __GNUC__

namespace proclib {
namespace demangle {

/// @cond
#ifdef __GNUC__
using namespace abi;
#else // !__GNUC__
char *__cxa_demangle (const char *, char *, size_t *, int *);
#endif // __GNUC__
/// @endcond

std::string
demangle_gcc(char const *const name) noexcept
{
    ASSERT(name);

    std::string result;

    char   output[512];
    size_t output_size = sizeof(output);
    int    error       = 0;
    __cxa_demangle(name, output, &output_size, &error);

    if (0 != error)
    {
        return result;
    }

    result = output;
    return result;
}

} // namespace demangle
} // namespace proclib
