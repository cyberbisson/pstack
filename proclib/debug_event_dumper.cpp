
/** @file debug_event_dumper.cpp
 ** @brief Code to print debug events ad nausium.
 **
 ** @author  Matt Bisson
 ** @date    14 February, 2008
 ** @since   Proclib 1.2
 ** @version Proclib 1.3
 **/

#include <stdio.h>
#include "debug_event_dumper.hpp"

//////////////////////////////////////////////////////////////////////////////
// Event handlers:
//////////////////////////////////////////////////////////////////////////////

bool proclib::debug_event_dumper::OnCreateProcess (
    processId_t pid, threadId_t tid, const CREATE_PROCESS_DEBUG_INFO& data)
    throw ()
{
    printf ("DEBUG EVENT: OnCreateProcess\n");
    printCommonData (pid, tid);

    if ((NULL != data.lpImageName) && ('\0' != *((char *)data.lpImageName)))
    {
        printf (
#ifdef _UNICODE
            (0 == data.fUnicode)
#else
            (0 != data.fUnicode)
#endif
            ? "\tProcess Name:     %S\n"
            : "\tProcess Name:     %s\n",
            data.lpImageName);
    }
    else
    {
        printf ("\tProcess Name:     (NONE)\n");
    }

    fflush (stdout);
    return debug_event_listener::OnCreateProcess (pid, tid, data);
}

bool proclib::debug_event_dumper::OnCreateThread (
    processId_t pid, threadId_t tid, const CREATE_THREAD_DEBUG_INFO& data)
    throw ()
{
    printf ("DEBUG EVENT: OnCreateThread\n");
    printCommonData (pid, tid);

    fflush (stdout);
    return debug_event_listener::OnCreateThread (pid, tid, data);
}

bool proclib::debug_event_dumper::OnException (
    processId_t pid, threadId_t tid, const EXCEPTION_DEBUG_INFO& data)
    throw ()
{
    printf ("DEBUG EVENT: OnException\n");
    printCommonData (pid, tid);

    fflush (stdout);
    return debug_event_listener::OnException (pid, tid, data);
}

bool proclib::debug_event_dumper::OnExitProcess (
    processId_t pid, threadId_t tid, const EXIT_PROCESS_DEBUG_INFO& data)
    throw ()
{
    printf ("DEBUG EVENT: OnExitProcess\n");
    printCommonData (pid, tid);

    fflush (stdout);
    return debug_event_listener::OnExitProcess (pid, tid, data);
}

bool proclib::debug_event_dumper::OnExitThread (
    processId_t pid, threadId_t tid, const EXIT_THREAD_DEBUG_INFO& data)
    throw ()
{
    printf ("DEBUG EVENT: OnExitThread\n");
    printCommonData (pid, tid);

    fflush (stdout);
    return debug_event_listener::OnExitThread (pid, tid, data);
}

bool proclib::debug_event_dumper::OnLoadDll (
    processId_t pid, threadId_t tid, const LOAD_DLL_DEBUG_INFO& data)
    throw ()
{
    printf ("DEBUG EVENT: OnLoadDll\n");
    printCommonData (pid, tid);

    if ((NULL != data.lpImageName) && ('\0' != *((char *)data.lpImageName)))
    {
        printf (
#ifdef _UNICODE
            (0 == data.fUnicode)
#else
            (0 != data.fUnicode)
#endif
            ? "\tDLL Name:         %S\n"
            : "\tDLL Name:         %s\n",
            data.lpImageName);
    }
    else
    {
        printf ("\tDLL Name:         (NONE)\n");
    }

    fflush (stdout);
    return debug_event_listener::OnLoadDll (pid, tid, data);
}

bool proclib::debug_event_dumper::OnDebugString (
    processId_t pid, threadId_t tid, const OUTPUT_DEBUG_STRING_INFO& data)
    throw ()
{
    printf ("DEBUG EVENT: OnDebugString\n");
    printCommonData (pid, tid);

    fflush (stdout);
    return debug_event_listener::OnDebugString (pid, tid, data);
}

bool proclib::debug_event_dumper::OnDebuggerError (
    processId_t pid, threadId_t tid, const RIP_INFO& data)
    throw ()
{
    printf ("DEBUG EVENT: OnDebuggerError\n");
    printCommonData (pid, tid);

    fflush (stdout);
    return debug_event_listener::OnDebuggerError (pid, tid, data);
}

bool proclib::debug_event_dumper::OnUnloadDll (
    processId_t pid, threadId_t tid, const UNLOAD_DLL_DEBUG_INFO& data)
    throw ()
{
    printf ("DEBUG EVENT: OnUnloadDll\n");
    printCommonData (pid, tid);

    fflush (stdout);
    return debug_event_listener::OnUnloadDll (pid, tid, data);
}

bool proclib::debug_event_dumper::OnUnknownEvent (
    const DEBUG_EVENT& evt, processId_t pid, threadId_t tid)
    throw ()
{
    printf ("DEBUG EVENT: OnUnknownEvent (0x%08X)\n", evt.dwDebugEventCode);
    printCommonData (pid, tid);

    fflush (stdout);
    return debug_event_listener::OnUnknownEvent (evt, pid, tid);
}

//////////////////////////////////////////////////////////////////////////////
// Private utilities:
//////////////////////////////////////////////////////////////////////////////

/** @brief I don't want to repeat this code in every event handler...
 **
 ** @param[in] pid The debugged process ID.
 ** @param[in] tid The active thread ID.
 **/
void proclib::debug_event_dumper::printCommonData (
    processId_t pid, threadId_t tid)
    throw ()
{
    printf ("\tProcess ID:       %d\n", pid);
    printf ("\tActive thread ID: %d\n", tid);
}
