// ===-- include/psystem/framework/not_copyable.hpp ------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Defines the psystem::not_assignable interface.
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
#ifndef PSYSTEM_FRAMEWORK_NOT_COPYABLE_HPP
#define PSYSTEM_FRAMEWORK_NOT_COPYABLE_HPP

#include "platform.hpp"

namespace psystem {

/**
 * @brief Remove the ability to copy or reassign from derived classes.
 *
 * When a class should disallow both copying and reassigning, it should derive
 * from psystem::not_copyable (to disallow only reassignment, see
 * psystem::not_assignable).  This mechanism provides a simple and readable way
 * to avoid boiler-plate method deletion, and to signal intent in the class
 * declaration.  This has no run-time overhead.
 *
 * @note This can be cirucumvented, by overloading operator=() or providing a
 *       copy constructor again.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
class not_copyable
{
protected:
    /// @brief Delete the copy constructor.
    not_copyable(not_copyable const&) = delete;

    /// @brief Delete the assignment operator.
    not_copyable& operator=(not_copyable const&) = delete;

    /// @brief A simple CTOR suitable for noexcept and constexpr uses.
    constexpr not_copyable() noexcept { }

    /// @brief A simple CTOR suitable for noexcept uses.
    ~not_copyable() noexcept { }
};

} // namespace psystem

#endif // PSYSTEM_FRAMEWORK_NOT_COPYABLE_HPP
