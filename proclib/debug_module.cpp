
/** @file debug_module.cpp
 ** @brief Defines functionality for inter-process debugging.
 **
 ** @author  Matt Bisson
 ** @date    8 February, 2008
 ** @since   Proclib 1.2
 ** @version Proclib 1.3
 **/

#include <windows.h>
#include <map>

#include <debug_event_listener.hpp>

#include <exception/internal_exception.hpp>
#include <exception/null_pointer_exception.hpp>
#include <exception/unimplemented_exception.hpp>
#include <exception/windows_exception.hpp>

#include "debug_module.hpp"

using namespace proclib;

//////////////////////////////////////////////////////////////////////////////
// Module-specific data:
//////////////////////////////////////////////////////////////////////////////

/** @brief Defines a handy mapping of processes to an attached debugger. */
typedef std::map<processId_t, debug_module*> debuggers_t;

/** @brief A staticly held list of debugger instances. */
static debuggers_t *s_Debuggers = NULL;

/** Statically created by the debug module, this will allocate the container for
 ** debugger classes, and on shutdown, will destroy them (along with all objects
 ** in the container).
 **
 ** @brief Allocates and frees the global list of debuggers.
 ** @author  Matt Bisson
 ** @date    8 February, 2008
 ** @since   Proclib 1.2
 ** @version Proclib 1.3
 **/
static class debugger_list_manager
{
public:
    /** @brief The constructor allocates space for keeping track of debuggers.
     **
     ** As we are a static object, this constructor will be called on application
     ** creation.  If s_Debuggers (the static object) is non-NULL, this
     ** constructor returns.
     ** @throws std::bad_alloc If the s_Debuggers object can not be allocated.
     **/
    debugger_list_manager () throw (std::bad_alloc)
    {
        if (NULL != s_Debuggers) return;

        s_Debuggers = new debuggers_t;
        if (NULL == s_Debuggers) throw std::bad_alloc ();
    }

    /** @brief The destructor frees all the debugger instances.
     **
     ** As we are a static object, this destructor will be called on application
     ** exit.  If the static object is NULL, this does nothing.
     **/
    ~debugger_list_manager () throw ()
    {
        if (NULL == s_Debuggers) return;

        debuggers_t *tempDebuggers = s_Debuggers;
        s_Debuggers = NULL;

        const debuggers_t::const_iterator kMapEnd = tempDebuggers->end ();
        for (debuggers_t::iterator it = tempDebuggers->begin ();
             kMapEnd != it;
             ++it)
        {
            if (NULL == it->second) return;

            try
            {
                it->second->destroy ();
            }
            catch (...)
            {
                // We can't really do anything here except delete the object and
                // hope for the best.  The destructor is hidden (because we're
                // supposed to use shutdown(), so we'll need to find a way to
                // free the memory by FORCING destruction rules.
                it->second->force_destroy ();
            }
        }

        // We're done with this structure.
        delete tempDebuggers;
    }
} s_DebugReaper;
/**< @brief Since the debug "reaper" lives statically, it will clean out all
 **     debugger instances on application exit. **/

bool proclib::debug_module::s_UsingAdmin = false;

//////////////////////////////////////////////////////////////////////////////
// Static public interface:
//////////////////////////////////////////////////////////////////////////////

/** @brief Enable or disable system privileges to examine another process.
 **
 ** These privileges apply to all debugging sessions, so use with care if you
 ** need one debugger to have them and another to not.
 ** @param[in] enable If true, enable the privileges; if false, disable them.
 ** @throws unimplemented_exception Disabling the privilege is not implemented.
 ** @throws windows_exception There was some OS error changing privileges.
 **/
void proclib::debug_module::EnableDebugPrivilege (bool enable /*=true*/)
    throw (psystem::exception::unimplemented_exception,
           psystem::exception::windows_exception)
{
    if (enable == s_UsingAdmin) return;

    if (!enable)
    {
        throw psystem::exception::unimplemented_exception (
            __FILE__, __FUNCTION__, __LINE__,
            "I don't know how to downgrade privileges yet...");
    }

    HANDLE hToken = NULL;

    if (!OpenProcessToken (
            GetCurrentProcess (), TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
        THROW_WINDOWS_EXCEPTION(
            GetLastError (), "Cannot get current process security information");
    }

    try
    {
        TOKEN_PRIVILEGES privs;

        memset (&privs, 0, sizeof (privs));
        privs.PrivilegeCount = 1;

        if (!LookupPrivilegeValue (
                NULL, SE_DEBUG_NAME, &privs.Privileges->Luid))
        {
            THROW_WINDOWS_EXCEPTION(
                GetLastError (), "Trouble getting current process prvileges");
        }

        privs.Privileges->Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;
        if (!AdjustTokenPrivileges (hToken, FALSE, &privs, sizeof (privs),
                                    NULL, NULL))
        {
            THROW_WINDOWS_EXCEPTION(
                GetLastError (), "Cannot change to \"debug\" privileges");
        }
    }
    catch (...)
    {
        /** @todo Use scoped object */
        if (NULL != hToken) CloseHandle (hToken);
        throw;
    }

    if (NULL != hToken) CloseHandle (hToken);
    s_UsingAdmin = enable;
}

/** @brief Retrieve a debugger to look at a given process.
 **
 ** First, find an existing debugger in the list of already created debuggers.
 ** This means that you can treat GetDebugger as an accessor to quickly find a
 ** debugger, rather than holding on to an instance in various places.  If
 ** there's not an instance for the given process, then create a new one and
 ** return it.
 **
 ** @param[in] processId The system process ID that you wish to debug,
 ** @return A debugger object that's ready to go!
 ** @throws null_pointer_exception If the debugger list is in a bad state.
 ** @throws unimplemented_exception Only one process can be debugged at a time.
 ** @throws bad_alloc If a new debugger object cannot be allocated.
 **/
debug_module& proclib::debug_module::GetDebugger (processId_t processId)
    throw (psystem::exception::null_pointer_exception,
           psystem::exception::unimplemented_exception,
           std::bad_alloc)
{
    if (NULL == s_Debuggers)
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    debuggers_t::iterator found = s_Debuggers->find (processId);
    if (s_Debuggers->end () != found)
    {
        if (NULL == found->second)
        {
            throw psystem::exception::null_pointer_exception (
                __FILE__, __FUNCTION__, __LINE__);
        }

        return *(found->second);
    }

    return *Create (processId);
}

//////////////////////////////////////////////////////////////////////////////
// Public interface:
//////////////////////////////////////////////////////////////////////////////

/** @brief Add an object (to the end) of the debug event listener queue.
 **
 ** If the listener has already been added, this method does nothing.
 **
 ** @param[in] listener An object derived from the proclib::debug_event_listener
 **     class that will recieve events from the debugger.  The listener must
 **     live at least as long as it remains in the debug module's queue.
 **/
void proclib::debug_module::addListener (proclib::debug_event_listener& listener)
    throw ()
{
    if (m_Listeners.end () != find (
            m_Listeners.begin (), m_Listeners.end (), &listener))
    {
        return;
    }

    m_Listeners.push_back (&listener);
}

/** @brief Remove an object from the debug event listener queue.
 **
 ** If the listener has not been added, this method does nothing.
 **
 ** The specified object must be the same instance of the object that was added.
 ** This function will not free any memory or destroy the object passed in.
 **
 ** @param[in] listener The object to remove.
 **/
void proclib::debug_module::removeListener (
    proclib::debug_event_listener& listener)
    throw ()
{
    const std::vector<proclib::debug_event_listener*>::iterator to_remove =
        find (m_Listeners.begin (), m_Listeners.end (), &listener);

    if (m_Listeners.end () == to_remove)
    {
        return;
    }

    m_Listeners.erase (to_remove);
}

/** @brief Keep pumping debug events until otherwise specified.
 **
 ** This function will continue to direct debug events to the debug listener
 ** objects until either,
 **   @li One of the listeners throws an exception, or
 **   @li Someone calls pauseEventLoop().
 **
 ** After any debug event gets processed, execution of the debugged process
 ** continues.
 **
 ** @throws internal_exception If one of the listeners threw an error that was
 **     not derived from std::exception.
 ** @throws windows_exception If there was a problem either continuing execution
 **     or trying to receive debugger events.
 ** @throws std::exception If one of the listeners threw an error.
 **/
void proclib::debug_module::eventLoop ()
    throw (std::exception)
{
    init_or_throw ();

    m_ContinueLooping = true;

    while (m_ContinueLooping)
    {
        pumpEvent ();
    }
}

/** @brief Stop processing debug events.
 **
 ** This will not hault execution of a running program.  This will only stop
 ** the call to eventLoop() from seeing more debug events.
 ** @throws windows_exception If the debugger was not initialized, and it's
 **     initialization causes an error.
 **/
void proclib::debug_module::pauseEventLoop ()
    throw (psystem::exception::windows_exception)
{
    init_or_throw ();
    m_ContinueLooping = false;
}

/** @brief Check the debug event queue, and call handlers if we saw an event.
 **
 ** This function will not continue execution of the process, it simply waits
 ** (for 0 seconds) for a debug event to occur.
 ** @return Did anything handle this event?  If no event occurred, this function
 **     returns false.
 ** @throws internal_exception If one of the listeners threw an error that was
 **     not derived from std::exception.
 ** @throws windows_exception If there was a problem either continuing execution
 **     or trying to receive debugger events.
 ** @throws std::exception If one of the listeners threw an error.
 **/
bool proclib::debug_module::pumpAvailableEvent () throw (std::exception)
{
    init_or_throw ();

    return pumpEvent (0);
}

/** @brief Process a debugger event (or timeout).
 **
 ** This function will continue execution of the debugged program until it
 ** receives a debug event.  There is one notable exception -- if the timeout
 ** is 0 milliseconds, it will not continue the program.
 **
 ** Once it receives an event, it loops through the list of
 ** proclib::debug_event_listener objects, given each a chance to see the
 ** message.  If any listeners through an exception, it will stop traversing the
 ** list of listeners and this function will throw that exception.
 **
 ** @param[in] ms The timeout in milliseconds to wait for a debug event.  If
 **     this is given as "INFINITE", this function will wait forever (until it
 **     recieves an event).
 ** @return If an event was seen, this returns true if and only if a listener
 **     said they handled it.  In all other cases, it will return false.
 ** @throws internal_exception If one of the listeners threw an error that was
 **     not derived from std::exception.
 ** @throws windows_exception If there was a problem either continuing execution
 **     or trying to receive debugger events.
 ** @throws std::exception If one of the listeners threw an error.  Given this
 **     behavior, it must be noted that <b>event handling cannot continue</b>
 **     after any handlers throw an exception.  Write your handlers carefully.
 ** @todo All these exceptions are entangled!  We need exceptions that are less
 **     generic so callers can tell where the problems are.
 **/
bool proclib::debug_module::pumpEvent (DWORD ms /*=INFINITE*/)
    throw (std::exception)
{
    init_or_throw ();

    DEBUG_EVENT debugEvt;

    if ((0 < ms) && (0 != m_ActiveThreadId))
    {
        // DBG_EXCEPTION_NOT_HANDLED means that if the last event was
        // EXCEPTION_DEBUG_EVENT, continuing this debugged thread must allow the
        // original exception processing to continue.  Oddly, DBG_CONTINUE
        // essentially IGNORES the exception, so we don't use it.  If we have
        // not seen an EXCEPTION_DEBUG_EVENT, either value continues execution
        // identically.
        if (!ContinueDebugEvent (
                m_ProcessId, m_ActiveThreadId, DBG_EXCEPTION_NOT_HANDLED))
        {
            THROW_WINDOWS_EXCEPTION_F(
                GetLastError (), "Cannot continue process %d", m_ProcessId);
        }
    }

    if (!WaitForDebugEvent (&debugEvt, ms))
    {
        if (ERROR_SEM_TIMEOUT == GetLastError ()) return false;

        THROW_WINDOWS_EXCEPTION_F(
            GetLastError (), "Cannot debug process %d", m_ProcessId);
    }

    /** @sideeffect m_ActiveThreadId is set here. */
    m_ActiveThreadId = debugEvt.dwThreadId;

    // EITHER SET THIS VALUE OR DON'T RETURN (i.e., throw an exception).
    bool retval;

    try
    {
        struct _debug_listener_distributer
            : public std::unary_function<debug_event_listener*, void>
        {
            _debug_listener_distributer (const DEBUG_EVENT& evt)
                : m_Event   (evt),
                  m_Handled (false)
            {}

            inline bool getHandled () const { return m_Handled; }

            void operator() (debug_event_listener *in)
            {
                m_Handled = in->dispatchEvent (m_Event) || m_Handled;
            }

        private:
            const DEBUG_EVENT& m_Event;
            bool               m_Handled;

            // Can't assign to this -- it contains a reference
            _debug_listener_distributer& operator= (
                const _debug_listener_distributer&) throw ()
            {
                return *this;
            }
        };

        struct _debug_listener_distributer debug_listener_distributer =
            std::for_each (
                m_Listeners.begin (), m_Listeners.end (),
                _debug_listener_distributer (debugEvt));

        retval = debug_listener_distributer.getHandled ();
    }
    catch (std::exception&)
    {
        // Rethrow because we don't want to create our generic-looking exception
        // by landing in the catch-block below...
        throw;
    }
    catch (...)
    {
        // WTF?  Repackage this.
        throw psystem::exception::internal_exception (
            __FILE__, __FUNCTION__, __LINE__,
            "Unknown exception while receiving a debugger event");
    }

    return retval;
}

void proclib::debug_module::dump_object () throw ()
{
    printf ("DEBUGGER (0x%08I64X):\n", this);
    printf ("\tm_ProcessId:       %u\n", m_ProcessId);
    printf ("\tm_ActiveThreadId:  %u\n", m_ActiveThreadId);
    printf ("\ts_Listeners size:  %u\n", m_Listeners.size ());
    printf ("\tm_ContinueLooping: %s\n", m_ContinueLooping ? "true" : "false");
    printf ("\ts_UsingAdmin:      %s\n",
            debug_module::s_UsingAdmin ? "true" : "false");

    fflush (stdout);
}

//////////////////////////////////////////////////////////////////////////////
// Protected methods:
//////////////////////////////////////////////////////////////////////////////

/** @brief Construct a debugger object.
 **
 ** The constructor is responsible for adding itself to the list of active
 ** debuggers.  Likewise, the destructor must remove itself.
 **
 ** @throws unimplemented_exception If this is a second debugger instance.  Only
 **     <b>one</b> debugger at a time is allowed.
 **/
proclib::debug_module::debug_module (processId_t processId)
    throw (psystem::exception::unimplemented_exception)
    : trace_object (INVALID_HANDLE_VALUE),
      m_ActiveThreadId  (0),
      m_ContinueLooping (true),
      m_ProcessId       (processId)
{
    /** @todo Make a WARNING/INFO/DEBUG/ERROR printf */
#ifdef _DEBUG
    if (s_Debuggers->end () != s_Debuggers->find (processId))
    {
        fprintf (stderr, "Clobbering an existing debugger.\n");
    }
#endif

    if (s_Debuggers->size () > 0)
    {
        throw psystem::exception::unimplemented_exception (
            __FILE__, __FUNCTION__, __LINE__,
            "Only one debugger allowed at a time");
    }
}

/** @brief Clean up and remove this debugger.
 **
 ** If the debugger list is NULL, this function does nothing (rather than
 ** returning an exception).
 **/
proclib::debug_module::~debug_module ()
    throw ()
{
}

/** @brief Allocate, and construct a debugger object.
 **
 ** @param[in] processId The debugger will examine the process given by this ID.
 ** @return A newly created debug_module object.  This should be freed by calling
 **         destroy().
 ** @throws unimplemented_exception If this is not the only debugger instance.
 ** @throws bad_alloc If memory could not be allocated.
 **/
proclib::debug_module *proclib::debug_module::Create (processId_t processId)
    throw (psystem::exception::unimplemented_exception, std::bad_alloc)
{
    proclib::debug_module *ret = new debug_module (processId);

    if (NULL == ret) throw std::bad_alloc ();
    return ret;
}

/** @brief Connect this debugger to a process.
 ** @throws windows_exception If there was a connection issue.
 **/
void proclib::debug_module::init () throw (psystem::exception::windows_exception)
{
    s_Debuggers->insert (debuggers_t::value_type (m_ProcessId, this));

    if (!DebugActiveProcess (m_ProcessId))
    {
        THROW_WINDOWS_EXCEPTION_F(
            GetLastError (), "Cannot attach to process %d", m_ProcessId);
    }

    if (!DebugSetProcessKillOnExit (FALSE))
    {
        THROW_WINDOWS_EXCEPTION(
            GetLastError (), "This won't end well...");
    }

    // Now wait for us to get our "create process" event...
    pumpEvent ();
}

/** @brief Close out the debugger and any open handles.
 **
 ** @throws windows_exception If we cannot stop the debugger from holding on to
 **     the process.
 **/
void proclib::debug_module::shutdown ()
    throw (psystem::exception::windows_exception)
{
    /** @todo \htmlonly Need to call CloseHandle for some events -- MSDN:
     ** <p>
     ** When a CREATE_PROCESS_DEBUG_EVENT occurs, the debugger application
     ** receives a handle to the image file of the process being debugged,
     ** a handle to the process being debugged, and a handle to the initial
     ** thread of the process being debugged in the DEBUG_EVENT structure.  The
     ** members these handles are returned in are u.CreateProcessInfo.hFile
     ** (image file), u.CreateProcessInfo.hProcess (process), and
     ** u.CreateProcessInfo.hThread  (initial thread). If the system previously
     ** reported an EXIT_PROCESS_DEBUG_EVENT debugging event, the system closes
     ** the handles to the process and thread when the debugger calls the
     ** ContinueDebugEvent function. The debugger should close the handle to the
     ** image file by calling the CloseHandle function.
     ** <p>
     ** When a LOAD_DLL_DEBUG_EVENT occurs, the debugger application receives a
     ** handle to the loaded DLL in the u.LoadDll.hFile member of the
     ** DEBUG_EVENT structure. This handle should be closed by the debugger
     ** \endhtmlonly
     **/
    if (!DebugActiveProcessStop (m_ProcessId))
    {
        THROW_WINDOWS_EXCEPTION_F(
            GetLastError (), "Cannot detach from process %d", m_ProcessId);
    }

    if (NULL != s_Debuggers) s_Debuggers->erase (m_ProcessId);
}
