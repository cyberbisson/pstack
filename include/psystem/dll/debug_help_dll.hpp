// ===-- include/psystem/dll/debug_help_dll.hpp ----------- -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare the wrapper for Microsoft's Debug Helper DLL.
 *
 * @author  Matt Bisson
 * @date    14 July, 2008 -- 20 November, 2015
 * @since   ProcLib 1.3
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#pragma once
#ifndef PSYSTEM_DLL_DBGHELP_HPP
#define PSYSTEM_DLL_DBGHELP_HPP

#include "psystem/psystem_ns.hpp"

#include <DbgHelp.h>

#include "psystem/framework/shared_library.hpp"

namespace psystem {

/**
 * @brief Wraps invocations into the Windows DbgHelp DLL onto a lazily-loaded
 *        version.
 *
 * This decoupling of the "automatic" DLL invocation allows us to check the
 * version of DbgHelp.dll before continuing.  Slight API differences have caused
 * difficulty in the past.
 *
 * @author  Matt Bisson
 * @date    14 July, 2008 -- 20 November, 2015
 * @since   ProcLib 1.3
 * @version PSystem 2.0
 *
 * @todo unload() will leave bad function pointers here.
 */
class debug_help_dll final
    : public psystem::shared_library
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct (without loading) an instance to wrap DbgHelp.dll.
     */
    debug_help_dll() noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Populates all loaded_function instances in from the library
     *        immediately.
     * @throws psystem::exception::system_exception
     *     if any of the following occur:
     *     @li the library could not be loaded.
     *     @li the specified symbol could not be found in the loaded shared
     *         library.
     */
    void load_all_functions();

    ////////////////////////////////////////////////////////////////////////////
    /// @name DbgHelp Imports
    ////////////////////////////////////////////////////////////////////////////
public:
    /// @brief See MSDN for details on  [StackWalk64](https://msdn.microsoft.com/en-us/library/windows/desktop/ms680650%28v=vs.85%29.aspx).
    loaded_function<decltype(::StackWalk64)> StackWalk64;
    /// @brief See MSDN for details on  [SymCleanup](https://msdn.microsoft.com/en-us/library/windows/desktop/ms680696%28v=vs.85%29.aspx).
    loaded_function<decltype(::SymCleanup)> SymCleanup;
    /// @brief See MSDN for details on  [SymFromAddr](https://msdn.microsoft.com/en-us/library/windows/desktop/ms681323%28v=vs.85%29.aspx).
    loaded_function<decltype(::SymFromAddr)> SymFromAddr;
    /// @brief See MSDN for details on  [SymFunctionTableAccess64](https://msdn.microsoft.com/en-us/library/windows/desktop/ms681327%28v=vs.85%29.aspx).
    loaded_function<decltype(::SymFunctionTableAccess64)> SymFunctionTableAccess64;
    /// @brief See MSDN for details on  [SymGetModuleBase64](https://msdn.microsoft.com/en-us/library/windows/desktop/ms681335%28v=vs.85%29.aspx).
    loaded_function<decltype(::SymGetModuleBase64)> SymGetModuleBase64;
    /// @brief See MSDN for details on  [SymGetOptions](https://msdn.microsoft.com/en-us/library/windows/desktop/ms681337%28v=vs.85%29.aspx).
    loaded_function<decltype(::SymGetOptions)> SymGetOptions;
    /// @brief See MSDN for details on  [SymInitialize](https://msdn.microsoft.com/en-us/library/windows/desktop/ms681351%28v=vs.85%29.aspx).
    loaded_function<decltype(::SymInitialize)> SymInitialize;
    /// @brief See MSDN for details on  [SymSetOptions](https://msdn.microsoft.com/en-us/library/windows/desktop/ms681366%28v=vs.85%29.aspx).
    loaded_function<decltype(::SymSetOptions)> SymSetOptions;
};

/// @brief The loaded instance of DbgHelp.dll.
extern debug_help_dll DbgHelp;

} // namespace psystem

#endif // PSYSTEM_DLL_DBGHELP_HPP

