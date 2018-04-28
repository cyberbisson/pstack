// ===-- src/pstack/pstack_event_handler.cpp -------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define event handlers that accumulate data for generating stack
 *        traces.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PStack 2.0
 * @version PStack 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#include "pstack/pstack_event_handler.hpp"

#include <psystem/framework/memory.hpp>

#include <psystem/module_info.hpp>
#include <psystem/process_info.hpp>
#include <psystem/thread_info.hpp>

namespace pstack {

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

pstack_event_handler::pstack_event_handler() noexcept
    : m_is_debugger_ready(false)
{

}

pstack_event_handler::~pstack_event_handler() noexcept
{

}

////////////////////////////////////////////////////////////////////////////////
// base_debug_event_listener Interface
////////////////////////////////////////////////////////////////////////////////

bool
pstack_event_handler::create_process_event(
    psystem::process_id_t const pid, psystem::thread_id_t const tid,
    psystem::shared_handle<> const& file_handle,
    CREATE_PROCESS_DEBUG_INFO const& info)
    noexcept
{
    m_process_info = std::make_unique<psystem::process_info>(pid, info);
    m_active_thread = tid;

    /// @todo Should we not "share" HANDLE?
    bool const ret = m_process_info->add_thread(
        std::make_unique<psystem::thread_info>(pid, tid, info));

    psystem::module_info mod(file_handle, info);
    m_process_info->m_modules.insert(
        std::make_pair(mod.get_base_of_image(), std::move(mod)));

    /// @todo Manual module file symbol loading
#if 0
    HANDLE file_map = CreateFileMapping(
        file_handle.get(), NULL, PAGE_READONLY, 0, 1, NULL);

    char *base = (char*)MapViewOfFile(file_map, FILE_MAP_READ, 0, 0, 0);

    const IMAGE_DOS_HEADER& dos_header = *((IMAGE_DOS_HEADER *)base);

    if (IMAGE_DOS_SIGNATURE == dos_header.e_magic)
    {
        const IMAGE_NT_HEADERS& ntHeader =
            *((IMAGE_NT_HEADERS *)(base + dos_header.e_lfanew));

        /** @note As of 2007.12.05, there is no cygwin/mingw application with
         **       a major version greater than 1. */
        bool msvc_generated = (1 != ntHeader.OptionalHeader.MajorImageVersion);
    }
#endif

    return ret;
}

bool
pstack_event_handler::create_thread_event(
    psystem::process_id_t const pid, psystem::thread_id_t const tid,
    CREATE_THREAD_DEBUG_INFO const& info)
    noexcept
{
    /// @todo Should we not "share" HANDLE?
    bool const ret = m_process_info->add_thread(
        std::make_unique<psystem::thread_info>(pid, tid, info));
    return ret;
}

bool
pstack_event_handler::exception_event(
    psystem::process_id_t, psystem::thread_id_t,
    EXCEPTION_DEBUG_INFO const& info)
    noexcept
{
    switch (info.ExceptionRecord.ExceptionCode)
    {
    case EXCEPTION_BREAKPOINT:
        m_is_debugger_ready = true;
        return true;
    }

    return false;
}

bool
pstack_event_handler::exit_process_event(
    psystem::process_id_t, psystem::thread_id_t,
    EXIT_PROCESS_DEBUG_INFO const&)
    noexcept
{
    m_process_info.reset();
    return true;
}

bool
pstack_event_handler::exit_thread_event(
    psystem::process_id_t, psystem::thread_id_t tid,
    EXIT_THREAD_DEBUG_INFO const&)
    noexcept
{
    m_process_info->remove_thread(tid);
    return true;
}

bool
pstack_event_handler::load_dll_event(
    psystem::process_id_t, psystem::thread_id_t,
    psystem::shared_handle<> const& file_handle,
    LOAD_DLL_DEBUG_INFO const& info)
    noexcept
{
    psystem::module_info mod(file_handle, info);

    m_process_info->m_modules.insert(
        std::make_pair(mod.get_base_of_image(), std::move(mod)));
    return true;
}

bool
pstack_event_handler::unload_dll_event(
    psystem::process_id_t, psystem::thread_id_t,
    UNLOAD_DLL_DEBUG_INFO const&)
    noexcept
{
    /// @todo Need to figure out index for some kind of set
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////////////////////////

psystem::thread_id_t
pstack_event_handler::get_active_thread_id() const noexcept
{
    return m_active_thread;
}

psystem::process_info *
pstack_event_handler::get_process_info() const noexcept
{
    return m_process_info.get();
}

bool
pstack_event_handler::is_debugger_ready() const noexcept
{
    return m_is_debugger_ready;
}

} // namespace pstack
