// ===-- src/proclib/base_debug_event_listener.cpp -------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Defines the proclib::base_debug_event_listener functionality.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   ProcLib 2.0
 * @version ProcLib 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#include "proclib/base_debug_event_listener.hpp"

namespace proclib {

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

base_debug_event_listener::base_debug_event_listener() noexcept
{

}

base_debug_event_listener::~base_debug_event_listener() noexcept
{

}

////////////////////////////////////////////////////////////////////////////////
// Public Interface
////////////////////////////////////////////////////////////////////////////////

bool
base_debug_event_listener::create_process_event(
    psystem::process_id_t,
    psystem::thread_id_t,
    psystem::shared_handle<> const&,
    CREATE_PROCESS_DEBUG_INFO const&) noexcept
{
    return false;
}

bool
base_debug_event_listener::create_thread_event(
    psystem::process_id_t,
    psystem::thread_id_t,
    CREATE_THREAD_DEBUG_INFO const&) noexcept
{
    return false;
}

bool
base_debug_event_listener::exception_event(
    psystem::process_id_t,
    psystem::thread_id_t,
    EXCEPTION_DEBUG_INFO const&) noexcept
{
    return false;
}

bool
base_debug_event_listener::exit_process_event(
    psystem::process_id_t,
    psystem::thread_id_t,
    EXIT_PROCESS_DEBUG_INFO const&) noexcept
{
    return false;
}

bool
base_debug_event_listener::exit_thread_event(
    psystem::process_id_t,
    psystem::thread_id_t,
    EXIT_THREAD_DEBUG_INFO const&) noexcept
{
    return false;
}

bool
base_debug_event_listener::load_dll_event(
    psystem::process_id_t,
    psystem::thread_id_t,
    psystem::shared_handle<> const&,
    LOAD_DLL_DEBUG_INFO const&) noexcept
{
    return false;
}

bool
base_debug_event_listener::output_debug_string_event(
    psystem::process_id_t,
    psystem::thread_id_t,
    OUTPUT_DEBUG_STRING_INFO const&) noexcept
{
    return false;
}

bool
base_debug_event_listener::rip_event(
    psystem::process_id_t,
    psystem::thread_id_t,
    RIP_INFO const&) noexcept
{
    return false;
}

bool
base_debug_event_listener::unload_dll_event(
    psystem::process_id_t,
    psystem::thread_id_t,
    UNLOAD_DLL_DEBUG_INFO const&) noexcept
{
    return false;
}

} // namespace proclib
