// ===-- include/psystem/framework/stack_allocated.hpp ---- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.
/**
 * @file
 * @brief Defines the psystem::stack_allocated interface.
 *
 * @author  Matt Bisson
 * @date    21 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#pragma once
#ifndef PSYSTEM_FRAMEWORK_STACK_ALLOCATED_HPP
#define PSYSTEM_FRAMEWORK_STACK_ALLOCATED_HPP

#include "platform.hpp"

namespace psystem {

/**
 * @brief Disable heap allocation and deallocation.
 *
 * This is a zero-overhead base-class for deleting the various heap-based
 * allocation operators.  This way a class can declare its intent without having
 * to tediously delete all the operators.  Of course, operators can be
 * reintroduced further down the class heirarchy.
 *
 * The most obvious case for a stack_allocated object type is some kind of RAII
 * object that manages a resource, and attempts to do so solely within the a
 * single scope (e.g., @c std::lock_guard).
 *
 * @author  Matt Bisson
 * @date    21 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
struct stack_allocated
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Overloaded Operators
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Individual object deletion
    static void operator delete(void*) = delete;

    /// @brief Array of type object deletion
    static void operator delete[](void*) = delete;

    /// @brief Individual object allocation
    static void *operator new(size_t) = delete;

    /// @brief Array of type allocation
    static void *operator new[](size_t) = delete;
};

} // namespace psystem

#endif // PSYSTEM_FRAMEWORK_STACK_ALLOCATED_HPP
