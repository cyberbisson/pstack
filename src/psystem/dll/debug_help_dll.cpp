// ===-- src/psystem/dll/debug_help_dll.cpp --------------- -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define functionality for the wrapper for Microsoft's Debug Helper DLL.
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

#include "psystem/dll/debug_help_dll.hpp"

////////////////////////////////////////////////////////////////////////////////
/// @name Local Macros
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Quick macro to load a function by name into a load_function instance.
 * @todo This doesn't update the base class' function pointers!
 */
#define LOAD_FUNCTION(f) f.retarget(get_function(#f))

namespace psystem {

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

debug_help_dll::debug_help_dll() noexcept
    : shared_library("dbghelp.dll", 8),
      StackWalk64(nullptr),
      SymCleanup(nullptr),
      SymFromAddr(nullptr),
      SymFunctionTableAccess64(nullptr),
      SymGetModuleBase64(nullptr),
      SymGetOptions(nullptr),
      SymInitialize(nullptr),
      SymSetOptions(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////
// Public Interface
////////////////////////////////////////////////////////////////////////////////

void
debug_help_dll::load_all_functions()
{
    load();

    LOAD_FUNCTION(StackWalk64);
    LOAD_FUNCTION(SymCleanup);
    LOAD_FUNCTION(SymFromAddr);
    LOAD_FUNCTION(SymFunctionTableAccess64);
    LOAD_FUNCTION(SymGetModuleBase64);
    LOAD_FUNCTION(SymGetOptions);
    LOAD_FUNCTION(SymInitialize);
    LOAD_FUNCTION(SymSetOptions);
}

} // namespace psystem
