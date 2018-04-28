// ===-- include/pstack/pstack_event_handler.hpp ---------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare a handler specifically designed to gather data for printing
 *        stack traces.
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

#pragma once
#ifndef PSTACK_EVENT_HANDLER_HPP
#define PSTACK_EVENT_HANDLER_HPP

#include "pstack_ns.hpp"

#include <psystem/framework/memory.hpp>
#include <proclib/base_debug_event_listener.hpp>

namespace psystem
{
    class process_info;
}

namespace pstack {

/**
 * @brief Handle debugger events with the goal of generating stack traces.
 *
 * This class handles events sent by the debugger --- building structures as it
 * goes --- so that it can generate stack traces for one or more threads in a
 * single process.
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
class pstack_event_handler final
    : public proclib::base_debug_event_listener
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /// @brief Construct the pstack_event_handler instance.
    pstack_event_handler() noexcept;

    /// @brief Destroy the pstack_event_handler instance.
    virtual ~pstack_event_handler() noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name base_debug_event_listener Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /// @copydoc proclib::base_debug_event_listener::create_process_event()
    /// @post get_process_info() returns a valid psystem::process_info instance.
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
    /// @copydoc proclib::base_debug_event_listener::exit_process_event()
    /// @post get_process_info() returns @c nullptr.  Any previous values
    ///       returned from the method are invalidated.
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
    virtual bool unload_dll_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        UNLOAD_DLL_DEBUG_INFO const& info) noexcept override;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Accessors
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Access the ID for the thread that was executing when the debugger
     *        attached.
     * @return A system thread ID (TID).  If no events have been seen, this
     *         value is undefined.
     */
    psystem::thread_id_t get_active_thread_id() const noexcept;

    /**
     * @brief Access information about the process that generated the events for
     *        this handler.
     *
     * @return
     *     This returns @c nullptr if it is called before this handler receives
     *     create_process_event(), and after exit_process_event() (without a
     *     subsequent @c create_process_event).  A pointer returned by this
     *     function is only valid until the next @c exit_process_event, or until
     *     the instance is destroyed.
     */
    psystem::process_info *get_process_info() const noexcept;

    /**
     * @brief Determines if the debugger is ready to give a valid stack trace
     *        for all threads.
     *
     * @return
     *     This is @c false until the pstack_event_handler decides that it has
     *     seen all of the debugger events that come from the debugger attaching
     *     to a process.
     */
    bool is_debugger_ready() const noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief The process for which events will be dispatched to this handler.
    std::unique_ptr<psystem::process_info> m_process_info;

    /** @brief The debugger is ready to make stack traces for all threads.
     *
     * @todo Should this be a state machine to allow us to determine "saw
     *       process" -> "saw threads" -> "saw modules" -> etc?
     */
    bool m_is_debugger_ready;

    /// @brief The thread that was executing code when the debugger attached.
    psystem::thread_id_t m_active_thread;
};

} // namespace pstack

#endif // PSTACK_EVENT_HANDLER_HPP
