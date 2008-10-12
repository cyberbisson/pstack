
/** @file process_trace.hpp
 ** @brief Declare interfaces for tracing a single process.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.0
 ** @version Proclib 1.3
 **/

/**
 * @todo MAKE ALL DESTRUCTORS THROW() so that we don't have stack unwinding
 *       problems.
 */

#ifndef __PROCESS_TRACE_HPP__
#define __PROCESS_TRACE_HPP__

#include <map>
#include <string>
#include <vector>

#include "proclib_ns.hpp"
#include "trace_object.hpp"

#include "debug_event_listener.hpp"
#ifdef _DEBUG
#   include "debug_event_dumper.hpp"
#endif

namespace psystem
{
    namespace exception
    {
        class unimplemented_exception;
        class windows_exception;
    }
}
namespace proclib
{
    class process_module;
    class stack_frame;
    class thread_trace;

/** @brief An object that is ready to scan a process for information.
 **
 ** This is really the entry point for an application that wishes to get
 ** information about a running process, whether that be a list of threads,
 ** loaded DLL's, or even a stack trace.  The process_trace object actually
 ** creates a debugger and is capable of upgrading the privileges of the
 ** caller's application (to debug processes run by other users).
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.0
 ** @version Proclib 1.3
 **/
class process_trace : public proclib::trace_object
{
    //////////////////////////////////////////////////////////////////////////
    /// @name Datatype Definitions
    //////////////////////////////////////////////////////////////////////////
private:
    /** @brief This is our hook into the debugger at the process_trace level.
     **
     ** The only purpose for this class is to give the process_trace object
     ** a handle to the debugger's process object when it attaches.
     **
     ** @author  Matt Bisson
     ** @date    25 November, 2007
     ** @since   Proclib 1.0
     ** @version Proclib 1.3
     **/
    class proclib_debugger : public proclib::debug_event_listener
    {
    public:
        /** @brief Default constructor hangs on to the process_trace. */
        explicit proclib_debugger (process_trace& parent) throw ()
            : m_Parent (parent) {};

    protected:
        virtual bool OnCreateProcess (
            processId_t pid, threadId_t tid,
            const CREATE_PROCESS_DEBUG_INFO& data)
            throw ();

    private:
        /** @brief We will use this reference to return the debugger's handle. */
        process_trace& m_Parent;

        /** @brief Copying this object is not allowed. */
        proclib_debugger& operator= (const proclib_debugger&) throw ()
        {
            return *this;
        }
    };

public:
    /** @brief An array of thread objects. */
    typedef std::vector<proclib::thread_trace*> threadList_t;

    /** @brief Map the base address of a running module to its object. */
    typedef std::map<const psystem::memAddress_t,
                     proclib::process_module*> moduleList_t;

    //////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    //////////////////////////////////////////////////////////////////////////
public:
    static process_trace *Create (
        processId_t processId, bool allThreads = true)
        throw (psystem::exception::unimplemented_exception,
               psystem::exception::windows_exception,
               std::bad_alloc);

    virtual void dump_object () throw ();

    /** @brief Return the OS-native process handle for use in system API's. */
    inline hProcess_t getProcessHandle () const throw ()
    {
        return getHandle ();
    }

    /** @todo Shouldn't this be abstracted?
     ** @brief Return the OS-native machine type. */
    inline DWORD getMachineType () const throw ()
    {
        return m_MachineType;
    }

    // Symbols:

    symbol_t findSymbol (const psystem::memAddress_t& location);
    symbol_t findSymbol (
        const proclib::process_module& module,
        const psystem::memAddress_t&   location);

    // Modules:

    const proclib::process_module& findModule (
        const psystem::memAddress_t& location) const;
    const char *                   getModuleName (
        const psystem::memAddress_t& location) const throw ();

    // Threads:

    /** @brief Return the list of threads that belong to this process. */
    inline const threadList_t& getThreads ()
    {
        init_or_throw ();
        return m_Threads;
    }

    //////////////////////////////////////////////////////////////////////////
    /// @name Protected Methods
    //////////////////////////////////////////////////////////////////////////
protected:
    explicit process_trace (processId_t processId, bool allThreads)
        throw (psystem::exception::unimplemented_exception,
               psystem::exception::windows_exception);

    void         attach   () throw (psystem::exception::windows_exception);
    virtual void init     () throw (psystem::exception::windows_exception);
    virtual void shutdown ();

    //////////////////////////////////////////////////////////////////////////
    /// @name Private Methods
    //////////////////////////////////////////////////////////////////////////
private:
    DWORD initMachineType () const throw ();
    void  initSymbols     () const throw ();

    static bool CALLBACK proclib::process_trace::EnumerateLoadedModules (
        const TCHAR            *moduleName,
        psystem::memAddress_t   moduleBase,
        unsigned long int       moduleSize,
        void                   *userdata)
        throw ();

    //////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    //////////////////////////////////////////////////////////////////////////
private:
    /** @brief We own all the thread_trace objects that belong to this process.
     **/
    threadList_t m_Threads;

    /** @brief If false, we stop after examining the running thread. */
    bool m_AllThreads;

    /** @brief Numerical ID for this process. */
    processId_t m_ProcessId;

    /** @brief Numerical value of the architecture this process was built for. */
    DWORD m_MachineType;

    /** @brief Maps a memory address to a loaded (code) module. */
    moduleList_t m_Modules;

    /** @brief We communicate with the debugger through this object. */
    proclib_debugger m_DebugListener;

#ifdef _DEBUG
    /** @brief Print a verbose output of all the events the debugger sees. */
    proclib::debug_event_dumper m_EventDumper;
#endif
};

}

#endif // __PROCESS_TRACE_HPP__
