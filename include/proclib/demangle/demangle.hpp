// ===-- include/proclib/demangle/demangle.hpp ------------ -*- C++ -*- --=== //
// Copyright (c) 2016 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declares methods for C++ name de-mangling.
 *
 * @author  Matt Bisson
 * @date    25 November, 2007 -- 15 April, 2016
 * @since   ProcLib 1.0
 * @version ProcLib 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#pragma once
#ifndef PROCLIB_DEMANGLE_DEMANGLE_HPP
#define PROCLIB_DEMANGLE_DEMANGLE_HPP

#include "demangle_ns.hpp"

namespace proclib {
namespace demangle {

/**
 * @brief De-mangle GCC-generated C++ symbols.
 *
 * @param[in] name The name to de-mangle (must not be @c nullptr).
 * @return The human-readable name.  If there was an error, this returns an
 *         empty string.
 */
std::string demangle_gcc(char const *name) noexcept;

} // namespace demangle
} // namespace proclib

#endif // PROCLIB_DEMANGLE_DEMANGLE_NS_HPP
