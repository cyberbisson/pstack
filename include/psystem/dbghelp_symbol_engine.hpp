// ===-- include/psystem/dbghelp_symbol_engine.hpp -------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declares a debugger symbol engine that uses the DbgHelp library.
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
#ifndef PSYSTEM_DBGHELP_SYMBOL_ENGINE_HPP
#define PSYSTEM_DBGHELP_SYMBOL_ENGINE_HPP

#include "psystem_ns.hpp"

#include <functional>
#include <memory>
#include <psystem/framework/not_copyable.hpp>

namespace psystem {

class symbol;

/**
 * @brief Provide an facility (using DbgHelp) for reading and mapping debugger
 *        symbols.
 *
 * The DbgHelp library is a Windows-specific library for reading debugging
 * symbols, mapping them to addresses, and even gathering source code locations
 * where available.  This interface encapsulates the library for use with the
 * rest of the PSystem modules.
 *
 * A governing philosophy of symbol engines in psystem is that they do not
 * maintain any stateful information for the symbols themselves, they only
 * provide a consistent facade for one or more symbol APIs.
 *
 * @warning
 *     According to the DbgHelp documentation, most all of the functions are
 *     single-threaded.  Avoid accessing the methods here from multiple threads
 *     without some serialization strategy.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
class dbghelp_symbol_engine
    : public psystem::not_copyable
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct an instance of the symbol engine.
     *
     * @warning
     *     Creating multiple instances of dbghelp_symbol_engine with the same
     *     process handle will result in undefined behavior.
     *
     * @param[in] h_process An open handle to the process for which symbols
     *                      should be loaded.
     *
     * @throws psystem::exception::system_exception
     *     If there was an error initializing the DbgHelp symbol server with the
     *     given process handle.
     * @todo Track process handles and throw an exception if the same one is
     *     accessed twice.
     */
    explicit dbghelp_symbol_engine(HANDLE h_process);

    /**
     * @brief Free previously allocated resources for debug symbol usage.
     */
    ~dbghelp_symbol_engine();

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Create a psystem::symbol instance for a symbol at the provided
     *        memory address.
     *
     * This is a factory method.  After calling this method, ownership is
     * transferred directly to the caller.  Invoking this multiple times with
     * the same address results in multiple symbol instances.  It is not
     * idempotent.
     *
     * @param[in] address The address to match to a debugger symbol.
     * @return
     *     If a symbol is found, this method returns a @c std::unique_ptr to a
     *     new instance of psystem::symbol with the information.  If not, this
     *     method returns @c nullptr.
     * @throws psystem::exception::system_exception
     *     If there was a system error accessing the symbol from DbgHelp.
     */
    std::unique_ptr<symbol> find_symbol(address_t address);

private:
    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////

    /// @brief An open handle to the process used to gather symbol information.
    HANDLE const m_process_handle;
};

} // namespace psystem

#endif // PSYSTEM_DBGHELP_SYMBOL_ENGINE_HPP
