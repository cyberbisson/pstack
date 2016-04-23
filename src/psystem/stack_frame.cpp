// ===-- src/psystem/stack_frame.cpp ---------------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define a container class for a single frame in a stack trace.
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

#include "psystem/stack_frame.hpp"

namespace psystem {

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

stack_frame::stack_frame(STACKFRAME64 const& sf) noexcept
    : m_fp(sf.AddrFrame.Offset),
      m_pc(sf.AddrPC.Offset)
{

}

////////////////////////////////////////////////////////////////////////////////
// Public Interface
////////////////////////////////////////////////////////////////////////////////

address_t
stack_frame::get_fp() const noexcept
{
    return m_fp;
}

address_t
stack_frame::get_pc() const noexcept
{
    return m_pc;
}

} // namespace psystem
