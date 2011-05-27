
/** @file debug_event_listener.hpp
 ** @brief Provide an interface for getting debug events.
 **
 ** @author  Matt Bisson
 ** @date    14 February, 2008
 ** @since   Proclib 1.2
 ** @version Proclib 1.3
 **/

#ifndef __DEBUG_EVENT_LISTENER_HPP__
#define __DEBUG_EVENT_LISTENER_HPP__

#include <windows.h>
#include <exception>

#include "proclib_ns.hpp"

namespace proclib
{

/** @brief The basic "callback" interface to debugger events.
 **
 ** This "interface" actually defines all the callback methods.  In most cases,
 ** the default behavior does nothing and returns false, but to be safe, you
 ** should remember to call the base method from your handler.
 **
 ** In order to use the debugger that fired an event (from within the context
 ** of a callback function), use proclib::debug_module::GetDebugger().
 **
 ** @author  Matt Bisson
 ** @date    8 February, 2008
 ** @since   Proclib 1.2
 ** @version Proclib 1.3
 **/
class debug_event_listener
{
    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////
public:
    virtual ~debug_event_listener () throw (std::exception);
protected:
    explicit debug_event_listener () throw (std::exception);

    //////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    //////////////////////////////////////////////////////////////////////////
public:
    bool dispatchEvent (const DEBUG_EVENT& evt) throw (std::exception);

    //////////////////////////////////////////////////////////////////////////
    /// @name Event Callbacks
    //////////////////////////////////////////////////////////////////////////
protected:
    virtual bool OnCreateProcess (
        processId_t pid, threadId_t tid, const CREATE_PROCESS_DEBUG_INFO& data)
        throw (std::exception);
    virtual bool OnCreateThread (
        processId_t pid, threadId_t tid, const CREATE_THREAD_DEBUG_INFO& data)
        throw (std::exception);
    virtual bool OnDebuggerError (
        processId_t pid, threadId_t tid, const RIP_INFO& data)
        throw (std::exception);
    virtual bool OnDebugString (
        processId_t pid, threadId_t tid, const OUTPUT_DEBUG_STRING_INFO& data)
        throw (std::exception);
    virtual bool OnException (
        processId_t pid, threadId_t tid, const EXCEPTION_DEBUG_INFO& data)
        throw (std::exception);
    virtual bool OnExitProcess (
        processId_t pid, threadId_t tid, const EXIT_PROCESS_DEBUG_INFO& data)
        throw (std::exception);
    virtual bool OnExitThread (
        processId_t pid, threadId_t tid, const EXIT_THREAD_DEBUG_INFO& data)
        throw (std::exception);
    virtual bool OnLoadDll (
        processId_t pid, threadId_t tid, const LOAD_DLL_DEBUG_INFO& data)
        throw (std::exception);
    virtual bool OnUnknownEvent (
        const DEBUG_EVENT& evt, processId_t pid, threadId_t tid)
        throw (std::exception);
    virtual bool OnUnloadDll (
        processId_t pid, threadId_t tid, const UNLOAD_DLL_DEBUG_INFO& data)
        throw (std::exception);
};

}
#endif // __DEBUG_EVENT_LISTENER_HPP__
