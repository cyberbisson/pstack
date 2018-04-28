// ===-- include/psystem/framework/memory.hpp ------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Wraps the C++ standard header, @c memory, to provide a uniform
 *        interface.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#pragma once
#ifndef PSYSTEM_FRAMEWORK_MEMORY_HPP
#define PSYSTEM_FRAMEWORK_MEMORY_HPP

#include "platform.hpp"
#include <memory>

// Visual C++ already has this function, despite its strange absence from C++11.
#ifndef _MSC_VER
#   if (__cplusplus <= 201103L)

/// @brief Injecting into the C++ "standard" namespace to compensate for
///        compiler differences.
namespace std {

/**
 * @brief Construct a new instance of @p T, assigning it to a
 *        @c std::unique_ptr.
 *
 * The main benefit of this method is exception safety.  Consider the following
 * code:
 *
@code{.cpp}
    void f(std::unique_ptr<T1> t1, std::unique_ptr<T2> t2);

    // Later, at the call site...
    f(std::unique_ptr<T1>(new T1), std::unique_ptr<T2>(new T2));
@endcode
 *
 * This presents a subtle opportunity for resource leaks when exceptions are
 * thrown by one of the constructors.  The C++ standard (for optimization
 * purposes) provides no requirement than any of the operations occur in any
 * particular sequence, other than that functions used for parameters must be
 * executed before the calling function.  Simply put, this means that it is
 * perfectly valid for the compiler to execute operator @c new for both
 * parameters before executing the constructors for @c std::unique_ptr.  If the
 * first constructor (@c T1) throws an exception, the memory for the second
 * parameter will be leaked.
 *
 * @tparam T     The type to contain in the @c std::unique_ptr.
 * @tparam Types The argument pack to be forwarded to the constructor for @p T.
 *
 * @param args These parameters will be forwarded directly to the constructor
 *             for @p T.
 * @return A new instance of @c std::unique_ptr<T> with a dynamically allocated,
 *         constructed instance of @p T.
 *
 * @throws ... This function may throw any exception thown by the constructor or
 *             the @c new operator that it executes.
 *
 * @note The C++14 standard defines two more types that we're not defining
 *       (because they are not used so far.  That is a @c make_unique for
 *       array-based @p T parameters, and a disabled @c make_unique for constant
 *       sized arrays.
 */
template<typename T, typename... Types>
unique_ptr<T>
make_unique(Types&&... args)
{
    return unique_ptr<T>(new T(forward<Types>(args)...));
}

/**
 * @brief Construct a dynamically allocated array of @p T, assigning it to a
 *        @c std::unique_ptr.
 *
 * @tparam T The type of elements in the array contained in the
 *           @c std::unique_ptr.
 *
 * @param[in] array_sz
 *     The number of elements in the newly constructed array.
 * @return
 *     A new instance of @c std::unique_ptr<T[]> with a dynamically allocated
 *     array of @p T.
 *
 * @throws ... This function may throw any exception thown by the constructor or
 *             the @c new operator that it executes.
 */
template<typename T>
unique_ptr<T[]>
make_unique<T[]>(size_t const array_sz)
{
    return unique_ptr<T[]>(new T[array_sz]);
}

} // namespace std

#   endif // __cplusplus <= 201103L
#endif // _MSC_VER
#endif // PSYSTEM_FRAMEWORK_MEMORY_HPP
