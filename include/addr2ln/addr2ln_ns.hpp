// ===-- include/addr2ln/addr2ln_ns.hpp ------------------- -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief The basic header file for the Addr2Ln namespce.
 *
 * All modules that declare ::addr2ln namespace items must include this header
 * explicitly.  When external modules reference this header, they are declaring
 * a dependency on the namespace / module.
 *
 * @author  Matt Bisson
 * @date    9 July, 2008 -- 14 April, 2016
 * @since   Addr2ln 0.1
 * @version Addr2ln 0.1
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

/**
 * @dir
 * @brief This directory contains the public interface declarations for the
 *        ::addr2ln namespace.
 *
 * @author  Matt Bisson
 * @date    9 July, 2008 -- 14 April, 2016
 * @since   Addr2ln 0.1
 * @version Addr2ln 0.1
 */

#pragma once
#ifndef ADDR2LN_ADDR2LN_NS_HPP
#define ADDR2LN_ADDR2LN_NS_HPP

#include "psystem/psystem_ns.hpp"

////////////////////////////////////////////////////////////////////////////////
/// @name Application Version Macros
////////////////////////////////////////////////////////////////////////////////

/// @brief Informational name of this library.
#define ADDR2LN_APP_NAME         "Addr2ln (Address-to-source-line Printer)"

/// @brief Major version number (representing major functionality changes).
#ifndef ADDR2LN_MAJOR_VERSION
#   define ADDR2LN_MAJOR_VERSION 0
#endif

/// @brief Minor version (representing incremental bug-fixing and minor feature
///        changes).
#ifndef ADDR2LN_MINOR_VERSION
#   define ADDR2LN_MINOR_VERSION 1
#endif

/// @brief Corresponds to a quick patch level applied to a build.
#ifndef ADDR2LN_PATCH_LEVEL
#   define ADDR2LN_PATCH_LEVEL   0
#endif

/// @brief Corresponds to a nightly build number.
///
/// If this is 0, this is a "special" build.
#ifndef ADDR2LN_BUILD_NUMBER
#   define ADDR2LN_BUILD_NUMBER  0
#endif

/// @brief This is used to make a note in the version info in the executable.
#if (ADDR2LN_BUILD_NUMBER == 0)
#   define _PRIVATE_BUILD        1
#endif

/// @}

// We will inlcude this from all sources -- including the resource-file
// compiler -- so let's make sure it compiles.
#ifndef RC_INVOKED

/**
 * @brief Contains functionality required to map addresses to source code
 *        information.
 *
 * @author  Matt Bisson
 * @date    9 July, 2008 -- 14 April, 2016
 * @since   Addr2ln 0.1
 * @version Addr2ln 0.1
 */
namespace addr2ln
{

} // namespace addr2ln

#endif // RC_INVOKED

#endif // ADDR2LN_ADDR2LN_NS_HPP
