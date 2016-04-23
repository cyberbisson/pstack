// ===-- include/psystem/thread_info.hpp ------------------ -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare a container for a single thread of execution.
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
#ifndef PSYSTEM_THREAD_INFO_HPP
#define PSYSTEM_THREAD_INFO_HPP

#include "psystem_ns.hpp"
#include "stack_frame.hpp"

#include <mutex>
#include <vector>

#include <psystem/framework/memory.hpp>
#include <psystem/framework/not_assignable.hpp>

namespace psystem {

class dbghelp_symbol_engine;
class process_info;

/**
 * @brief A container for a single thread of execution in a process.
 *
 * This thread_info instance represents a thread of execution for a process
 * being debugged.  This may be a worker thread, or the main process thread
 * (i.e., the thread containing the system's invocation of @c main).
 *
 * One important piece of data contained in this class is the stack trace.  The
 * thread_info makes this data available via get_stack_trace().
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
class thread_info
    : public psystem::not_assignable
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Type Definitions
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Function pointer to a function that maps addresses to symbol
     *        information.
     *
     * This function type should have the same semantics as the Windows API,
     * @c SymFunctionTableAccess64.  Form more information, see the MSDN entry
     * for @c FunctionTableAccessProc64.
     */
    typedef void* (__stdcall *address_to_symbol_func)(HANDLE, DWORD64);

    /**
     * @brief Function pointer to a function that determines the module base
     *        address from any input address.
     *
     * This function type should have the same semantics as the Windows API,
     * @c SymGetModuleBase64.  Form more information, see the MSDN entry
     * for @c GetModuleBaseProc64.
     */
    typedef DWORD64(__stdcall *get_module_base_func)(HANDLE, DWORD64);

    /**
     * @brief The container for the entire stack trace.
     *
     * This type models the Random Access Container concept.  Stack frames
     * indeces are zero-based, with the most recently invoked function (i.e.,
     * the current frame) at 0.
     */
    using stack_trace = std::vector<stack_frame>;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct an instance of thread_info based on a process.
     *
     * This constructor creates the thread_info instance for the main thread of
     * exection for the process.
     *
     * @param[in] pid  The process's unique system ID (PID).
     * @param[in] tid  This thread's unique system ID (TID).
     * @param[in] info Information about the process and its main thread.
     */
    thread_info(
        process_id_t pid,
        thread_id_t tid,
        CREATE_PROCESS_DEBUG_INFO const& info) noexcept;

    /**
     * @brief Construct an instance of thread_info.
     *
     * This constructor deals with worker threads, and not the main thread for
     * the process.
     *
     * @param[in] pid  The process's unique system ID (PID).
     * @param[in] tid  This thread's unique system ID (TID).
     * @param[in] info Information about the thread.
     */
    thread_info(
        process_id_t pid,
        thread_id_t tid,
        CREATE_THREAD_DEBUG_INFO const& info) noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Accessors
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Access the ID of the process that owns this thread.
     * @return The owning process's system ID (PID).
     */
    process_id_t get_process_id() const noexcept;

    /**
     * @brief Retrieve the system handle to this thread.
     *
     * This handle may be used with system debugging and thread inspection
     * functions.
     *
     * @return This handle is valid until the thread_info instance is destroyed.
     */
    HANDLE get_thread_handle() const noexcept;

    /**
     * @brief Access the ID of this thread.
     * @return The unique system ID (TID) of this thread.
     */
    thread_id_t get_thread_id() const noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Retrieve a list of the stack frames that are active in the current
     *        thread.
     *
     * This method will attempt to generate a stack trace list on the first
     * invocation, and (unless the generation was unsuccessful) any subsequent
     * invocations will simply return that @c stack_trace.
     *
     * @param[in] proc
     *     The process owning this thread.  Passing the incorrect thread will
     *     lead to undefined behavior!
     * @param[in] address_to_symbol
     *     This parameter must match the @c PFUNCTION_TABLE_ACCESS_ROUTINE64
     *     type.  If this is @c nullptr, it will use the default
     *     @c SymFunctionTableAccess64 function.
     * @param[in] get_module_base
     *     This parameter must match the @c PGET_MODULE_BASE_ROUTINE64 type.
     *     If this is @c nullptr, it will use the default @c SymGetModuleBase64
     *     function.
     *
     * @return
     *     A listing of stack frames.  This is a reference that will be valid
     *     until this instance is destroyed.
     *
     * @throws psystem::exception::system_exception
     *     If there was an issue while traversing the stack frames.
     *
     * @todo
     *     Find a way to get rid of the @p proc parameter.  It's illogical.  We
     *     are a thread of execution in a process, so why do we need a process
     *     passed in?  What if they pass a different process?  What would that
     *     logically even mean??
     */
    stack_trace const& get_stack_trace(
        process_info const& proc,
        address_to_symbol_func address_to_symbol,
        get_module_base_func get_module_base) const;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Private Utilities
    ////////////////////////////////////////////////////////////////////////////
private:
    /**
     * @brief Generate a stack trace for this thread.
     *
     * This method is part of a lazy initialization for the
     * thread_info::m_stack_trace member variable.  It is expected that it be
     * called from the get_stack_trace() method.  It is not possible to return a
     * value from @c std::call_once, so we've designed it to return data via
     * parameters.
     *
     * If the parameter, @p out is @c nullptr, this method simply does nothing.
     *
     * @param[in]  process_handle A process handle for system debug API.
     * @param[in]  thread_handle  A thread handle for system debug API.
     * @param[in]  address_to_symbol
     *     Passed to @c StackWalk64.  Check MSDN for details.
     * @param[in]  get_module_base
     *     Passed to @c StackWalk64.  Check MSDN for details.
     * @param[out] out
     *     This parameter receives the output of the stack trace generation.
     *     It may not be @c nullptr.  The new @c stack_trace instance will only
     *     be assigned to @p out upon successful completion of the method.  If
     *     the method fails, the value remains what it was when passed into the
     *     function.
     *
     * @throws psystem::exception::system_exception
     *     If there was an issue while traversing the stack frames.
     */
    static void generate_stack_trace(
        HANDLE process_handle,
        HANDLE thread_handle,
        address_to_symbol_func address_to_symbol,
        get_module_base_func get_module_base,
        std::unique_ptr<stack_trace> *out);

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief The system debug API handle to this thread.
    HANDLE m_thread_handle;

    /// @brief An unique ID for a thread of execution in a process.
    thread_id_t const m_thread_id;

    /// @brief The unique ID of the process that owns this thread.
    process_id_t const m_process_id;

    /**
     * @brief A list of the stack frames executing in this thread.
     *
     * This is expensive to generate, and is only created lazily.
     */
    std::unique_ptr<stack_trace> mutable m_stack_trace;

    /**
     * @brief Mutex for lazy generation of thread_info::m_stack_trace.
     *
     * The thread_info::m_stack_trace data is generated by the @c const member
     * function, get_stack_trace().  Mutable data altered in a @c const context
     * should always be protected by a threading guard.
     */
    std::once_flag mutable m_stack_trace_lock;
};

} // namespace psystem

#endif // PSYSTEM_THREAD_INFO_HPP
