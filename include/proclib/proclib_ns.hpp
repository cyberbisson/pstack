// ===-- include/proclib/proclib_ns.hpp ------------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief The basic header file for the ProcLib namespce.
 *
 * All modules that declare ::proclib namespace items must include this header
 * explicitly.  When external modules reference this header, they are declaring
 * a dependency on the namespace / module.
 *
 * @author  Matt Bisson
 * @date    25 November, 2007 -- 15 August, 2014
 * @since   ProcLib 1.0
 * @version ProcLib 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

/**
 * @dir
 * @brief This directory contains the public interface declarations for the
 *        ::proclib namespace.
 *
 * @author  Matt Bisson
 * @date    25 November, 2007 -- 15 August, 2014
 * @since   ProcLib 1.0
 * @version ProcLib 2.0
 */

#pragma once
#ifndef PROCLIB_PROCLIB_NS_HPP
#define PROCLIB_PROCLIB_NS_HPP

#include <psystem/psystem_ns.hpp>

////////////////////////////////////////////////////////////////////////////////
/// @name Application Version Macros
////////////////////////////////////////////////////////////////////////////////

/// @brief Informational name of this library.
#define PROCLIB_APP_NAME         "ProcLib (Process Debugging Library)"

/// @brief Major version number (representing major functionality changes).
#ifndef PROCLIB_MAJOR_VERSION
#   define PROCLIB_MAJOR_VERSION 2
#endif

/// @brief Minor version (representing incremental bug-fixing and minor feature
///        changes).
#ifndef PROCLIB_MINOR_VERSION
#   define PROCLIB_MINOR_VERSION 0
#endif

/// @brief Corresponds to a quick patch level applied to a build.
#ifndef PROCLIB_PATCH_LEVEL
#   define PROCLIB_PATCH_LEVEL   0
#endif

/// @brief Corresponds to a nightly build number.
///
/// If this is 0, this is a "special" build.
#ifndef PROCLIB_BUILD_NUMBER
#   define PROCLIB_BUILD_NUMBER  0
#endif

/// @brief This is used to make a note in the version info in the executable.
#if (PROCLIB_BUILD_NUMBER == 0)
#   define _PRIVATE_BUILD        1
#endif

/// @}

/**
 * @brief Contains the library for debugging running processes.
 *
 * @author  Matt Bisson
 * @date    25 November, 2007 -- 15 August, 2014
 * @since   ProcLib 1.0
 * @version ProcLib 2.0
 */
namespace proclib
{

} // namespace proclib

#endif // PROCLIB_PROCLIB_NS_HPP
