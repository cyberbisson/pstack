
/** @file thread_trace.cpp
 ** @brief Define the logical objects needed to examine a thread.
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.0
 ** @version Proclib 1.3
 **/

#include <windows.h>

#include <dbghelp.hpp>
#include <process_trace.hpp>
#include <stack_frame.hpp>

#include <exception/windows_exception.hpp>

#include "thread_trace.hpp"

using proclib::thread_trace;

//////////////////////////////////////////////////////////////////////////////
/// @name Module-specific Functions
//////////////////////////////////////////////////////////////////////////////

/** @brief A callback for StackWalk64 that loads the DbgHelp DLL only on demand.
 **/
static DWORD64 WINAPI cbSymGetModuleBase (HANDLE hProcess, DWORD64 addr)
{
    return psystem::dbghelp::SymGetModuleBase64 (hProcess, addr);
}

/** @brief A callback for StackWalk64 that loads the DbgHelp DLL only on demand.
 **/
static PVOID WINAPI cbSymFunctionTableAccess (HANDLE hProcess, DWORD64 addr)
{
    return psystem::dbghelp::SymFunctionTableAccess64 (hProcess, addr);
}

/// @}

//////////////////////////////////////////////////////////////////////////////
// Public interface:
//////////////////////////////////////////////////////////////////////////////

/** This will call the constructor and initialize the thread_trace object.
 ** @brief Allocate memory and construct a thread_trace object.
 **
 ** @param[in] parent   The process that owns this thread.
 ** @param[in] threadId The systems identification for this thread.
 ** @return A newly allocated thread_trace object.
 ** @throws windows_exception If basic construction operations fail.
 ** @throws bad_alloc If we could not allocate the memory.
 **/
thread_trace *proclib::thread_trace::Create (
    proclib::process_trace& parent, threadId_t threadId)
    throw (psystem::exception::windows_exception, std::bad_alloc)
{
    proclib::thread_trace *ret = new thread_trace (parent, threadId);

    if (NULL == ret) throw std::bad_alloc ();

    return ret;
}

/** If the thread_trace is not initialized, it will happen here.
 ** @brief This is a simple accessor for stack information for this thread.
 **
 ** @throws windows_exception If the object tried to initialize and failed.
 **/
const thread_trace::stackFrames_t& proclib::thread_trace::getStack ()
    throw (psystem::exception::windows_exception)
{
    init_or_throw ();
    return m_StackFrames;
}

//////////////////////////////////////////////////////////////////////////////
// Construction / destruction:
//////////////////////////////////////////////////////////////////////////////

/** The constructor always initializes the thread's handle.
 ** @brief Initialize a thread with it's ID and parent.
 **
 ** @param[in] parent   The process that owns this thread.
 ** @param[in] threadId The systems identification for this thread.
 ** @throws windows_exception If we could net find a handle for the given
 **                           thread ID.
 **/
proclib::thread_trace::thread_trace (
    proclib::process_trace& parent, threadId_t threadId)
    throw (psystem::exception::windows_exception)
    : trace_object (INVALID_HANDLE_VALUE),
      m_ThreadId (threadId),
      m_Parent   (parent)
{
    hThread_t hThread = OpenThread (THREAD_GET_CONTEXT, FALSE, threadId);
    if (NULL == hThread)
    {
        THROW_WINDOWS_EXCEPTION_F(
            GetLastError(), "Cannot open thread (ID: %d)", threadId);
    }

    setHandle (hThread);
}

//////////////////////////////////////////////////////////////////////////////
// Inherited object interface:
//////////////////////////////////////////////////////////////////////////////

void proclib::thread_trace::dump_object ()
    throw ()
{
    /** @todo Dump the thread object. */
}

/** @brief populate the stack_frame list for this thread. 
 **
 ** This function will find the initial stack context, then allocate and assign
 ** value to the stack_frame objects.
 ** @throws windows_exception If the OS-native stack walking function failed.
 **/
void proclib::thread_trace::init ()
    throw (psystem::exception::windows_exception)
{
    if (!m_StackFrames.empty ()) return;

    STACKFRAME64 tmpstack = initStackContext ();

    while (psystem::dbghelp::StackWalk64 (
               m_Parent.getMachineType (),
               m_Parent.getProcessHandle (),
               getThreadHandle (),
               &tmpstack,
               NULL, // &context,
               NULL, // ReadMemoryRoutine
               cbSymFunctionTableAccess,
               cbSymGetModuleBase,
               NULL // TranslateAddress
               ))
    {
        /** @todo Watch for exceptions / memory leaks */
        stack_frame *sf = stack_frame::Create (*this, tmpstack);
        m_StackFrames.push_back (sf);
    }

    // If the function return wasn't 0 and we exited the loop, StackWalk
    // encountered an error.
    if (NULL != tmpstack.AddrReturn.Offset)
    {
        THROW_WINDOWS_EXCEPTION_F(GetLastError(), "Error traversing stack");
    }
}

void proclib::thread_trace::shutdown ()
    throw ()
{
    /** @todo Clean up after thread_trace */
}

//////////////////////////////////////////////////////////////////////////////
// Private member functions:
//////////////////////////////////////////////////////////////////////////////

/** @brief Prepare our internal state for use in StackWalk().
 **
 ** @throws windows_exception If we cannot determine the execution context of
 **     this thread.
 **/
STACKFRAME64 proclib::thread_trace::initStackContext () const
    throw (psystem::exception::windows_exception)
{
    CONTEXT      context;
    STACKFRAME64 sf;

    memset (&context, 0, sizeof (context));
    context.ContextFlags = CONTEXT_FULL;

    /** @note The big secret is that if you don't initialize the STACKFRAME64
     **       object with thread context, StackWalk64 will fail. */

    if (!GetThreadContext (getThreadHandle (), &context))
    {
        THROW_WINDOWS_EXCEPTION(GetLastError (), "Can't get thread context");
    }

    memset (&sf, 0, sizeof (sf));
    sf.AddrPC.Offset    = context.Eip;
    sf.AddrPC.Mode      = AddrModeFlat;
    sf.AddrStack.Offset = context.Esp;
    sf.AddrStack.Mode   = AddrModeFlat;
    sf.AddrFrame.Offset = context.Ebp;
    sf.AddrFrame.Mode   = AddrModeFlat;

    return sf;
}
