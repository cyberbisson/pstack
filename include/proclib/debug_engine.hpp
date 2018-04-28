// ===-- include/proclib/debug_engine.hpp ----------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare an "engine" to drive debugging events and contain require
 *        data.
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
#ifndef PROCLIB_DEBUG_ENGINE_HPP
#define PROCLIB_DEBUG_ENGINE_HPP

#include "proclib_ns.hpp"

#include <map>

#include <psystem/framework/memory.hpp>
#include <psystem/framework/not_copyable.hpp>

namespace proclib {

class debugger_cache;
class process_debugger;

/**
 * @brief A management interface for debugging applications.
 *
 * The debug_engine is responsible for attaching to processes, creating debugged
 * processes, dispatching events from the debugger and so on.  The engine is not
 * specific to any one process, but it does manage the process_debugger
 * instances, which provide access to process specific data.
 *
 * @warning
 *     Constructing multiple instances of this class causes undefined behavior
 *     when invoking attach_to_process() on the same PID.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   ProcLib 2.0
 * @version ProcLib 2.0
 */
class debug_engine
    : public psystem::not_copyable
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Type Definitions
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Container for process_debuggers
     */
    using process_debuggers =
        std::map< psystem::process_id_t, std::unique_ptr<process_debugger> >;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct an instance of the debug engine.
     *
     * @param[in] kill_on_exit
     *     When the debugger detaches from a process, this flag determines the
     *     fate of the process being debugged.  If @c true, the process exits
     *     when the debugger detaches, otherwise, it continues normal execution.
     * @warning
     *     Unfortunately, Microsoft's debugger implementation makes this a
     *     global flag for all debuggers.  It is advisable to only debug
     *     multiple processes when the flag will be consistent across all
     *     instances, and will never change.
     */
    explicit debug_engine(bool kill_on_exit = false) noexcept;

    /**
     * @brief Clean up resources used by debug_engine.
     *
     * This will deallocate all active process_debugger instances, and their
     * data.
     */
    ~debug_engine() noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Attach a debugger to a running process using the process ID (PID).
     *
     * Upon attaching to a running process, the debugger immediately freezes the
     * debuggee, and waits for debug events to be consumed (with
     * wait_for_event()).
     *
     * @param[in] pid The ID of the process to which the debugger will attach.
     *
     * @return
     *     A recently attached process_debugger instance that is being managed
     *     by this engine.
     * @throws psystem::exception::system_exception
     *     If the native debugger could not attach to the process.
     *
     * @attention
     *     Calls process_debugger::set_kill_process_on_exit() (which has global
     *     ramifications).
     * @pre  @c get_process_debugger(pid) is unsuccessful.
     * @post @c get_process_debugger(pid) returns a valid process_debugger.
     */
    process_debugger& attach_to_process(psystem::process_id_t pid);

    /**
     * @brief Allows the current process to debug other processes.
     *
     * The method must be executed at least once before attaching a debugger to
     * any running processes.
     *
     * @throws psystem::exception::system_exception
     *     If there was an error accessing the current process or altering its
     *     privileges with the OS.
     */
    void enable_debug_privilege() const;

    /**
     * @brief Fetch a process_debugger instance.
     *
     * @param[in] pid The ID of the process associated to a debugger.
     * @return
     *     A pointer to the found process_debugger instance.  Its lifespan
     *     lasts as long as the debugger is attached to the process with this
     *     debug_engine.  If no process_debugger is found, this method simply
     *     returns @c nullptr.
     */
    process_debugger *get_process_debugger(psystem::process_id_t pid) const;

    /**
     * @brief Block the current thread until any debugger sees an event.
     *
     * This method automatically forwards the received event on to the
     * appropriate process_debugger.  To register for events, invoke
     * process_debugger::add_event_listener() with a custom instance of
     * proclib::base_debug_event_listener.
     *
     * @param[in] ms The amount of time (in ms) to wait for the next event.
     * @return
     *     @c true if some event handler responded to the event in a non-trivial
     *     way, @c false otherwise.
     * @throws psystem::exception::system_exception
     *     If there was a problem waiting for an event or continuing execution
     *     after the event.
     *
     * @see proclib::base_debug_event_listener
     * @see proclib::process_debugger::add_event_listener()
     */
    bool wait_for_event(DWORD ms = INFINITE) const;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief A set of attached debuggers indexed by process ID (PID)
    process_debuggers m_debuggers;

    /// @brief Was the debug_engine asked to kill attached processes on detach?
    bool const m_kill_on_exit;
};

} // namespace proclib

#endif // PROCLIB_DEBUG_ENGINE_HPP
