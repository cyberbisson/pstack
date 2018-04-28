// ===-- src/proclib/debug_engine.cpp --------------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define an "engine" to drive debugging events and contain require data.
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

#include "proclib/debug_engine.hpp"

#include <cstdio> /// @todo Remove when logging exists

#include <psystem/framework/memory.hpp>
#include <psystem/framework/shared_handle.hpp>
#include <psystem/framework/unique_handle.hpp>
#include <psystem/exception/system_exception.hpp>

#include <proclib/base_debug_event_listener.hpp>
#include <proclib/process_debugger.hpp>

namespace proclib {

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

debug_engine::debug_engine(bool kill_on_exit /*=false*/) noexcept
    : m_kill_on_exit(kill_on_exit)
{

}

debug_engine::~debug_engine() noexcept
{

}

////////////////////////////////////////////////////////////////////////////////
// Public Interface
////////////////////////////////////////////////////////////////////////////////

process_debugger&
debug_engine::attach_to_process(psystem::process_id_t const pid)
{
    // We should never get here (assuming that PIDs are tracked in a std::set).
    // It's OK if we have duplicate debuggers, though, so let's not kill release
    // builds because of our mistake.  We could throw an execption here if the
    // release build should be killed in this scenario...
    ASSERT(0 == m_debuggers.count(pid));

    auto debugger = std::make_unique<process_debugger>(pid);
    debugger->set_kill_process_on_exit(m_kill_on_exit);

    process_debugger *const ret = debugger.get();
    m_debuggers[pid] = std::move(debugger);

    ASSERT(ret);
    return *ret;
}

void
debug_engine::enable_debug_privilege() const
{
    psystem::unique_handle<> h_token;

    if (!OpenProcessToken(
            GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &h_token))
    {
        THROW_WINDOWS_EXCEPTION(
            GetLastError(), "Cannot get current process security information");
    }
    ASSERT(h_token);

    TOKEN_PRIVILEGES privs = { 0 };
    privs.PrivilegeCount = 1;

    if (!LookupPrivilegeValue(
            nullptr, SE_DEBUG_NAME, &privs.Privileges->Luid))
    {
        THROW_WINDOWS_EXCEPTION(
            GetLastError(), "Trouble getting current process prvileges");
    }

    privs.Privileges->Attributes = SE_PRIVILEGE_ENABLED;
    if (!AdjustTokenPrivileges(
            h_token.get(), FALSE, &privs, 0, nullptr, nullptr))
    {
        THROW_WINDOWS_EXCEPTION(
            GetLastError(), "Cannot change to \"debug\" privileges");
    }
}

process_debugger *
debug_engine::get_process_debugger(psystem::process_id_t const pid) const
{
    auto debugger_it = m_debuggers.find(pid);
    if ((debugger_it == m_debuggers.end()) || (!debugger_it->second))
    {
        return nullptr;
    }

    return debugger_it->second.get();
}

bool
debug_engine::wait_for_event(DWORD const ms /*=INFINITE*/) const
{
    DEBUG_EVENT debug_evt;

    if (!WaitForDebugEvent(&debug_evt, ms))
    {
        DWORD const error_code = GetLastError();

        /// @todo No way to determine time-out vs unhandled event
        if (ERROR_SEM_TIMEOUT == error_code) return false;

        THROW_WINDOWS_EXCEPTION(error_code, "Cannot retrieve debug events");
    }

    auto debugger_it = m_debuggers.find(debug_evt.dwProcessId);
    if (debugger_it == m_debuggers.end())
    {
        printf("Couldn't find a debugger with PID: %lu\n",
               debug_evt.dwProcessId);
        return false; /// @todo LOG
    }

    process_debugger *const debugger = debugger_it->second.get();
    if (!debugger)
    {
        printf("Debugger already cleared out for PID: %lu\n",
               debug_evt.dwProcessId);
        return false; /// @todo LOG
    }

    bool ret = false;

    // Need to call CloseHandle for some events -- MSDN:
    //
    // When a CREATE_PROCESS_DEBUG_EVENT occurs, the debugger application
    // receives a handle to the image file of the process being debugged,
    // a handle to the process being debugged, and a handle to the initial
    // thread of the process being debugged in the DEBUG_EVENT structure.  The
    // members these handles are returned in are u.CreateProcessInfo.hFile
    // (image file), u.CreateProcessInfo.hProcess (process), and
    // u.CreateProcessInfo.hThread  (initial thread). If the system previously
    // reported an EXIT_PROCESS_DEBUG_EVENT debugging event, the system closes
    // the handles to the process and thread when the debugger calls the
    // ContinueDebugEvent function. The debugger should close the handle to the
    // image file by calling the CloseHandle function.
    //
    // When a LOAD_DLL_DEBUG_EVENT occurs, the debugger application
    // receives a handle to the loaded DLL in the u.LoadDll.hFile member of the
    // DEBUG_EVENT structure. This handle should be closed by the debugger

    switch (debug_evt.dwDebugEventCode)
    {
    case CREATE_PROCESS_DEBUG_EVENT:
    {
        psystem::shared_handle<> file_handle(
            debug_evt.u.CreateProcessInfo.hFile);

        ret = debugger->get_events().create_process_event(
            debug_evt.dwProcessId, debug_evt.dwThreadId,
            file_handle,
            debug_evt.u.CreateProcessInfo);
        break;
    }

    case CREATE_THREAD_DEBUG_EVENT:
        ret = debugger->get_events().create_thread_event(
            debug_evt.dwProcessId, debug_evt.dwThreadId,
            debug_evt.u.CreateThread);
        break;
    case EXCEPTION_DEBUG_EVENT:
        ret = debugger->get_events().exception_event(
            debug_evt.dwProcessId, debug_evt.dwThreadId,
            debug_evt.u.Exception);
        break;
    case EXIT_PROCESS_DEBUG_EVENT:
        ret = debugger->get_events().exit_process_event(
            debug_evt.dwProcessId, debug_evt.dwThreadId,
            debug_evt.u.ExitProcess);
        break;
    case EXIT_THREAD_DEBUG_EVENT:
        ret = debugger->get_events().exit_thread_event(
            debug_evt.dwProcessId, debug_evt.dwThreadId,
            debug_evt.u.ExitThread);
        break;
    case LOAD_DLL_DEBUG_EVENT:
    {
        psystem::shared_handle<> file_handle(debug_evt.u.LoadDll.hFile);

        ret = debugger->get_events().load_dll_event(
            debug_evt.dwProcessId, debug_evt.dwThreadId,
            file_handle,
            debug_evt.u.LoadDll);
        break;
    }
    case OUTPUT_DEBUG_STRING_EVENT:
        ret = debugger->get_events().output_debug_string_event(
            debug_evt.dwProcessId, debug_evt.dwThreadId,
            debug_evt.u.DebugString);
        break;
    case RIP_EVENT:
        ret = debugger->get_events().rip_event(
            debug_evt.dwProcessId, debug_evt.dwThreadId,
            debug_evt.u.RipInfo);
        break;
    case UNLOAD_DLL_DEBUG_EVENT:
        ret = debugger->get_events().unload_dll_event(
            debug_evt.dwProcessId, debug_evt.dwThreadId,
            debug_evt.u.UnloadDll);
        break;

        // Most compilers don't warn if you missed a case when default is
        // present.
#ifndef _DEBUG
    default:
        printf("--> Got UNKNOWN event\n");
        break; /// @todo LOG
#endif
    }

    // Continuing right away.  If we want to implement a full debugger, we
    // should split this away from the WaitForDebugEvent call.
    //
    // DBG_EXCEPTION_NOT_HANDLED means that if the last event was
    // EXCEPTION_DEBUG_EVENT, continuing this debugged thread must allow the
    // original exception processing to continue.  Oddly, DBG_CONTINUE
    // essentially IGNORES the exception, so we don't use it.  If we have
    // not seen an EXCEPTION_DEBUG_EVENT, either value continues execution
    // identically.
    if (!ContinueDebugEvent(
            debug_evt.dwProcessId,
            debug_evt.dwThreadId,
            DBG_EXCEPTION_NOT_HANDLED))
    {
        THROW_WINDOWS_EXCEPTION_F(
            GetLastError(),
            "Cannot continue process %d", debug_evt.dwProcessId);
    }

    return ret;
}

} // namespace proclib
