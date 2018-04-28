// ===-- src/proclib/process_debugger.hpp ----------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare the debugger-to-process relationship.
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

#include "proclib/process_debugger.hpp"

#include <iostream> /// @todo Remove iostream when we have a logging facility.
#include <vector>

#include <psystem/framework/memory.hpp>
#include <psystem/exception/system_exception.hpp>

#include <proclib/base_debug_event_listener.hpp>

namespace proclib {

////////////////////////////////////
// --- dispatching_event_listener ---
////////////////////////////////////

/**
 * @brief The implementation class for passing debugger events to registered
 *        listeners.
 *
 * @note This class is marked @c final because it is an internal component,
 *       although being local to a single compilation unit, this is likely an
 *       acedemic issue only.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   ProcLib 2.0
 * @version ProcLib 2.0
 */
struct process_debugger::dispatching_event_listener final
    : public base_debug_event_listener
{
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

    /**
     * @brief Client-registered custom debug event listeners.
     * @see proclib::process_debugger::add_event_listener()
     * @see proclib::process_debugger::remove_event_listener()
     */
    std::vector< std::shared_ptr<base_debug_event_listener> > m_listeners;
};

/**
 * @brief Macro for proclib::process_debugger::dispatching_event_listener method
 *        bodies.
 *
 * Most of the methods are identical, and dispatch incoming events to listeners
 * using the same function name.  This macro is merely for unifying boilerplate
 * code.
 *
 * @param listener_func The name of the function to dispatch.
 * @param info_type     Data-type for event-specific details (e.g, @c RIP_INFO).
 * @todo We could probably do this with templates.
 */
#define DEFINE_DISPATCH_EVENT_FUNC(listener_func, info_type)            \
    bool                                                                \
    process_debugger::dispatching_event_listener:: listener_func(       \
        psystem::process_id_t const pid,                                \
        psystem::thread_id_t const tid,                                 \
        info_type const& info)                                          \
        noexcept                                                        \
    {                                                                   \
        bool ret = false;                                               \
                                                                        \
        for (std::shared_ptr<base_debug_event_listener>& listener: m_listeners)\
        {                                                               \
            ASSERT(listener);                                           \
            ret = listener->listener_func(pid, tid, info) || ret;       \
        }                                                               \
                                                                        \
        return ret;                                                     \
    }

/// @cond DOXYGEN_IGNORE
DEFINE_DISPATCH_EVENT_FUNC(create_thread_event, CREATE_THREAD_DEBUG_INFO);
DEFINE_DISPATCH_EVENT_FUNC(exception_event, EXCEPTION_DEBUG_INFO);
DEFINE_DISPATCH_EVENT_FUNC(exit_process_event, EXIT_PROCESS_DEBUG_INFO);
DEFINE_DISPATCH_EVENT_FUNC(exit_thread_event, EXIT_THREAD_DEBUG_INFO);
DEFINE_DISPATCH_EVENT_FUNC(output_debug_string_event, OUTPUT_DEBUG_STRING_INFO);
DEFINE_DISPATCH_EVENT_FUNC(rip_event, RIP_INFO);
DEFINE_DISPATCH_EVENT_FUNC(unload_dll_event, UNLOAD_DLL_DEBUG_INFO);

// Special case: extra shared_handle parameter
bool
process_debugger::dispatching_event_listener::create_process_event(
    psystem::process_id_t const pid, psystem::thread_id_t const tid,
    psystem::shared_handle<> const& file_handle,
    CREATE_PROCESS_DEBUG_INFO const& info)
    noexcept
{
    bool ret = false;

    for (std::shared_ptr<base_debug_event_listener>& listener : m_listeners)
    {
        ASSERT(listener);
        ret = listener->create_process_event(pid, tid, file_handle, info)
            || ret;
    }

    return ret;
}

// Special case: extra shared_handle parameter
bool
process_debugger::dispatching_event_listener::load_dll_event(
    psystem::process_id_t const pid, psystem::thread_id_t const tid,
    psystem::shared_handle<> const& file_handle,
    LOAD_DLL_DEBUG_INFO const& info)
    noexcept
{
    bool ret = false;

    for (std::shared_ptr<base_debug_event_listener>& listener : m_listeners)
    {
        ASSERT(listener);
        ret = listener->load_dll_event(pid, tid, file_handle, info) || ret;
    }

    return ret;
}
/// @endcond

////////////////////////////////////
// --- process_debugger ---
////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

process_debugger::process_debugger(psystem::process_id_t const pid)
    : m_process_id(pid),
      m_events(std::make_unique<dispatching_event_listener>())
{
    if (!DebugActiveProcess(m_process_id))
    {
        THROW_WINDOWS_EXCEPTION_F(
            GetLastError(), "Cannot attach to process %d", m_process_id);
    }
}

process_debugger::~process_debugger()
try
{
    if (!DebugActiveProcessStop(m_process_id))
    {
        THROW_WINDOWS_EXCEPTION_F(
            GetLastError(), "Cannot detach from process %d", m_process_id);
    }
}
catch (psystem::exception::system_exception const& sex)
{
    // Since we shouldn't throw from the destructor, let's just log and move on.
    // We can't really do much if this fails anyway.

    /// @todo Replace this when we have a logging facility.
    std::cerr << "pstack: " << sex << '\n';
}

////////////////////////////////////////////////////////////////////////////////
// Public Interface
////////////////////////////////////////////////////////////////////////////////

base_debug_event_listener&
process_debugger::get_events() const noexcept
{
    ASSERT(m_events);
    return *m_events;
}

psystem::process_id_t
process_debugger::get_process_id() const noexcept
{
    return m_process_id;
}

process_debugger::event_listener_id_t
process_debugger::add_event_listener(
    std::shared_ptr<base_debug_event_listener> const& l)
{
    ASSERT(l);

    /// @todo Deal with double-adds of the same listener
    m_events->m_listeners.emplace_back(l);
    return 0; /// @todo Need a way to generate an ID
}

bool
process_debugger::remove_event_listener(event_listener_id_t const /*id*/)
{
    /// @todo This is not implemented!
    return false;
}

void
process_debugger::set_kill_process_on_exit(bool const kill_on_exit)
{
    if (!DebugSetProcessKillOnExit(kill_on_exit ? TRUE : FALSE))
    {
        THROW_WINDOWS_EXCEPTION(GetLastError(), "This won't end well...");
    }
}

} // namespace proclib
