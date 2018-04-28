// ===-- src/psystem/dll_export_iterator.cpp -------------- -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define functionality for iterating across exported symbols in a DLL.
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

#include "psystem/dll_export_iterator.hpp"

namespace psystem {

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

dll_export_iterator::dll_export_iterator(
    uint8_t const *const base_address,
    integral_address_t const va_offset,
    uint16_t const index,
    uint32_t const *const export_rva,
    uint16_t const *const idx_with_name,
    uint32_t const *const export_name_rva) noexcept
    : m_base_address(base_address),
      m_va_offset(va_offset),
      m_idx_with_name(idx_with_name),
      m_current_idx(index),
      m_export_rva(export_rva),
      m_export_name_rva(export_name_rva)
{
    /// @todo Add ASSERTions here.
}

////////////////////////////////////////////////////////////////////////////////
// Basic Iterator Interface
////////////////////////////////////////////////////////////////////////////////

bool
dll_export_iterator::operator==(dll_export_iterator const& other) const
{
    if (this == &other) return true;

    return m_current_idx == other.m_current_idx;
}

bool
dll_export_iterator::operator!=(dll_export_iterator const& other) const
{
    return !(*this == other);
}

////////////////////////////////////////////////////////////////////////////////
// Random Access Iterator Interface
////////////////////////////////////////////////////////////////////////////////

psystem::dll_export_iterator&
dll_export_iterator::operator++()
{
    if (*m_idx_with_name == m_current_idx)
    {
        ++m_idx_with_name;
        ++m_export_name_rva;
    }

    ++m_export_rva;
    ++m_current_idx;

    return *this;
}

} // namespace psystem
