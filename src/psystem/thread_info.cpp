// ===-- src/psystem/thread_info.cpp ---------------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define a container for a single thread of execution.
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

#include "psystem/thread_info.hpp"

#include <psystem/dll/debug_help_dll.hpp>

#include <psystem/dbghelp_symbol_engine.hpp>
#include <psystem/process_info.hpp>
#include <psystem/exception/system_exception.hpp>

////////////////////////////////////////////////////////////////////////////////
/// @name Static Utilities
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialize a stack frame object for the first call into @c StackWalk64.
 *
 * @param[in] context A thread context to initialize data in the resulting
 *                    @c STACKFRAME64 object.
 * @return A newly initialzed @c STACKFRAME64 object.
 */
static STACKFRAME64
create_initial_stack_frame(CONTEXT const& /*context*/) noexcept
{
    STACKFRAME64 sf = { 0 };
#if 0
    // This is old code that is specific to 32-bit x86.
    sf.AddrPC.Offset    = context.Eip;
    sf.AddrPC.Mode      = AddrModeFlat;
    sf.AddrStack.Offset = context.Esp;
    sf.AddrStack.Mode   = AddrModeFlat;
    sf.AddrFrame.Offset = context.Ebp;
    sf.AddrFrame.Mode   = AddrModeFlat;
#endif
    return sf;
}

/**
 * @brief Create and initialize a stack frame context based on the thread
 *        context.
 *
 * @param[in] thread_handle This method requires a valid thread handle to get
 *                          the thread context.
 * @return A newly initialized frame context.
 *
 * @throws psystem::exception::system_exception
 *     If there was some error getting the current thread context.
 */
static CONTEXT
get_initial_thread_context(HANDLE thread_handle)
{
    CONTEXT context = { 0 };
    context.ContextFlags = CONTEXT_FULL;

    if (!GetThreadContext(thread_handle, &context))
    {
        THROW_WINDOWS_EXCEPTION(GetLastError(), "Can't get thread context");
    }

    return context;
}

/// @}

namespace psystem {

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

thread_info::thread_info(
    process_id_t pid, thread_id_t tid, CREATE_PROCESS_DEBUG_INFO const& info)
    noexcept
    : m_thread_handle(info.hThread),
      m_thread_id(tid),
      m_process_id(pid)
{

}

thread_info::thread_info(
    process_id_t pid, thread_id_t tid, CREATE_THREAD_DEBUG_INFO const& info)
    noexcept
    : m_thread_handle(info.hThread),
      m_thread_id(tid),
      m_process_id(pid)
{

}

////////////////////////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////////////////////////

process_id_t
thread_info::get_process_id() const noexcept
{
    return m_process_id;
}

thread_id_t
thread_info::get_thread_id() const noexcept
{
    return m_thread_id;
}

HANDLE
thread_info::get_thread_handle() const noexcept
{
    return m_thread_handle;
}

////////////////////////////////////////////////////////////////////////////////
// Public Interface
////////////////////////////////////////////////////////////////////////////////

thread_info::stack_trace const&
thread_info::get_stack_trace(
    process_info const& proc,
    address_to_symbol_func const address_to_symbol,
    get_module_base_func const get_module_base) const
{
    // This might be a little more heavy-weight than a double-checked lock
    // idiom, but it's hopefully more efficient then simply locking the whole
    // method.  It's certainly more correct than an old fashined double-checked
    // lock...
    std::call_once(
        m_stack_trace_lock,
        &generate_stack_trace,
        proc.get_process_handle(),
        m_thread_handle,
        ((address_to_symbol)
         ? address_to_symbol
         : DbgHelp.SymFunctionTableAccess64.get_ptr()),
        ((get_module_base)
         ? get_module_base
         : DbgHelp.SymGetModuleBase64.get_ptr()),
        &m_stack_trace);

    ASSERT(m_stack_trace);
    return *m_stack_trace;
}

////////////////////////////////////////////////////////////////////////////////
// Private Utilities
////////////////////////////////////////////////////////////////////////////////

/*static*/ void
thread_info::generate_stack_trace(
    HANDLE process_handle,
    HANDLE thread_handle,
    address_to_symbol_func const address_to_symbol,
    get_module_base_func const get_module_base,
    std::unique_ptr<stack_trace> *const out)
{
    // The caller doesn't care about the return data, so don't even bother...
    if (!out) return;

    CONTEXT context = get_initial_thread_context(thread_handle);
    STACKFRAME64 sf = create_initial_stack_frame(context);

    auto ret = std::make_unique<stack_trace>();

    do
    {
        if (!DbgHelp.StackWalk64(
                IMAGE_FILE_MACHINE_AMD64, /// @todo Better way to get this
                process_handle,
                thread_handle,
                &sf,
                &context,
                nullptr, /* ReadProcessMemoryProc64 */
                address_to_symbol,
                get_module_base,
                nullptr /* TranslateAddressProc64 */))
        {
            THROW_WINDOWS_EXCEPTION(GetLastError(), "Stack-walk failed");
        }

        ret->emplace_back(sf);
    }
    while (sf.AddrReturn.Offset);

    *out = std::move(ret);
}

} // namespace psystem
