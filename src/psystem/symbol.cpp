// ===-- src/psystem/symbol.cpp --------------------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define methods for a container for an individual debugging symbol.
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

#include "psystem/symbol.hpp"

namespace psystem {

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

symbol::symbol(
    address_t const address,
    address_t const code_offset,
    tstring const& name) noexcept
    : m_address(address),
      m_code_offset(code_offset),
      m_name(name)
{

}

////////////////////////////////////////////////////////////////////////////////
// Public Interface
////////////////////////////////////////////////////////////////////////////////

address_t
symbol::get_address() const noexcept
{
    return m_address;
}

address_t
symbol::get_code_offset() const noexcept
{
    return m_code_offset;
}

psystem::tstring
symbol::get_name() const noexcept
{
    return m_name;
}

} // namespace psystem
