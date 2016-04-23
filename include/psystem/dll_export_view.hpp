// ===-- include/psystem/dll_export_view.hpp -------------- -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare a class to examine DLL export data.
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

#pragma once
#ifndef PSYSTEM_DLL_EXPORT_VIEW_HPP
#define PSYSTEM_DLL_EXPORT_VIEW_HPP

#include "psystem_ns.hpp"

namespace psystem {

class dll_export_iterator;

/**
 * @brief Exposes a DLL's symbol export table.
 *
 * This class exposes the DLL export table's exported symbols.  This is a "view"
 * in that is it backed by a memory-mapped file.  All data extracted from the
 * members is actually read from the module, and not stored here.
 *
 * @author  Matt Bisson
 * @date    8 February, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
class dll_export_view
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct a view of DLL export information.
     *
     * @param[in] base_address
     *    All addresses are offsets from the module base address, given here.
     * @param[in] load_address
     *    The address at which the DLL would be loaded into memory.
     * @param[in] va_offset
     *    All Relative Virtual Addresses (RVA) must be offset by this value to
     *    be treated as a valid pointer.
     * @param[in] idx_with_name
     *    The beginning of an array of DLL export ordinals.
     * @param[in] export_rva
     *    The beginning of an array of DLL export symbols' RVAs.
     * @param[in] export_name_rva
     *    The beginning of an array of DLL export symbol names' RVAs.  Once the
     *    RVA is adjusted, the value can be dereferenced as a character array.
     * @param[in] number_of_functions
     *    The total number of exported functions in the DLL.
     * @param[in] number_of_names
     *    The total number of named exported functions in the DLL.  This value
     *    is less than or equal to @p number_of_functions.
     *
     * @todo Might be nicer if only something with a memory-mapped file could
     *       create an instance.
     */
    dll_export_view(
        uint8_t const *base_address,
        uint8_t const *load_address,
        integral_address_t va_offset,
        uint16_t const *idx_with_name,
        uint32_t const *export_rva,
        uint32_t const *export_name_rva,
        uint32_t number_of_functions,
        uint32_t number_of_names) noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Acquire an iterator to the first exported symbol in the view.
     * @return An iterator representing the first visible DLL export.
     */
    dll_export_iterator begin() const;

    /**
     * @brief Acquire an iterator just past the final exported symbol in the
     *        view.
     * @return An iterator representing the end of the visible DLL exports.
     */
    dll_export_iterator end() const;

    /**
     * @brief The address at which this DLL expects to be loaded.
     *
     * This would be added to a DLL export address to produce an address in
     * memory where the executable code for that export resides.  In practice,
     * this is only a "preferred" load address, and the OS may rebase the DLL if
     * there are two modules that wish to use the same address space.  Note that
     * Windows system DLLs coordinate amongst themselves (at compile time) to
     * avoid rebasing.
     *
     * @return The preferred load address for the DLL.
     */
    psystem::address_t get_symbol_base_address() const;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief All addresses are offsets from the module base address, given
    ///        here.
    uint8_t const *m_base_address;

    /**
     * @brief The address at which this DLL expects to be loaded.
     *
     * This would be added to a DLL export address to produce an address in
     * memory where the executable code for that export resides.
    */
    uint8_t const *m_load_address;

    /// @brief All Relative Virtual Addresses (RVA) must be offset by this value
    ///        to be treated as a valid pointer.
    integral_address_t m_va_offset;

    /**
     * @brief The beginning of an array of DLL export ordinals.
     *
     * The ordinal may be used as an index into the @c m_export_name_rva list.
     */
    uint16_t const *m_idx_with_name;

    /// @brief The beginning of an array of DLL export symbols' RVAs.
    uint32_t const *m_export_rva;

    /**
     * @brief The beginning of an array of DLL export symbol names' RVAs.
     *
     * Once the RVA is adjusted, the value can be dereferenced as a character
     * array.
     */
    uint32_t const *m_export_name_rva;

    /**
     * @brief The total number of exported functions in the DLL.
     *
     * It is the count of elements in @c m_export_rva.
     */
    uint32_t m_number_of_functions;

    /**
     * @brief The total number of named exported functions in the DLL.
     *
     * This value is less than or equal to @p number_of_functions.  It is the
     * count of elements in @c m_export_name_rva and @c m_idx_with_name.
     */
    uint32_t m_number_of_names;
};

} // namespace psystem

#endif // PSYSTEM_DLL_EXPORT_VIEW_HPP
