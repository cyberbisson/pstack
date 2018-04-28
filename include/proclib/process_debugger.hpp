// ===-- include/proclib/process_debugger.hpp ------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare a debugger-to-process relationship.
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
#ifndef PROCLIB_PROCESS_DEBUGGER_HPP
#define PROCLIB_PROCESS_DEBUGGER_HPP

#include "proclib_ns.hpp"

#include <psystem/framework/memory.hpp>
#include <psystem/framework/not_copyable.hpp>

namespace proclib {

class base_debug_event_listener;

/**
 * @brief Debugging facility for an individual process.
 *
 * This class ties the debugger to a specific debuggee process.  The
 * process_debugger responds to all debug events that pertain to its debuggee.
 * Clients may receive (and deal with) these events using the
 * add_event_listener() method with a custom base_debug_event_listener.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   ProcLib 2.0
 * @version ProcLib 2.0
 *
 * @todo
 *     This doesn't do much right now.  Maybe it should automatically add the
 *     (currently) pstack::pstack_event_handler and provide access to
 *     process_info, etc?
 */
class process_debugger
    : psystem::not_copyable
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Type Definitions
    ////////////////////////////////////////////////////////////////////////////
public:
    /// @brief An unique ID for events registered with add_event_listener()
    using event_listener_id_t = int32_t;

    /// @brief Indicates that the value stored in a
    ///        process_debugger::event_listener_id_t is not valid.
    static constexpr event_listener_id_t invalid_event_listener_id = -1;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Create an instance of the debugger with a given process ID.
     *
     * @param[in] pid
     *     The numerical ID of a process that is currently active to attach to
     *     the debugger.
     * @throws psystem::exception::system_exception
     *     If the debugger cannot attach to the specified running process
     *     (including if the process with the given ID does not exist).
     */
    explicit process_debugger(psystem::process_id_t pid);

    /**
     * @brief Detach the debugger from the process.
     *
     * Based on prior calls to set_kill_process_on_exit(), this method may allow
     * the debugged process to continue normal execution, or it may terminate
     * the process.
     *
     * @note
     *     If the debugger could not detach from the process that was previously
     *     attached, this method will log the error.
     * @post All registered listeners will be released, possibly causing
     *       deletion.
     */
    ~process_debugger();

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Accessors
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Access the main event handler for this process.
     *
     * This is mostly useful for the proclib::debug_engine to pass
     * process-specific events to this class (for distribution to listeners
     * registered via add_event_listener()).
     *
     * @return A reference to the process's events.  It is valid until this
     *         instance is destroyed.
     */
    base_debug_event_listener& get_events() const noexcept;

    /**
     * @brief Get the ID of the process being debugged here.
     * @return The numerical process ID (PID).
     */
    psystem::process_id_t get_process_id() const noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Add a listener for debugger events sent to this process.
     *
     * This method inserts a listener for debugger events that are relevent for
     * the process managed by this process_debugger instance.  The event
     * handlers are not guaranteed to be fired in any order.  Event handlers
     * must not throw exceptions.
     *
     * Handlers must neither add nor remove listener while handling an event.
     * Doing so results in undefined behavior.
     *
     * @param[in] l The custom listener to register.
     * @return
     *     A "token" that uniquely identifies the listener that was successfully
     *     added.  If the listener was not added, this method returns
     *     process_debugger::invalid_event_listener_id.
     *
     * @pre  An event is not currently being processed.
     * @pre  @p l has not already been registered with this process_debugger.
     * @post @p l will begin receiving debugger events pertaining to this
     *       process.
     */
    event_listener_id_t add_event_listener(
        std::shared_ptr<base_debug_event_listener> const& l);

    /**
     * @brief Remove an already registered event listener.
     *
     * This method removes a listener that was previously registered with
     * add_event_listener() by its ID.  If the ID was not found, this method
     * does nothing.
     *
     * @param[in] id The ID received on addition of the listener instance (from
     *               add_event_listener()).
     * @return @c true if the listener was found and removed; @c false if not.
     *
     * @pre  An event is not currently being processed.
     * @pre  A listener with an ID of @p id has already been registered with
     *       this process_debugger.
     * @post The listener with an ID of @p id will no longer receive debugger
     *       events pertaining to this process.
     */
    bool remove_event_listener(event_listener_id_t id);

    /**
     * @brief Determine the fate of a process after the debugger detaches.
     *
     * When the debugger detaches from a process or teminates, the process being
     * debugged may either continue normal execution or terminate.  The default
     * action is to terminate the debuggee.  Typical debugger behavior is to
     * terminate a process that was created by the debugger, and allow a process
     * to which the debugger attached to continue normal execution.
     *
     * @param[in] kill_on_exit
     *     If @c true, the debugger should terminate the debuggee when
     *     detaching.  Otherwise, the debuggee will continue normal execution.
     *
     * @attention
     *     This is a global flag for all debuggers.  If not called, processes
     *     will be set to be killed on exit.  This is a member function because
     *     you must have connected to at least one debugger before even calling
     *     it.
     */
    void set_kill_process_on_exit(bool kill_on_exit);

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief The Process ID (PID) of the process debugged by this instance.
    psystem::process_id_t const m_process_id;

    // PImpl pattern (somewhat) -- make a pointer to the data that will be used
    // for real work so we can forward declare it and avoid a direct header
    // dependency.
    struct dispatching_event_listener;

    /**
     * @brief Debug events will be passed here, and forwarded to added custom
     *        versions of the listeners.
     */
    std::unique_ptr<dispatching_event_listener> const m_events;
};

} // namespace proclib

#endif // PROCLIB_PROCESS_DEBUGGER_HPP
