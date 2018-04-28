// ===-- include/psystem/dll_export_iterator.hpp ---------- -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare an iterator type for Windows Dynamic Link-Library (DLL)
 *        exports.
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
#ifndef PSYSTEM_DLL_EXPORT_ITERATOR_HPP
#define PSYSTEM_DLL_EXPORT_ITERATOR_HPP

#include "psystem_ns.hpp"

namespace psystem {

/**
 * @brief Iterator type for Windows Dynamic Link-Library (DLL) exports.
 *
 * Windows DLLs maintain their exported symbols in a "directory" of exports.  In
 * order to get names, this information must be correlated with a separate
 * location.  Because of these computations (and others), this "iterator" is not
 * a true iterator, and cannot currently expose an operator*().  There is no
 * single collection with all the required information, so such an operator
 * would have to create an instance to hold the result, which defeats the
 * purpose of having an iterator in my view (it should simply abstract access to
 * something that exists in a container, rather than creating new instances to
 * be accessed).
 *
 * @author  Matt Bisson
 * @date    8 February, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @todo Beef up this interface!  It's really just the minimal set of what I use
 *       today.
 * @todo Hide the public member data -- this is a temporary hack for time.
 * @todo Figure out how this could possibly be a "regular" iterator.
 */
class dll_export_iterator
    // : public std::iterator<
    //     std::forward_iterator_tag,
    //     std::pair<psystem::address_t, char const *>,
    //     size_t>
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct an instance of this DLL export iterator.
     *
     * @param[in] base_address
     *     Address to the memory-mapped DLL image.
     * @param[in] va_offset
     *     Offset required to translate the virtual address to a raw data
     *     pointer.
     * @param[in] index
     *     Index to the current exported symbol.
     * @param[in] export_rva
     *     Pointer to the array of addresses of the exported symbols.
     * @param[in] idx_with_name
     *     Pointer in the array of export indices (that have names).
     * @param[in] export_name_rva
     *     Pointer to the array of exported symbol names.
     */
    dll_export_iterator(
        uint8_t const *base_address,
        integral_address_t va_offset,
        uint16_t index,
        uint32_t const *export_rva,
        uint16_t const *idx_with_name,
        uint32_t const *export_name_rva) noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Basic Iterator Interface and Random Access Iterator Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Prefix increment.
     *
     * Simply advance the iterator to the next position.
     *
     * @return A reference to the current iterator.
     */
    dll_export_iterator& operator++();

    /**
     * @brief Determine the equality of two instances.
     *
     * @param[in] other The instance to compare against @c this for equivalence.
     * @return
     *     @c true if both @c this and @p other examine the same location in the
     *     same container; @c false in all other cases.
     */
    bool operator==(dll_export_iterator const& other) const;

    /**
     * @brief Determine the inequality of two instances.
     *
     * @param[in] other The instance to compare against @c this for
     *                  non-equivalence.
     * @return
     *     @c false if both @c this and @p other examine the same location in
     *     the same container; @c true in all other cases.
     */
    bool operator!=(dll_export_iterator const& other) const;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Extra Accessors
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Retrieve the name of the current exported symbol.
     * @return This method always returns the name, as un-named symbols are
     *         skipped
     */
    char const *get_export_name() const
    {
        return reinterpret_cast<char const*>(
            m_base_address + *m_export_name_rva + m_va_offset);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief Address to the memory-mapped DLL image.
    uint8_t const *m_base_address;

    /**
     * @brief Offset required to translate the virtual address to a raw data
     *        pointer.
     */
    integral_address_t m_va_offset;

public:
    /**
     * @brief Pointer in the array of export indices that have names.
     *
     * The array being pointed to by this member contains a list of indices for
     * exported DLL symbols that have function-names.  The array is sorted in
     * ascending order.
     */
    uint16_t const *m_idx_with_name;

    /// @brief Index to the current exported symbol.
    uint16_t m_current_idx;

    /**
     * @brief Pointer to the array of addresses of the exported symbols.
     *
     * The array pointed to by this member contains a list of the starting
     * address of all the exported symbols from the DLL.  Addresses are relative
     * virtual addresses (RVA) that are relative to the starting virtual address
     * of the @c .code segment.
     */
    uint32_t const *m_export_rva;

    /**
     * @brief Pointer to the array of exported symbol names.
     *
     * The array pointed to by this member contains the list of relative virtual
     * addresses that point to the names of exported symbols.  The array pointed
     * to by m_idx_with_name indicates the indices associated with the names.
     * There is not a 1:1 mapping between exported symbols and the list of
     * names, as not all symbols have names.  Addresses are relative to the
     * virtual address of the "raw data" segment of the current (export table)
     * section.
     */
    uint32_t const *m_export_name_rva;
};

} // namespace psystem

#endif // PSYSTEM_DLL_EXPORT_ITERATOR_HPP
