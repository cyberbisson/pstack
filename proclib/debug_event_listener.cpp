
/** @file debug_event_listener.cpp
 ** @brief Code for the debug event interface.
 **
 ** @author  Matt Bisson
 ** @date    14 February, 2008
 ** @since   Proclib 1.2
 ** @version Proclib 1.3
 **/

#include <windows.h>
#include <exception>

#include <exception/internal_exception.hpp>

#include "debug_event_listener.hpp"

//////////////////////////////////////////////////////////////////////////////
// Utility:
//////////////////////////////////////////////////////////////////////////////

/** @brief Contstruct an event listener.
 **/
proclib::debug_event_listener::debug_event_listener () throw (std::exception)
{
    // Wow!
}

/** @brief Destroy this event listener.
 **/
proclib::debug_event_listener::~debug_event_listener () throw (std::exception)
{
    // There's a whole lot o' shakin' going on.
}

/** @brief Given a debug event, dispatch to the correct callback.
 **
 ** @param[in] evt The OS-specific debug event.
 ** @return True, if we handled the event, false if not.
 ** @throws std::exception Events are allowed to throw any derivative of
 **     std::exception.
 **/
bool proclib::debug_event_listener::dispatchEvent (const DEBUG_EVENT& evt)
    throw (std::exception)
{
    try
    {
        switch (evt.dwDebugEventCode)
        {
        case CREATE_PROCESS_DEBUG_EVENT:
            return OnCreateProcess (
                evt.dwProcessId, evt.dwThreadId, evt.u.CreateProcessInfo);
        case CREATE_THREAD_DEBUG_EVENT:
            return OnCreateThread (
                evt.dwProcessId, evt.dwThreadId, evt.u.CreateThread);
        case EXCEPTION_DEBUG_EVENT:
            return OnException (
                evt.dwProcessId, evt.dwThreadId, evt.u.Exception);
        case EXIT_PROCESS_DEBUG_EVENT:
            return OnExitProcess (
                evt.dwProcessId, evt.dwThreadId, evt.u.ExitProcess);
        case EXIT_THREAD_DEBUG_EVENT:
            return OnExitThread (
                evt.dwProcessId, evt.dwThreadId, evt.u.ExitThread);
        case LOAD_DLL_DEBUG_EVENT:
            return OnLoadDll (
                evt.dwProcessId, evt.dwThreadId, evt.u.LoadDll);
        case OUTPUT_DEBUG_STRING_EVENT:
            return OnDebugString (
                evt.dwProcessId, evt.dwThreadId, evt.u.DebugString);
        case RIP_EVENT:
            return OnDebuggerError (
                evt.dwProcessId, evt.dwThreadId, evt.u.RipInfo);
        case UNLOAD_DLL_DEBUG_EVENT:
            return OnUnloadDll (
                evt.dwProcessId, evt.dwThreadId, evt.u.UnloadDll);
        default:
            return OnUnknownEvent (evt, evt.dwProcessId, evt.dwThreadId);
        }
    }
    catch (std::exception&)
    {
        throw;
    }
    catch (...)
    {
        // WTF?  Repackage this.
        throw psystem::exception::internal_exception (
            __FILE__, __FUNCTION__, __LINE__,
            "Unknown exception while receiving a debugger event");
    }
}

//////////////////////////////////////////////////////////////////////////////
// Event callbacks:
//////////////////////////////////////////////////////////////////////////////

/** @brief Called when the debugger attaches to or creates a process.
 **
 ** @param[in] pid The process ID of the debugged application.
 ** @param[in] tid The active thread ID that sent this event.
 ** @param[in] data Event-specific data.
 ** @return True, if we handled the event, false if not.
 ** @throws std::exception All handlers may throw any derivative of
 **     std::exception.
 **/
bool proclib::debug_event_listener::OnCreateProcess (
    processId_t pid, threadId_t tid, const CREATE_PROCESS_DEBUG_INFO& data)
    throw (std::exception)
{
    return false;
}

/** @brief Called when the debugged process starts a new thread execution.
 **
 ** @param[in] pid The process ID of the debugged application.
 ** @param[in] tid The active thread ID that sent this event.
 ** @param[in] data Event-specific data.
 ** @return True, if we handled the event, false if not.
 ** @throws std::exception All handlers may throw any derivative of
 **     std::exception.
 **/
bool proclib::debug_event_listener::OnCreateThread (
    processId_t pid, threadId_t tid, const CREATE_THREAD_DEBUG_INFO& data)
    throw (std::exception)
{
    return false;
}

/** @brief Called when the debugger encounters a fatal error.
 **
 ** @todo Excecution of the debugger cannot continue after this error?  What do
 **     I need to do here?
 **
 ** @param[in] pid The process ID of the debugged application.
 ** @param[in] tid The active thread ID that sent this event.
 ** @param[in] data Event-specific data.
 ** @return True, if we handled the event, false if not.
 ** @throws std::exception All handlers may throw any derivative of
 **     std::exception.
 **/
bool proclib::debug_event_listener::OnDebuggerError (
    processId_t pid, threadId_t tid, const RIP_INFO& data)
    throw (std::exception)
{
    return false;
}

/** @brief Called when the application being debugged sends data back to the
 **        debugger.
 **
 ** @param[in] pid The process ID of the debugged application.
 ** @param[in] tid The active thread ID that sent this event.
 ** @param[in] data Event-specific data.
 ** @return True, if we handled the event, false if not.
 ** @throws std::exception All handlers may throw any derivative of
 **     std::exception.
 **/
bool proclib::debug_event_listener::OnDebugString (
    processId_t pid, threadId_t tid, const OUTPUT_DEBUG_STRING_INFO& data)
    throw (std::exception)
{
    return false;
}

/** @brief Called when a Windows SEH error occurs (see below for an exception).
 **
 ** The OnException() function gets called when a Windows Structured Exception
 ** Handling (SEH) error is encountered.  There is one notable exception here --
 ** if the debugged application runs on the console, and the user presses
 ** Ctrl-C, OnException() gets called.  This is so you can interactively stop a
 ** running application and break into the debugger. 
 **
 ** @param[in] pid The process ID of the debugged application.
 ** @param[in] tid The active thread ID that sent this event.
 ** @param[in] data Event-specific data.
 ** @return True, if we handled the event, false if not.
 ** @throws std::exception All handlers may throw any derivative of
 **     std::exception.
 **/
bool proclib::debug_event_listener::OnException (
    processId_t pid, threadId_t tid, const EXCEPTION_DEBUG_INFO& data)
    throw (std::exception)
{
    return false;
}

/** @brief Called during termination of the debugged process.
 **
 ** @param[in] pid The process ID of the debugged application.
 ** @param[in] tid The active thread ID that sent this event.
 ** @param[in] data Event-specific data.
 ** @return True, if we handled the event, false if not.
 ** @throws std::exception All handlers may throw any derivative of
 **     std::exception.
 **/
bool proclib::debug_event_listener::OnExitProcess (
    processId_t pid, threadId_t tid, const EXIT_PROCESS_DEBUG_INFO& data)
    throw (std::exception)
{
    return false;
}

/** @brief Called when a thread is about to be removed.
 **
 ** @param[in] pid The process ID of the debugged application.
 ** @param[in] tid The active thread ID that sent this event.
 ** @param[in] data Event-specific data.
 ** @return True, if we handled the event, false if not.
 ** @throws std::exception All handlers may throw any derivative of
 **     std::exception.
 **/
bool proclib::debug_event_listener::OnExitThread (
    processId_t pid, threadId_t tid, const EXIT_THREAD_DEBUG_INFO& data)
    throw (std::exception)
{
    return false;
}

/** @brief Called when the process loads an external module.
 **
 ** @param[in] pid The process ID of the debugged application.
 ** @param[in] tid The active thread ID that sent this event.
 ** @param[in] data Event-specific data.
 ** @return True, if we handled the event, false if not.
 ** @throws std::exception All handlers may throw any derivative of
 **     std::exception.
 **/
bool proclib::debug_event_listener::OnLoadDll (
    processId_t pid, threadId_t tid, const LOAD_DLL_DEBUG_INFO& data)
    throw (std::exception)
{
    return false;
}

/** @brief Called when an event comes from the debugger that dispatchEvent()
 **        does not understand.
 **
 ** @param[in] evt The original event object (for more specific uses).
 ** @param[in] pid The process ID of the debugged application.
 ** @param[in] tid The active thread ID that sent this event.
 ** @return True, if we handled the event, false if not.
 ** @throws std::exception All handlers may throw any derivative of
 **     std::exception.
 **/
bool proclib::debug_event_listener::OnUnknownEvent (
    const DEBUG_EVENT& evt, processId_t pid, threadId_t tid)
    throw (std::exception)
{
    return false;
}

/** @brief Called when the last open handle to a DLL gets released.
 **
 ** @param[in] pid The process ID of the debugged application.
 ** @param[in] tid The active thread ID that sent this event.
 ** @param[in] data Event-specific data.
 ** @return True, if we handled the event, false if not.
 ** @throws std::exception All handlers may throw any derivative of
 **     std::exception.
 **/
bool proclib::debug_event_listener::OnUnloadDll (
    processId_t pid, threadId_t tid, const UNLOAD_DLL_DEBUG_INFO& data)
    throw (std::exception)
{
    return false;
}
