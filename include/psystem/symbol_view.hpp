// ===-- include/psystem/symbol_view.hpp ------------------ -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare a class to examine an executable images's debugging symbols.
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
#ifndef PSYSTEM_SYMBOL_VIEW_HPP
#define PSYSTEM_SYMBOL_VIEW_HPP

#include "psystem_ns.hpp"

namespace psystem {

class image_symbol_iterator;

/**
 * @brief Exposes an executable image's debugging symbol data.
 *
 * This class exposes an executable image's debugging symbols.  This is a "view"
 * in that is it backed by a memory-mapped file.  All data extracted from the
 * members is actually read from the module, and not stored here.
 *
 * @author  Matt Bisson
 * @date    8 February, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
class symbol_view
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct the view into the debug symbol table.
     *
     * @param[in] symbol_start
     *     The address that points to the beginning of the symbol table.
     * @param[in] string_start
     *     The address that points to the beginning of the string table.  The
     *     beginning of the string table marks the end of the debug symbol table
     *     in COFF executable images.  This address has a double-purpose.
     * @param[in] module_base
     *     Offset from which all function / symbol addresses will be located.
     */
    symbol_view(
        psystem::address_t  symbol_start,
        char const         *string_start,
        psystem::address_t  module_base) noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Acquire an iterator to the first debugging symbol in the view.
     * @return An iterator representing the first visible debugging symbol.
     */
    image_symbol_iterator begin() const;

    /**
     * @brief Acquire an iterator just past the final debugging symbol in the
     *        view.
     * @return An iterator representing the end of the visible debugging
     *         symbols.
     */
    image_symbol_iterator end() const;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
public:
    /// @brief The address that points to the beginning of the symbol table.
    psystem::address_t m_symbol_start;

    /**
     * @brief The address that points to the beginning of the string table.
     *
     * The beginning of the string table marks the end of the debug symbol table
     * in COFF executable images.  This address has a double-purpose.
     */
    char const *m_string_start;

    /**
     * @brief Offset from which all function / symbol addresses will be
     *        located.
     */
    psystem::address_t m_module_base;
};

} // namespace psystem

#endif // PSYSTEM_SYMBOL_VIEW_HPP
