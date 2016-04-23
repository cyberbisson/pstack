// ===-- include/psystem/framework/type_traits.hpp -------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Wraps the C++ standard header, @c type_traits, to provide a few more
 *        metafunctions.
 *
 * @author  Matt Bisson
 * @date    19 November, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#pragma once
#ifndef PSYSTEM_FRAMEWORK_TYPE_TRAITS_HPP
#define PSYSTEM_FRAMEWORK_TYPE_TRAITS_HPP

#include "platform.hpp"

#include <type_traits>

namespace psystem {

/**
 * @brief A template metafunction to determine if @p T is a function with a @c
 *        void return type.
 *
 * If @p T has a void return type, this metafunction evaluates to
 * @c std::true_type, otherwise it evaluates to @c std::false_type.
 *
 * @tparam T
 *     The type whose return type will be examined.  This results in a
 *     compilation error if @p T is not a function type.
 *
 * @author  Matt Bisson
 * @date    19 November, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
template <typename T> struct has_void_return_type;

/// @cond

/* This is part of the logic of the template metafunction.  We don't need to add
 * this to the generated documentation.
 */
template <typename... Args>
struct has_void_return_type<void(Args...)> : public std::true_type {};
template <typename R, typename... Args>
struct has_void_return_type<R(Args...)> : public std::false_type {};

#ifdef _MSC_VER
#   if 0 // Something else??
// Annoyingly, Win32 calling conventions cause these not to match the type
// specialization...
template <typename... Args>
struct has_void_return_type<void __stdcall(Args...)> : public std::true_type {};
template <typename R, typename... Args>
struct has_void_return_type<R __stdcall(Args...)> : public std::false_type {};
#   endif
#endif

/// @endcond

} // namespace psystem

#endif // PSYSTEM_FRAMEWORK_TYPE_TRAITS_HPP
