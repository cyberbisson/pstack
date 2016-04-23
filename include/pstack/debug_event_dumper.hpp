// ===-- include/pstack/debug_event_dumper.hpp ------------ -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare an event listener to provide verbose information about events.
 *
 * @author  Matt Bisson
 * @date    14 February, 2008 -- 5 August, 2014
 * @since   PStack 1.2
 * @version PStack 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#pragma once
#ifndef PSTACK_DEBUG_EVENT_DUMPER_HPP
#define PSTACK_DEBUG_EVENT_DUMPER_HPP

#include "pstack_ns.hpp"
#include <proclib/base_debug_event_listener.hpp>

namespace pstack {

/**
 * @brief As this class encounters events, it prints details to @c stdout.
 *
 * This class is useful for viewing the events that the debugger sees in real
 * time.  Simply create an instance of this class, and begin printing events by
 * invoking the psystem::process_debugger::add_event_listener() method.
 *
 * @author  Matt Bisson
 * @date    14 February, 2008 -- 5 August, 2014
 * @since   PStack 1.2
 * @version PStack 2.0
 *
 * @todo Make the output stream a CTOR parameter.
 */
class debug_event_dumper
    : public proclib::base_debug_event_listener
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name base_debug_event_listener Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    virtual bool create_process_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        psystem::shared_handle<> const& file_handle,
        CREATE_PROCESS_DEBUG_INFO const& info) noexcept override;
    virtual bool create_thread_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        CREATE_THREAD_DEBUG_INFO const& info) noexcept override;
    virtual bool exception_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        EXCEPTION_DEBUG_INFO const& info) noexcept override;
    virtual bool exit_process_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        EXIT_PROCESS_DEBUG_INFO const& info) noexcept override;
    virtual bool exit_thread_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        EXIT_THREAD_DEBUG_INFO const& info) noexcept override;
    virtual bool load_dll_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        psystem::shared_handle<> const& file_handle,
        LOAD_DLL_DEBUG_INFO const& info) noexcept override;
    virtual bool output_debug_string_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        OUTPUT_DEBUG_STRING_INFO const& info) noexcept override;
    virtual bool rip_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        RIP_INFO const& info) noexcept override;
    virtual bool unload_dll_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        UNLOAD_DLL_DEBUG_INFO const& info) noexcept override;
};

} // namespace pstack

#endif // PSTACK_DEBUG_EVENT_DUMPER_HPP
