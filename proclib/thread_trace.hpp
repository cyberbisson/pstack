
/** @file thread_trace.hpp
 ** @brief Declare the logical objects needed to examine a thread.
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.0
 ** @version Proclib 1.3
 **/

#ifndef __THREAD_TRACE_HPP__
#define __THREAD_TRACE_HPP__

#include <windows.h>
#include <vector>
#include <dbghelp.hpp>

#include "proclib_ns.hpp"
#include "trace_object.hpp"

namespace psystem
{
    namespace exception
    {
        class windows_exception;
    }
}
namespace proclib
{
    class process_trace;
    class stack_frame;

/** Every thread has it's own call stack, so this is logically where we access
 ** that information.
 **
 ** @brief Owns the stack crawl for a process' thread.
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.0
 ** @version Proclib 1.3
 **/
class thread_trace : public proclib::trace_object
{
public:
    //////////////////////////////////////////////////////////////////////////
    /// @name Public Data-types
    //////////////////////////////////////////////////////////////////////////

    /** @brief Defines a list of stack_frame objects. */
    typedef std::vector<proclib::stack_frame *> stackFrames_t;

    //////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    //////////////////////////////////////////////////////////////////////////

    static thread_trace *Create (
        proclib::process_trace& parent, threadId_t threadId)
        throw (psystem::exception::windows_exception, std::bad_alloc);

    const stackFrames_t& getStack ()
        throw (psystem::exception::windows_exception);

    /** @brief Obtains the system-internal handle for this object. */
    inline hThread_t getThreadHandle () const throw ()
    {
        return getHandle ();
    }

    /** @brief Returns a unique identifier for this thread. */
    inline threadId_t getThreadId () const throw ()
    {
        return m_ThreadId;
    }

    /** @brief Retrieves the reference to the process that owns this thread. */
    inline const proclib::process_trace& getParentProcess () const throw ()
    {
        return m_Parent;
    }

protected:
    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////

    explicit thread_trace (proclib::process_trace& parent, threadId_t threadId)
        throw (psystem::exception::windows_exception);

    //////////////////////////////////////////////////////////////////////////
    /// @name Inherited Object Interface
    //////////////////////////////////////////////////////////////////////////
public:
    virtual void dump_object () throw ();
protected:
    virtual void init        () throw (psystem::exception::windows_exception);
    virtual void shutdown    () throw ();

private:
    //////////////////////////////////////////////////////////////////////////
    /// @name Private Member Data
    //////////////////////////////////////////////////////////////////////////

    /** @brief The unique ID for this thread object. */
    threadId_t             m_ThreadId;

    /** @brief A reference to the process that created us. */
    proclib::process_trace& m_Parent;

    /** @brief What is the running stack trace for this thread? */
    stackFrames_t          m_StackFrames;

    //////////////////////////////////////////////////////////////////////////
    /// @name Private Utilities
    //////////////////////////////////////////////////////////////////////////

    STACKFRAME64 initStackContext () const
        throw (psystem::exception::windows_exception);

    /** @brief Copying this object is not allowed. */
    thread_trace& operator= (const thread_trace&) throw ()
    {
        return *this;
    }
};

}

#endif //  __THREAD_TRACE_HPP__
