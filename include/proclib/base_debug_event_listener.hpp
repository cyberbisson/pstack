// ===-- include/proclib/base_debug_event_listener.hpp ---- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declares the proclib::base_debug_event_listener interface.
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

#pragma once
#ifndef PROCLIB_BASE_DEBUG_EVENT_LISTENER_HPP
#define PROCLIB_BASE_DEBUG_EVENT_LISTENER_HPP

#include "proclib_ns.hpp"

#include <psystem/framework/shared_handle.hpp>

namespace proclib {

/**
 * @brief A polymorphic interface for detecting events received by the debugger.
 *
 * It is not possible to use this interface directly to listen for debugging
 * events, but it does provide a default implementation for derived classes
 * whereby the event is ignored.
 *
 * To handle an event, derived classes should override one of the methods
 * described by base_debug_event_listener's interface, and return @c true as the
 * result.  No exceptions may be thrown from an event handler.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   ProcLib 2.0
 * @version ProcLib 2.0
 */
class base_debug_event_listener
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Clean up base_debug_event_listener resources.
     */
    virtual ~base_debug_event_listener() noexcept;

protected:
    /**
     * @brief Construct the base_debug_event_listener instance.
     *
     * The constructor is protected because it is essentially pointless to
     * create (and register) an instance of the base listener.
     */
    base_debug_event_listener() noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Invoked when the debugger sends an event to indicate process
     *        creation.
     *
     * This events is sent upon attachment to a running process, or when the
     * debugger itself creates a process for debugging.  This event is important
     * also because it signifies that the executable module has been loaded, and
     * that the main process thread has been created.
     *
     * @param[in] pid The ID of the process that signaled this event.
     * @param[in] tid The ID of the thread that is currently in the active state.
     * @param[in] file_handle
     *     The "shared" handle to the open file that represents this process's
     *     module.  The handle will be closed if no listeners copy the
     *     @c shared_handle.
     * @param[in] info
     *     Information that pertains specifically to this event.  Note that any
     *     handles in the structure may be @c nullptr, and that the @p hFile
     *     member is only valid until exit_process_event().
     *
     * @return This method should return @c true to indicate that it has taken
     *     action based on receiving the event, or @c false to indicate that it
     *     has not.
     */
    virtual bool create_process_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        psystem::shared_handle<> const& file_handle,
        CREATE_PROCESS_DEBUG_INFO const& info) noexcept;

    /**
     * @brief Invoked when the debugger sends an event to indicate thread
     *        creation.
     *
     * This event is sent when a new thread is created during normal execution
     * in a process being debugged, or when the debugger attaches to a process
     * and encounters a preexisting thread of execution.
     *
     * @param[in] pid  The ID of the process that signaled this event.
     * @param[in] tid  The ID of the thread that was created or discovered.
     * @param[in] info Information that applies specifically to this event.
     *
     * @return This method should return @c true to indicate that it has taken
     *     action based on receiving the event, or @c false to indicate that it
     *     has not.
     */
    virtual bool create_thread_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        CREATE_THREAD_DEBUG_INFO const& info) noexcept;

    /**
     * @brief Invoked when the debugger sends an event to indicate some exception
     *        in normal process execution.
     *
     * An exception may be an unhandled C++ exception, it may be a system-level
     * exception (i.e., @c sigsegv on UNIX or GPF on Windows plaforms), or it
     * may simply indicate that the debugger has reached a breakpoint (which may
     * be requested either by the debugger itself, or by a user-specified break).
     *
     * @param[in] pid The ID of the process that signaled this event.
     * @param[in] tid The ID of the thread that is currently in the active state.
     * @param[in] info
     *     Information that applies specifically to this event.  This will
     *     contain the exception record, which in turn contains the cause of the
     *     exception.
     *
     * @return This method should return @c true to indicate that it has taken
     *     action based on receiving the event, or @c false to indicate that it
     *     has not.
     */
    virtual bool exception_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        EXCEPTION_DEBUG_INFO const& info) noexcept;

    /**
     * @brief Invoked when the debugger sends an event to indicate that the
     *        process has exited.
     *
     * @param[in] pid  The ID of the process that signaled this event.
     * @param[in] tid  The ID of the thread that was in the active state on exit.
     * @param[in] info Information that applies specifically to this event.
     *
     * @return This method should return @c true to indicate that it has taken
     *     action based on receiving the event, or @c false to indicate that it
     *     has not.
     *
     * @post All handles pertaining to this process are invalid.
     */
    virtual bool exit_process_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        EXIT_PROCESS_DEBUG_INFO const& info) noexcept;

    /**
     * @brief Invoked when the debugger sends an event to indicate that a thread
     *        has exited.
     *
     * @param[in] pid  The ID of the process that signaled this event.
     * @param[in] tid  The ID of the thread that just exited.
     * @param[in] info Information that applies specifically to this event.
     *
     * @return This method should return @c true to indicate that it has taken
     *     action based on receiving the event, or @c false to indicate that it
     *     has not.
     *
     * @post The thread handle retrieved from create_thread_event() is no longer
     *       valid.
     */
    virtual bool exit_thread_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        EXIT_THREAD_DEBUG_INFO const& info) noexcept;

    /**
     * @brief Invoked when the debugger sends an event to indicate that a DLL
     *        hase been loaded.
     *
     * This callback may be triggered if a DLL has been loaded into a running
     * process, or if the debugger discovers an already loaded DLL when
     * attaching to a process.
     *
     * @param[in] pid The ID of the process that signaled this event.
     * @param[in] tid The ID of the thread that is currently in the active state.
     * @param[in] file_handle
     *     The "shared" handle to the open file that represents this process's
     *     module.  The handle will be closed if no listeners copy the
     *     @c shared_handle.
     * @param[in] info
     *     Information that applies specifically to this event.  Note that any
     *     handles in the structure may be @c nullptr, and that the @p hFile
     *     member is only valid until unload_dll_event().
     *
     * @return This method should return @c true to indicate that it has taken
     *     action based on receiving the event, or @c false to indicate that it
     *     has not.
     */
    virtual bool load_dll_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        psystem::shared_handle<> const& file_handle,
        LOAD_DLL_DEBUG_INFO const& info) noexcept;

    /**
     * @brief Invoked when the debugger encounters a trace message from the
     *        debuggee.
     *
     * @param[in] pid  The ID of the process that signaled this event.
     * @param[in] tid  The ID of the currently active thread.
     * @param[in] info Information that applies specifically to this event.
     *
     * @return This method should return @c true to indicate that it has taken
     *     action based on receiving the event, or @c false to indicate that it
     *     has not.
     */
    virtual bool output_debug_string_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        OUTPUT_DEBUG_STRING_INFO const& info) noexcept;

    /**
     * @brief Invoked when the debugger encounters a fatal application exit or
     *        debugger error.
     *
     * Technically, this event indicates that the debugger passed invalid data
     * to the application, however the application itself generally causes this.
     * The debugger receives exception_event() on any potential crash from the
     * debuggee, and if it forwards this exception to the debuggee, it may
     * result in a crash that triggers this event.
     *
     * @param[in] pid  The ID of the process that signaled this event.
     * @param[in] tid  The ID of the currently active thread.
     * @param[in] info Information that applies specifically to this event.
     *
     * @return This method should return @c true to indicate that it has taken
     *     action based on receiving the event, or @c false to indicate that it
     *     has not.
     */
    virtual bool rip_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        RIP_INFO const& info) noexcept;

    /**
     * @brief Invoked when the debugger encounters a DLL unload.
     *
     * @param[in] pid  The ID of the process that signaled this event.
     * @param[in] tid  The ID of the currently active thread.
     * @param[in] info Information that applies specifically to this event.
     *
     * @return This method should return @c true to indicate that it has taken
     *     action based on receiving the event, or @c false to indicate that it
     *     has not.
     *
     * @post The handle retrieved from the corresponding load_dll_event() is no
     *       longer valid.
     */
    virtual bool unload_dll_event(
        psystem::process_id_t pid, psystem::thread_id_t tid,
        UNLOAD_DLL_DEBUG_INFO const& info) noexcept;
};

} // namespace proclib

#endif // PROCLIB_BASE_DEBUG_EVENT_LISTENER_HPP
