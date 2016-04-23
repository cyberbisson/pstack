// ===-- src/psystem/dll_export_view.cpp ------------------ -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define methods to examine DLL export data from a memory-mapped module.
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

#include "psystem/dll_export_view.hpp"

#include <psystem/dll_export_iterator.hpp>

namespace psystem {

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

dll_export_view::dll_export_view(
    uint8_t const *base_address,
    uint8_t const *load_address,
    integral_address_t const va_offset,
    uint16_t const *idx_with_name,
    uint32_t const *export_rva,
    uint32_t const *export_name_rva,
    uint32_t number_of_functions,
    uint32_t number_of_names) noexcept
    : m_base_address(base_address),
      m_load_address(load_address),
      m_va_offset(va_offset),
      m_idx_with_name(idx_with_name),
      m_export_rva(export_rva),
      m_export_name_rva(export_name_rva),
      m_number_of_functions(number_of_functions),
      m_number_of_names(number_of_names)
{
    /// @todo A lot of this should be psystem::address_t types instead of *
}

////////////////////////////////////////////////////////////////////////////////
// Public Interface
////////////////////////////////////////////////////////////////////////////////

dll_export_iterator
dll_export_view::begin() const
{
    return dll_export_iterator(
        m_base_address,
        m_va_offset,
        0,
        m_export_rva,
        m_idx_with_name,
        m_export_name_rva);
}

dll_export_iterator
dll_export_view::end() const
{
    return dll_export_iterator(
        m_base_address,
        m_va_offset,
        m_number_of_functions,
        m_export_rva + m_number_of_functions,
        m_idx_with_name + m_number_of_names,
        m_export_name_rva + m_number_of_names);
}

psystem::address_t
dll_export_view::get_symbol_base_address() const
{
    return reinterpret_cast<psystem::address_t>(m_load_address);
}

} // namespace psystem
