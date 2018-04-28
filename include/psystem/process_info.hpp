// ===-- include/psystem/process_info.hpp ----------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare a container object for a process's information.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#pragma once
#ifndef PSYSTEM_PROCESS_INFO_HPP
#define PSYSTEM_PROCESS_INFO_HPP

#include "psystem_ns.hpp"

#include <functional>
#include <map>

#include <psystem/framework/memory.hpp>
#include <psystem/framework/not_assignable.hpp>

#include <psystem/module_info.hpp>

namespace psystem {

class thread_info;

/**
 * @brief A container object for a process's information.
 *
 * This object encapsulates information about a process, as well as being the
 * top level item in the heirarchy of debugging information objects.  Traverse
 * the list of threads from here (get the stack traces from there) and so on.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @todo Should this be movable instead of psystem::not_assignable?
 */
class process_info
    : psystem::not_assignable
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Type Definitions
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Mapping of thread IDs (TIDs) to the psystem::thread_info instance.
     * @todo Make private...
     */
    using thread_info_map =
        std::map< thread_id_t, std::unique_ptr<thread_info> >;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct a process_info instance.
     *
     * @param[in] pid
     *     The system process ID (PID) of the process being examined.
     * @param[in] info
     *     Process-specific data to assimilate into this instance.
     */
    process_info(
        process_id_t pid,
        CREATE_PROCESS_DEBUG_INFO const& info) noexcept;

    /// @brief Free resources from the process_info instance.
    ~process_info() noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Accessors
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Access the handle to the process for system debugging API.
     *
     * @return A native "handle" that will remain valid for the lifetime of this
     *         process_info instance.
     */
    HANDLE get_process_handle() const noexcept;

    /**
     * @brief The system process ID (PID) relating to this instance.
     * @return A process ID that maps directly to a running process.
     */
    process_id_t get_process_id() const noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Allow a psystem::thread_info instance to be managed by this
     *        process_info instance.
     *
     * @param[in] thread
     *     The thread parameter is a @c std::unique_ptr value to "sink"
     *     ownership (i.e., transfer) from the caller to this instance.  The
     *     parameter must not contain @c nullptr.
     * @return @c true if the thread was successfully added; @c false otherwise.
     */
    bool add_thread(std::unique_ptr<thread_info> thread) noexcept;

    /**
     * @brief Remove a thread from being managed by this process_info instance.
     *
     * If the thread ID is not present in the current set of managed threads,
     * this method does nothing.
     *
     * @param[in] tid
     *     The thread's ID will be used to find and remove the
     *     psystem::thread_info instance.
     * @todo Perhaps this should return the thread...
     */
    void remove_thread(thread_id_t tid) noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief A map of base address for a module to the module info object
     *        itself.
     *
     * This is sorted by address in order from largest to smallest to facilitate
     * quick lookup of a symbol address to the module that owns it.  An address
     * is believed to exist within a module if the module's address is the
     * largest address that is less than the symbol address.
     */
    std::map<
        psystem::address_t,
        psystem::module_info,
        std::greater<psystem::address_t> > m_modules;

    /// @brief A mapping of thread ID to the psystem::thread_info instance.
    /// @todo Wrap this more sensibly w/o exposing unique_ptr.
    thread_info_map m_threads;

private:
    /// @brief The system ID the corresponds to the process encapsulated here.
    process_id_t const m_process_id;

    /// @brief A system handle for debug API to access this process.
    HANDLE m_process_handle;
};

} // namespace psystem

#endif // PSYSTEM_PROCESS_INFO_HPP
