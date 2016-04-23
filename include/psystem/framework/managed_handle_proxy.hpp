// ===-- include/psystem/framework/managed_handle_proxy.hpp -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Provide an internal class for safely altering a managed handle by
 *        address within its container.
 *
 * @author  Matt Bisson
 * @date    20 March, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#pragma once
#ifndef PSYSTEM_FRAMEWORK_MANAGED_HANDLE_PROXY_HPP
#define PSYSTEM_FRAMEWORK_MANAGED_HANDLE_PROXY_HPP

#include "platform.hpp"

#include "not_assignable.hpp"
#include "stack_allocated.hpp"

namespace psystem {

/// @brief PSystem utilities not meant for external consumption.
namespace internal {

/**
 * @brief Allows safe modification of a managed handle container when accessed
 *        by the handle's address.
 *
 * This class template works essentially by creating a temporary container for
 * the handle type.  Only when the instance is destroyed, is the change
 * synchronized back to the container instance.
 *
 * @tparam T The handle type that can be stored in the handle container.
 * @tparam C The continer type.  It should follow the interface of
 *         @c unique_handle, etc.
 *
 * @author  Matt Bisson
 * @date    20 March, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
template <typename T, typename C>
class managed_handle_proxy
    : public psystem::not_assignable,
      public psystem::stack_allocated
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct an instance for handle management.
     * @param[in] container The container managing a handle.
     */
    explicit managed_handle_proxy(C& container) noexcept
        : m_handle(container.get()),
          m_container(container)
    {
    }

    /**
     * @brief Synchronize changes back to the container on destruction.
     */
    ~managed_handle_proxy() noexcept
    {
        m_container.reset(m_handle);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Operator Overloads
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Access a pointer to the managed handle.
     * @return The (non-const) address to the managed handle.
     */
    operator T*() noexcept
    {
        return &m_handle;
    }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief A temporary handle for tracking changes while accessed as a
    ///        pointer-to-handle type.
    T m_handle;

    /// @brief The container tracking the handle.
    C& m_container;
};

} // namespace internal
} // namespace psystem

#endif // PSYSTEM_FRAMEWORK_MANAGED_HANDLE_PROXY_HPP
