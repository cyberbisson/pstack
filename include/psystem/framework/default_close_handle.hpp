// ===-- include/psystem/framework/default_close_handle.hpp -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.
/**
 * @file
 * @brief Introduces a functor to wrap @c CloseHandle() for RAII handle
 *        containers.
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
#ifndef PSYSTEM_FRAMEWORK_DEFAULT_CLOSE_HANDLE_HPP
#define PSYSTEM_FRAMEWORK_DEFAULT_CLOSE_HANDLE_HPP

#include "platform.hpp"

namespace psystem {

/**
 * @brief A simple functor to close handles (for use with RAII handle
 *        containers).
 *
 * This class is marked @c final because the interfaces that use it make
 * optimizations that become impossible if the class can be derived.  For
 * example, an @c unique_handle may specify this as a deleter in the template
 * parameter, which would case the class to not store a deleter at runtime, but
 * if default_close_handle were derivable, the caller could (rightfully) pass an
 * instance of the derived class, which would be ignored.
 *
 * @tparam T The handle type.
 *
 * @see psystem::shared_handle
 * @see psystem::unique_handle
 * @todo Examine the return value from CloseHandle().
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
template <typename T>
struct default_close_handle final
{
    /**
     * @brief Executes a handle clean-up routine.
     * @param[in] hndl The handle to clean up.
     */
    void operator()(T hndl) const noexcept
    {
        default_close_handle<T>::close_handle(hndl);
    }

    /**
     * @brief Executes a handle clean-up routine without requiring an instance.
     * @param[in] hndl The handle to clean up.
     */
    static void close_handle(T hndl) noexcept
    {
        /// @todo Log if the result is not a success.
        ::CloseHandle(hndl);
    }
};

} // namespace psystem

#endif // PSYSTEM_FRAMEWORK_DEFAULT_CLOSE_HANDLE_HPP
