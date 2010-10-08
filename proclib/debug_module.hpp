
/** @file debug_module.hpp
 ** @brief Defines interfaces for inter-process debugging.
 **
 ** @author  Matt Bisson
 ** @date    8 February, 2008
 ** @since   Proclib 1.2
 ** @version Proclib 1.3
 **/

#ifndef __DEBUG_MODULE_HPP__
#define __DEBUG_MODULE_HPP__

#include <algorithm>
#include <vector>

#include "proclib_ns.hpp"
#include "trace_object.hpp"

namespace psystem
{
    namespace exception
    {
        class null_pointer_exception;
        class unimplemented_exception;
        class windows_exception;
    }
}
namespace proclib
{
    class debug_event_listener;

/** @brief This object controls the execution of another process.
 **
 ** I have nothing interesting to say here...
 **
 ** @todo This is one process at a time.  For the future, I'd like if I could
 **       simultaneously debug two processes.
 ** @author  Matt Bisson
 ** @date    8 February, 2008
 ** @since   Proclib 1.2
 ** @version Proclib 1.3
 **/
class debug_module : public proclib::trace_object
{
    //////////////////////////////////////////////////////////////////////////
    /// @name Static Public Interface
    //////////////////////////////////////////////////////////////////////////
public:
    static void EnableDebugPrivilege (bool enable = true)
        throw (psystem::exception::unimplemented_exception,
               psystem::exception::windows_exception);
    static debug_module& GetDebugger (processId_t processId)
        throw (psystem::exception::null_pointer_exception,
               psystem::exception::unimplemented_exception,
               std::bad_alloc);

    //////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    //////////////////////////////////////////////////////////////////////////
public:
    void addListener        (debug_event_listener& listener) throw ();
    void removeListener     (debug_event_listener& listener) throw ();

    void eventLoop          () throw (std::exception);
    void pauseEventLoop     () throw (psystem::exception::windows_exception);
    bool pumpAvailableEvent () throw (std::exception);
    bool pumpEvent          (DWORD ms = INFINITE) throw (std::exception);

    virtual void dump_object () throw ();

    //////////////////////////////////////////////////////////////////////////
    /// @name Protected Methods
    //////////////////////////////////////////////////////////////////////////
protected:
    explicit debug_module (processId_t processId)
        throw (psystem::exception::unimplemented_exception);
    virtual  ~debug_module () throw ();

    static debug_module *Create (processId_t processId)
        throw (psystem::exception::unimplemented_exception, std::bad_alloc);

    virtual void init     () throw (psystem::exception::windows_exception);
    virtual void shutdown () throw (psystem::exception::windows_exception);

    //////////////////////////////////////////////////////////////////////////
    /// @name Private Member Data
    //////////////////////////////////////////////////////////////////////////
private:
    /** @brief Datatype for the list of objects listening for debug events. */
    typedef std::vector<proclib::debug_event_listener*> listeners_t;

    /** @brief The thread we last recieved debug events from is considered
     **        "active." */
    threadId_t  m_ActiveThreadId;

    /** @brief This will stop eventLoop from processing.  Don't know what that
     **        means yet. */
    bool        m_ContinueLooping;

    /** @brief The list of object listening for debug events. */
    listeners_t m_Listeners;

    /** @brief This debugger object knows about one process only. */
    processId_t m_ProcessId;

    /** @brief Keeps track of debugger access privileges.
     **
     ** Since admin rights are global to this process (i.e. not on a per-debugger
     ** basis), keep this state in a static variable. */
    static bool s_UsingAdmin;
};

}

#endif // __DEBUG_MODULE_HPP__
