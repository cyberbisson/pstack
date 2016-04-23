// ===-- include/psystem/symbol.hpp ----------------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare a container for an individual debugging symbol.
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
#ifndef PSYSTEM_SYMBOL_HPP
#define PSYSTEM_SYMBOL_HPP

#include "psystem_ns.hpp"

namespace psystem {

/**
 * @brief A container for data about a single debugger symbol.
 *
 * Debugger symbols have an address and hopefully a name.  After this, depending
 * on type (e.g., function, variable), they may have a source file and line
 * location, a runtime value, and so on.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @todo There should be a function_symbol as well as a plain symbol.
 * @todo Rename to symbol_info...
 */
class symbol
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct an instance of the psystem::symbol.
     *
     * @param[in] address     The memory location of the debugging symbol.
     * @param[in] code_offset The distance from the function entry point of the
     *                        current PC (in the current frame context).
     * @param[in] name        The (possibly mangled) symbol name.
     */
    symbol(
        address_t address,
        address_t code_offset,
        tstring const& name) noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Access the memory location of the symbol.
     * @return The address exists in the process space of the debuggee.
     */
    address_t get_address() const noexcept;

    /**
     * @brief Access the code offset from the PC to the symbol start.
     *
     * @return The distance from the function entry point of the PC (in the
     *         current frame context).  @c nullptr is a valid value.
     */
    address_t get_code_offset() const noexcept;

    /**
     * @brief The symbol name.
     *
     * @return The symbol name may be empty, and it is likely (if it's a C++
     *         symbol) to be mangled by the compiler.
     */
    tstring get_name() const noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief The memory location of the symbol.
    address_t m_address;

    /**
     * @brief The distance from the function entry point of the PC (in the
     *        current frame context).
     * @todo Move this to a function-specific class.
     */
    address_t m_code_offset;

    /// @brief The symbol's (possibly mangled) name.
    tstring m_name;
};

} // namespace psystem

#endif // PSYSTEM_SYMBOL_HPP
