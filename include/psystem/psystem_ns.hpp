// ===-- include/psystem/psystem_ns.hpp ------------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief The basic header file for the PSystem namespce.
 *
 * All modules that declare ::psystem namespace items must include this header
 * explicitly.  When external modules reference this header, they are declaring
 * a dependency on the namespace / module.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014 -- 15 August, 2014
 * @since   PSystem 1.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

/**
 * @dir
 * @brief This directory contains the public interface declarations for the
 *        ::psystem namespace.
 *
 * @author  Matt Bisson
 * @date    25 November, 2007 -- 15 August 2014
 * @since   PSystem 1.0
 * @version PSystem 2.0
 */

#pragma once
#ifndef PSYSTEM_PSYSTEM_NS_HPP
#define PSYSTEM_PSYSTEM_NS_HPP

#include "framework/platform.hpp"

////////////////////////////////////////////////////////////////////////////////
/// @name Application Version Macros
////////////////////////////////////////////////////////////////////////////////

/// @brief Informational name of this library.
#define PSYSTEM_APP_NAME         "PSystem (Process-Examination Framework)"

/// @brief Major version number (representing major functionality changes).
#ifndef PSYSTEM_MAJOR_VERSION
#   define PSYSTEM_MAJOR_VERSION 2
#endif

/// @brief Minor version (representing incremental bug-fixing and minor feature
///        changes).
#ifndef PSYSTEM_MINOR_VERSION
#   define PSYSTEM_MINOR_VERSION 0
#endif

/// @brief Corresponds to a quick patch level applied to a build.
#ifndef PSYSTEM_PATCH_LEVEL
#   define PSYSTEM_PATCH_LEVEL   0
#endif

/// @brief Corresponds to a nightly build number.
///
/// If this is 0, this is a "special" build.
#ifndef PSYSTEM_BUILD_NUMBER
#   define PSYSTEM_BUILD_NUMBER  0
#endif

/// @brief This is used to make a note in the version info in the executable.
#if (PSYSTEM_BUILD_NUMBER == 0)
#   define _PRIVATE_BUILD        1
#endif

/// @}

// We will inlcude this from all sources -- including the resource-file
// compiler -- so let's make sure it compiles.
#ifndef RC_INVOKED

/**
 * @brief Contains the process examination "system" and basic frameworks.
 *
 * @author  Matt Bisson
 * @date    25 November, 2007
 * @since   PSystem 1.0
 * @version PSystem 2.0
 */
namespace psystem
{

/**
 * @brief Initialize any resources that the psystem functionality needs.
 *
 * This method should only be called once, before utilizing any functionality
 * from the psystem namespace.
 *
 * @throws psystem::exception::system_exception
 *     if the DbgHelp DLL could not be loaded, or if it does not contain the
 *     required exported symbols.
 * @throws psystem::exception::system_exception
 *     if the DbgHelp DLL is not a supported version.
 */
void initialize_psystem();

/// @todo Move logging into separate header

/**
 * @brief Establish a list of possible logging detail levels.
 *
 * Each increasing log level implies that all lesser detail levels are also
 * logged.
 */
enum class log_level
{
    none  = 0,                ///< @brief Log nothing.
    error = none  | (1 << 0), ///< @brief Only log errors.
    info  = error | (1 << 1), ///< @brief Information logging as well as error.
    debug = error | (1 << 2)  ///< @brief In-depth debug logging.
};

// Validate our math on all platforms...
static_assert(
    log_level::info > log_level::error,
    "More verbose logging should have higher values.");

} // namespace psystem

#endif // RC_INVOKED

#endif // PSYSTEM_PSYSTEM_NS_HPP
