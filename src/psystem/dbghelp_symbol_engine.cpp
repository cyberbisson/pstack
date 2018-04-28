// ===-- src/psystem/dbghelp_symbol_engine.cpp ------------ -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Defines a debugger symbol engine that uses the DbgHelp library.
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

#include "psystem/dbghelp_symbol_engine.hpp"

#include <cstring>
#include <iostream> /// @todo Remove iostream when we have a logging facility.

#include <psystem/framework/memory.hpp>
#include <psystem/dll/debug_help_dll.hpp>
#include <psystem/symbol.hpp>
#include <psystem/exception/system_exception.hpp>

namespace psystem {

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

dbghelp_symbol_engine::dbghelp_symbol_engine(HANDLE const h_process)
    : m_process_handle(h_process)
{
    ASSERT(m_process_handle);

    // First, get options, then set/unset, then check the result
    auto const init_options = DbgHelp.SymGetOptions();
    auto const desired_options =
        init_options                  |
        SYMOPT_ALLOW_ABSOLUTE_SYMBOLS |
        SYMOPT_DEBUG                  |
        SYMOPT_DEFERRED_LOADS         |
        SYMOPT_FAIL_CRITICAL_ERRORS   |
        SYMOPT_INCLUDE_32BIT_MODULES  |
        SYMOPT_LOAD_LINES             |
        SYMOPT_NO_PROMPTS;
    auto const got_options = DbgHelp.SymSetOptions(desired_options);

    /// @todo This should be an except
    ASSERT_USING_VAR(got_options, got_options == desired_options);

    /// @todo In order to not "invade process = TRUE" we need to replace the
    ///       callbacks in StackWalk.
    /// @todo Custom search path for symbols?
    if (!DbgHelp.SymInitialize(m_process_handle, nullptr, TRUE))
    {
        THROW_WINDOWS_EXCEPTION(
            GetLastError(), "Cannot initialize symbol engine");
    }
}

dbghelp_symbol_engine::~dbghelp_symbol_engine()
try
{
    if (!DbgHelp.SymCleanup(m_process_handle))
    {
        THROW_WINDOWS_EXCEPTION(
            GetLastError(), "Cannot shut down symbol engine");
    }
}
catch (psystem::exception::system_exception const& sex)
{
    // Since we shouldn't throw from the destructor, let's just log and move on.
    // We can't really do much if this fails anyway.

    /// @todo Replace this when we have a logging facility.
    std::cerr << "pstack: " << sex << '\n';
}

////////////////////////////////////////////////////////////////////////////////
// Public Interface
////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<symbol>
dbghelp_symbol_engine::find_symbol(address_t const address)
{
    /// @todo Probably don't want to do this on the stack.  In fact, since this
    ///       is single-threaded, we might just allocate a static buffer?
    size_t const kSymbolSize =
        sizeof(SYMBOL_INFO) + ((MAX_SYM_NAME + 1) * sizeof(TCHAR));
    BYTE sym_buffer[kSymbolSize];
    SYMBOL_INFO *const symbol_info = reinterpret_cast<SYMBOL_INFO*>(sym_buffer);

    memset(symbol_info, 0, kSymbolSize);
    symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol_info->MaxNameLen   = MAX_SYM_NAME;

    DWORD64 code_offset = 0;

    if (!DbgHelp.SymFromAddr(
            m_process_handle,
            static_cast<DWORD64>(address),
            &code_offset,
            symbol_info))
    {
        DWORD const last_error = GetLastError();
        if (ERROR_INVALID_ADDRESS == last_error)
        {
            return nullptr;
        }

        // Throw an exception unless it's simply not found...
        THROW_WINDOWS_EXCEPTION(
            last_error, "Error resolving symbol from address");
    }

    /// @todo Strange to return an unique_ptr here...
    return std::make_unique<symbol>(
        address,
        code_offset,
        reinterpret_cast<TCHAR*>(symbol_info->Name));
}

} // namespace psystem
