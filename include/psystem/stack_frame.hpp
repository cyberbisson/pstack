// ===-- include/psystem/stack_frame.hpp ------------------ -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare a container for a single frame in a stack trace.
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
#ifndef PSYSTEM_STACK_FRAME_HPP
#define PSYSTEM_STACK_FRAME_HPP

#include "psystem_ns.hpp"

#include <psystem/dll/debug_help_dll.hpp>

namespace psystem {

class process_info;
class thread_info;

/**
 * @brief Contains data about a single frame in a stack trace.
 *
 * This is a simple container (data-only) object.  It is free to copy, move,
 * reassign, and will not affect any system-level debugging functionality.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
class stack_frame
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct a stack_frame instance.
     *
     * @param[in] sf Structure containing data about the stack frame to be
     *               copied into this instance.
     */
    explicit stack_frame(STACKFRAME64 const& sf) noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Access the memory location for the current frame pointer.
     *
     * @return The memory location where the frame pointer is stored for the
     *         current context.
     */
    address_t get_fp() const noexcept;

    /**
     * @brief Access the program counter location for the current stack frame.
     *
     * @return The memory location stored in the PC register for the current
     *         context.
     */
    address_t get_pc() const noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief The program counter location for the current frame pointer.
    address_t m_fp;

    /// @brief The program counter location for the current stack frame.
    address_t m_pc;
};

} // namespace psystem

#endif // PSYSTEM_STACK_FRAME_HPP
