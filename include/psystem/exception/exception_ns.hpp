// ===-- include/psystem/exception/exception_ns.hpp ------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declarations required for the psystem::exception namespace.
 *
 * This file should be explicitly included by every header that defines
 * components in the psystem::exception module.  When external modules reference
 * this header, they are declaring a dependency on the namespace / module.
 *
 * @author  Matt Bisson
 * @date    25 November, 2007 -- 5 August, 2014
 * @since   PSystem 1.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

/**
 * @dir
 * @brief This directory contains public interface declarations for the
 *        psystem::exception namespace.
 *
 * @author  Matt Bisson
 * @date    25 November, 2007 -- 5 August, 2014
 * @since   PSystem 1.0
 * @version PSystem 2.0
 */

#pragma once
#ifndef PSYSTEM_EXCEPTION_EXCEPTION_NS_HPP
#define PSYSTEM_EXCEPTION_EXCEPTION_NS_HPP

#include <psystem/framework/platform.hpp>

namespace psystem
{

/**
 * @brief All exceptions used by the psystem namespace exist in this namespace.
 *
 * @author  Matt Bisson
 * @date    25 November, 2007 -- 5 August, 2014
 * @since   PSystem 1.0
 * @version PSystem 2.0
 */
namespace exception
{

} // namespace exception

} // namespace psystem

#endif // PSYSTEM_EXCEPTION_EXCEPTION_NS_HPP
