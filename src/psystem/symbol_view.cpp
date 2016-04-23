// ===-- src/psystem/symbol_view.cpp ---------------------- -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define methods to examine an executable images's debugging symbols.
 *
 * @author  Matt Bisson
 * @date    8 February, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#include "psystem/symbol_view.hpp"

#include <psystem/image_symbol_iterator.hpp>

namespace psystem {

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

symbol_view::symbol_view(
    psystem::address_t const  symbol_start,
    char const               *string_start,
    psystem::address_t const  module_base) noexcept
    : m_symbol_start(symbol_start),
      m_string_start(string_start),
      m_module_base(module_base)
{
    ASSERT(
        reinterpret_cast<psystem::address_t>(m_string_start) >= symbol_start);
}

////////////////////////////////////////////////////////////////////////////////
// Public Interface
////////////////////////////////////////////////////////////////////////////////

image_symbol_iterator
symbol_view::begin() const
{
    return image_symbol_iterator(m_symbol_start);
}

image_symbol_iterator
symbol_view::end() const
{
    return image_symbol_iterator(
        // The start of the string table is the end of the symbol table...
        reinterpret_cast<psystem::address_t>(m_string_start));
}

} // namespace psystem
