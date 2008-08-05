
/** @file stack_frame.hpp
 ** @brief Declare objects that encapsulate a single stack frame.
 ** @author  Matt Bisson
 ** @date    24 December, 2007
 ** @since   Proclib 1.1
 ** @version Proclib 1.3
 **/

#ifndef __STACK_FRAME_HPP__
#define __STACK_FRAME_HPP__

#include <vector>
#include <dbghelp.hpp>

#include "proclib_ns.hpp"
#include "trace_object.hpp"

namespace psystem
{
    namespace exception
    {
        class null_pointer_exception;
    }
}
namespace proclib
{
    class process_module;
    class process_trace;
    class thread_trace;

/** I am a stack frame.  I basically contain information about the context of
 ** a single instruction being run in a single thread within it's call stack.
 **
 ** @brief Encapsulate a single stack frame.
 ** @author  Matt Bisson
 ** @date    24 December, 2007
 ** @since   Proclib 1.1
 ** @version Proclib 1.3
 **/
class stack_frame : public proclib::trace_object
{
public:
    //////////////////////////////////////////////////////////////////////////
    /// @name Datatype Declarations
    //////////////////////////////////////////////////////////////////////////

    /** @brief Contains data for a source code's line number in the stack */
    typedef unsigned int line_t;

    /** @note Let's hope nobody has any 4 billion line source files!
     ** @brief This value means that we don't know the source code line. */
    static const line_t NO_LINE = ~0Lu;

public:
    //////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    //////////////////////////////////////////////////////////////////////////

    static stack_frame *Create (
        const proclib::thread_trace& thread, const STACKFRAME64& framedata)
        throw (std::bad_alloc);

    /** @brief Return true if this frame could be linked to a loaded module */
    inline bool haveRelatedModule () const throw ()
    {
        return (NULL != m_RelatedModule);
    }

    const char *getModuleName () const
        throw (psystem::exception::null_pointer_exception);

    /** @brief Return true if we know the source file and line. */
    inline bool haveSourceLine () const throw ()
    {
        return (!m_SourceFile.empty ());
    }

    const proclib::process_module& getModule () const
        throw (psystem::exception::null_pointer_exception);

    /** @brief return the reference to the thread owning this stack frame. */
    inline const proclib::thread_trace& getParentThread () const throw ()
    {
        return m_ParentThread;
    }

    /** @todo fix cross-module dependency */
    const proclib::process_trace& getParentProcess () const throw ();

    line_t getSourceLine () const
        throw (psystem::exception::null_pointer_exception);

    const char *getSourceFile () const
        throw (psystem::exception::null_pointer_exception);

    /** @brief Retrieve the function called in this frame. 
     **
     ** If there is no function name available, this will return NULL. */
    inline const char *getFunctionName () const throw ()
    {
        return m_FunctionName;
    }

    /** @brief Return the program counter in this frame. */
    inline psystem::memAddress_t getPCAddress () const throw ()
    {
        return m_StackFrame.AddrPC.Offset;
    }

    /** @brief Return the frame pointer (base pointer). */
    inline psystem::memAddress_t getFrameAddress () const throw ()
    {
        return m_StackFrame.AddrFrame.Offset;
    }

    /** @brief Return the distance from the start of the current function. */
    inline psystem::memAddress_t getCodeOffset () const throw ()
    {
        return m_CodeOffset;
    }

protected:
    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////

    explicit stack_frame (
        const proclib::thread_trace& thread, const STACKFRAME64& framedata)
        throw ();

    //////////////////////////////////////////////////////////////////////////
    /// @name Inheritable Object Interface
    //////////////////////////////////////////////////////////////////////////

public:
    virtual void dump_object () throw ();
protected:
    virtual void init        () throw ();
    virtual void shutdown    () throw ();

private:
    //////////////////////////////////////////////////////////////////////////
    /// @name Private Member Data
    //////////////////////////////////////////////////////////////////////////

    /** @todo Make 'const'
     ** @brief The current code pointer in this stack frame. */
    psystem::memAddress_t          m_CodeOffset; // Make const

    /** @brief Textual name of the function in this stack frame (or NULL).
     **
     ** @note This memory is actually owned by process_module::m_Modules.
     **       Therefore, if this corresponding module goes away or m_Modules
     **       changes this value, we're in big trouble.
     **/
    const char                    *m_FunctionName;

    /** @todo Can this be a const ref?
     ** @brief Every stack frame's code pointer points to a loaded module. */
    const proclib::process_module *m_RelatedModule;

    /** @brief The file name where the code for this frame lives.
     **
     ** Note that only the ImgHelp routines give us this information, so the
     ** data does not come from process_module (as m_FunctionName <i>does</i>).
     **/
    std::string                    m_SourceFile;

    /** @brief The source line corresponding to the current instruction. */
    line_t                         m_SourceLine;

    /** @brief We hang on to the STACKFRAME64 structure in this object. */
    const STACKFRAME64             m_StackFrame;

    /** @brief A reference to the thread that runs this stack_frame. */
    const proclib::thread_trace&   m_ParentThread;

    /// @}
    /** @brief Copying this object is not allowed. */
    stack_frame& operator= (const stack_frame&) throw ()
    {
        return *this;
    }
};

}

#endif // __STACK_FRAME_HPP__
