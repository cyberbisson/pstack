// ===-- src/psystem/psystem_ns.cpp ----------------------- -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define global functions and data for the PSystem library.
 *
 * Ideally, there should be very little here.
 *
 * @author  Matt Bisson
 * @date    20 November, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#include "psystem/psystem_ns.hpp"
#include "psystem/dll/debug_help_dll.hpp"

#include <psystem/exception/system_exception.hpp>

/// @cond
using library_version = psystem::shared_library::library_version;
/// @endcond

/// @brief Data local to this compilation unit
namespace {

/// @brief We require (at least) version 6.0 of dbghelp.dll to function
///        properly.
library_version const DbgHelp_required_version = 0x00060000;

} // namespace <anon>

namespace psystem {

debug_help_dll DbgHelp;

void
initialize_psystem()
{
    library_version const dbghelp_version = DbgHelp.get_version();
    if (dbghelp_version < DbgHelp_required_version)
    {
        /// @todo Better exception
        throw psystem::exception::system_exception(
            __FILE__, __FUNCTION__, __LINE__, 0,
            "Need version %u.%u or higher of DbgHelp.dll (found version %u.%u)",
            DbgHelp_required_version >> 16, DbgHelp_required_version & 0xFFFF,
            dbghelp_version          >> 16, dbghelp_version          & 0xFFFF);
    }

    DbgHelp.load_all_functions();
}

} // namespace psystem
