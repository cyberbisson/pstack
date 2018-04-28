// ===-- include/psystem/framework/platform.hpp ----------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Master header file for Platform-wide declarations.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 *
 * @todo Decide on a consistent approach between @c TCHAR and @c char...
 * @todo Remove "_t" from non-system types.
 */

/**
 * @dir
 * @brief This directory contains definitions upon which the entire system
 * relies.
 *
 * The idea of the framework directory is that it contains very rudimentary
 * structures to facilitate more rapid development.  This area has utilies to
 * make use of RAII for platform-specific resources that do not (easily) support
 * it.  It also supplies types and macros to abstract away differences in
 * compilers.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */

#pragma once
#ifndef PSYSTEM_FRAMEWORK_PLATFORM_HPP
#define PSYSTEM_FRAMEWORK_PLATFORM_HPP

// Ignore MSVC-specific keywords on non-MSVC compilers
#if !defined(_MSC_VER) && !defined(__CYGWIN__)
#   define __stdcall
#endif

#include <Windows.h>
#include <cstddef>
#include <cstdint>

// We need to include these for the @c ASSERT macro.  This may cause
// complications with inclusions in the release builds...
#ifdef _DEBUG
#   include <cstdlib>
#   include <iostream>
#endif

/**
 * @file
 *
 * @note
 * MSVC does not support the @c noexcept keyword in 2013.  Consequently, we
 * cannot provide defaulted CTORs and DTORs that have an exception specification
 * either as required by the C++ language (see C++ specification 8.4.2).  There
 * is a run-time benefit associated with @c noexcept (and to a lesser extent,
 * @c throw()), so we'll still try to use it.  Because of all this, we can't
 * default special member functions, either, because the compiler doesn't like
 * throws specifiers.  This is the choice we've made, and it's the best of a
 * mediocre situation.
 */
#if defined(_MSC_VER) && (_MSV_VER <= 1800)
#   define _ALLOW_KEYWORD_MACROS ///< @brief Tell MSVC to allow certain keywords
#   define constexpr const  ///< @brief Replace keyword with something useful
#   define noexcept throw() ///< @brief Replace keyword with something useful
#endif

/**
 * @def NO_PTRDIFF_TYPE
 * @brief Enables custom definition of address arithmetic types
 *        (psystem::address_t and psystem::integral_address_t).
 *
 * Note that the ptrdiff_t and uintptr_t are listed as "optional" in the C++
 * standard, so we will maintain a custom implementation that is triggered by
 * the @c NO_PTRDIFF_TYPE preprocessor macro when needed.
 */

////////////////////////////////////////////////////////////////////////////////
/// @name Macros to Aid Code Flow
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Alters branch prediction in generated code to favor a "true" result.
 * @param cond A conditional expression that will be evaluated.
 */
#if defined(__GNUC__) || defined(__INTEL_COMPILER)
#   define LIKELY(cond)   __builtin_expect(!!(cond), 1)
#else
#   define LIKELY(cond)   (cond)
#endif

/**
 * @brief Alters branch prediction in generated code to favor a @c false result.
 * @param cond A conditional expression that will be evaluated.
 */
#if defined(__GNUC__) || defined(__INTEL_COMPILER)
#   define UNLIKELY(cond) __builtin_expect(!!(cond), 0)
#else
#   define UNLIKELY(cond) (cond)
#endif

/**
 * @brief Abort the process if the condition is not satisfied.
 *
 * This macro evaluates to nothing unless @c _DEBUG is defined as a preprocessor
 * token.  If the condition evaluates to @c false, the macro prints the
 * condition as well as the source code location to @c stderr.
 *
 * @param cond The (boolean) condition that we assert is always @c true.
 */
#ifdef _DEBUG
#   define ASSERT(cond)                                 \
    {                                                   \
        if (UNLIKELY( !(cond) ))                        \
        {                                               \
            std::cerr                                   \
                << "Assertion failed (" #cond ") at "   \
                << __FILE__ << ":" << __LINE__          \
                << " (" << __FUNCTION__ << ")"          \
                << std::endl;                           \
            abort();                                    \
        }                                               \
    }
#else // !DEBUG
#   define ASSERT(cond)
#endif // DEBUG

/**
 * @brief Abort the process if the condition is not satisfied, declaring the a
 *        variable is used by the assertion.
 *
 * This macro behaves identically to the ASSERT() macro, however it marks a
 * variable as "used" to prevent compilation warnings in non-debug code.
 *
 * @param var  The name of the variable that the assertion examines.
 * @param cond The (boolean) condition that we assert is always @c true.
 */
#ifdef _DEBUG
#   define ASSERT_USING_VAR(var, cond) ASSERT(cond)
#else // !DEBUG
#   define ASSERT_USING_VAR(var, cond) ((void)(var))
#endif // DEBUG

////////////////////////////////////////////////////////////////////////////////
/// @name Resource-file Macros
////////////////////////////////////////////////////////////////////////////////

/// @brief Resource flags to indicate a "private" build of the application.
#ifdef _PRIVATE_BUILD
#   define PRERELEASE_FLAGS VS_FF_PRERELEASE
#else
#   define PRERELEASE_FLAGS 0
#endif

/// @brief Resource flags to indicate that the application is a debug version.
#ifdef _DEBUG
#   define FILE_FLAGS PRERELEASE_FLAGS | VS_FF_DEBUG
#else
#   define FILE_FLAGS PRERELEASE_FLAGS
#endif

/**
 * @brief Make a C-string from a token.
 *
 * This is required for the resource file.  It seems that some (not all) fields
 * require an extra indirection to construct the desired string.  When passing a
 * pre-processor constant to one of the @c MAKE_xxx_VERSION_ID macros, the
 * result becomes the @em names of the macros passed in instead of the version
 * numbers that those macros define.
 *
 * @param[in] id The token to form into a string.
 */
#define STRINGIZE_SINGLE_VERSION_ID(id) #id

/**
 * @brief Builds a string to represent the product version.
 *
 * @param[in] maj Major version number (expects an integer).
 * @param[in] min Minor version number (expects an integer).
 * @param[in] patch Patch level (expects an integer).
 */
#define MAKE_PRODUCT_VERSION_ID(maj,min,patch)  \
    STRINGIZE_SINGLE_VERSION_ID(maj) "."        \
    STRINGIZE_SINGLE_VERSION_ID(min) "."        \
    STRINGIZE_SINGLE_VERSION_ID(patch)

/**
 * @brief Builds a string to represent the version of a specific file.
 *
 * @param[in] maj Major version number (expects an integer).
 * @param[in] min Minor version number (expects an integer).
 * @param[in] patch Patch level (expects an integer).
 * @param[in] bld Build ID / number (expects an integer).
 */
#define MAKE_FILE_VERSION_ID(maj,min,patch,bld)         \
    STRINGIZE_SINGLE_VERSION_ID(maj)   "."              \
    STRINGIZE_SINGLE_VERSION_ID(min)   "."              \
    STRINGIZE_SINGLE_VERSION_ID(patch) " (build "       \
    STRINGIZE_SINGLE_VERSION_ID(bld)   ")"

/// @}

// We will inlcude this from all sources -- including the resource-file
// compiler -- so let's make sure it compiles.
#ifndef RC_INVOKED

namespace psystem
{
    /// @brief Container type for system error codes.
    /// @todo Use this in place of @c DWORD
    typedef DWORD error_t;

    /// @brief Container type for a memory address.
#ifndef NO_PTRDIFF_TYPE
    typedef std::uintptr_t address_t;
#else // NO_PTRDIFF_TYPE
    typedef void* address_t;
#endif

    /// @brief A memory address as an integer type (platform specific).
#ifndef NO_PTRDIFF_TYPE
    typedef std::ptrdiff_t integral_address_t;
#else // NO_PTRDIFF_TYPE
#   if defined(_M_X64) || defined(__x86_64)
    typedef uint64_t integral_address_t;
#   else
    typedef uint32_t integral_address_t;
#   endif
#endif

    // If ever we move onto a non-64 bit system, integral_address_t will have to
    // be adjusted...
    static_assert(
        sizeof(address_t) == sizeof(integral_address_t),
        "Pointer size does not match size for integral_address_t");

    /// @brief Data-type that stores a process ID (PID).
    typedef DWORD process_id_t;

    /// @brief Data-type that stores a thread ID (TID).
    typedef DWORD thread_id_t;
} // namespace psystem

#include <string>

namespace psystem
{
    /**
     * @brief A @c std::string implementation based on the @c UNICODE macro.
     *
     * The psystem framwork supports character access as both multi-byte and
     * "wide" character.  This must be selected at compile-time, by defining the
     * @c UNICODE macro.  For example, an executable compiled with UTF-16 string
     * containers must still support UTF-8, but it will convert the data before
     * doing anything with it.
     */
#ifdef UNICODE
    typedef std::wstring tstring;
#else
    typedef std::string tstring;
#endif
} // namespace psystem

#endif // RC_INVOKED

#endif // PSYSTEM_FRAMEWORK_PLATFORM_HPP
