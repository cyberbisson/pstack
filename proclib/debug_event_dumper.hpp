
/** @file debug_event_dumper.hpp
 ** @brief Defines an object to output debug events to stdout.
 **
 ** @author  Matt Bisson
 ** @date    14 February, 2008
 ** @since   Proclib 1.2
 ** @version Proclib 1.3
 **/

#ifndef __DEBUG_EVENT_DUMPER_HPP__
#define __DEBUG_EVENT_DUMPER_HPP__

#include "proclib_ns.hpp"
#include "debug_event_listener.hpp"

namespace proclib
{

/** @brief As we see events, dump them to stdout.
 **
 ** @todo I need to actaully finish this.  It's like 10% implemented...
 ** @todo Integrate this into debug_module so it appears at the top of the
 **     queue, and so it's not so "out there."
 ** @author  Matt Bisson
 ** @date    14 February, 2008
 ** @since   Proclib 1.2
 ** @version Proclib 1.3
 **/
class debug_event_dumper : public proclib::debug_event_listener
{
    //////////////////////////////////////////////////////////////////////////
    /// @name Overlaoded Event Handlers
    //////////////////////////////////////////////////////////////////////////
protected:
    virtual bool OnCreateProcess (
        processId_t pid, threadId_t tid, const CREATE_PROCESS_DEBUG_INFO& data)
        throw ();
    virtual bool OnCreateThread (
        processId_t pid, threadId_t tid, const CREATE_THREAD_DEBUG_INFO& data)
        throw ();
    virtual bool OnDebuggerError (
        processId_t pid, threadId_t tid, const RIP_INFO& data)
        throw ();
    virtual bool OnDebugString (
        processId_t pid, threadId_t tid, const OUTPUT_DEBUG_STRING_INFO& data)
        throw ();
    virtual bool OnException (
        processId_t pid, threadId_t tid, const EXCEPTION_DEBUG_INFO& data)
        throw ();
    virtual bool OnExitProcess (
        processId_t pid, threadId_t tid, const EXIT_PROCESS_DEBUG_INFO& data)
        throw ();
    virtual bool OnExitThread (
        processId_t pid, threadId_t tid, const EXIT_THREAD_DEBUG_INFO& data)
        throw ();
    virtual bool OnLoadDll (
        processId_t pid, threadId_t tid, const LOAD_DLL_DEBUG_INFO& data)
        throw ();
    virtual bool OnUnknownEvent (
        const DEBUG_EVENT& evt, processId_t pid, threadId_t tid)
        throw ();
    virtual bool OnUnloadDll (
        processId_t pid, threadId_t tid, const UNLOAD_DLL_DEBUG_INFO& data)
        throw ();

    //////////////////////////////////////////////////////////////////////////
    /// @name Private Utilities
    //////////////////////////////////////////////////////////////////////////
private:
    void printCommonData (processId_t pid, threadId_t tid) throw ();
};

}

#endif // __DEBUG_EVENT_DUMPER_HPP__
