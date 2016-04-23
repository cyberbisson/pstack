// ===-- include/psystem/framework/not_assignable.hpp ----- -*- C++ -*- --=== //
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
#ifndef PSYSTEM_FRAMEWORK_NOT_ASSIGNABLE_HPP
#define PSYSTEM_FRAMEWORK_NOT_ASSIGNABLE_HPP

#include "platform.hpp"

namespace psystem {

/**
 * @brief Removes the ability to use an assignment operation on derived classes.
 *
 * In some cases, it is not desirable to be able to assign onto an existing
 * instance of a class while still allowing the ability to construct copies (see
 * psystem::not_copyable to prevent both).  An "immutable" class is one such
 * instance.  Classes may declare themselves with psystem::not_assignable as
 * their base to get this functionality.  This class will not add any overhead
 * at run-time.
 *
 * @note This can be cirucumvented, of course, by simply overloading operator=().
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
class not_assignable
{
protected:
    /// @brief Delete the basic assignment operator.
    not_assignable& operator=(not_assignable const&) const = delete;

    /// @brief A simple CTOR suitable for noexcept and constexpr uses.
    constexpr not_assignable() noexcept { }

    /// @brief A simple CTOR suitable for noexcept uses.
    ~not_assignable() noexcept { }
};

} // namespace psystem

#endif // PSYSTEM_FRAMEWORK_NOT_ASSIGNABLE_HPP
