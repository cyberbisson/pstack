
/** @file pstack.cpp
 ** @brief This module most notably contains main().
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PStack 1.0
 ** @version PStack 1.2
 **/

#include <windows.h>

#include <stdio.h>
#include <algorithm>

#include <process_module.hpp>
#include <process_trace.hpp>
#include <stack_frame.hpp>
#include <thread_trace.hpp>

#include <exception/internal_exception.hpp>
#include <exception/ui_exception.hpp>
#include <exception/windows_exception.hpp>

#include "pstack_ns.hpp"
#include "options.hpp"

using namespace proclib;
using namespace pstack;
using namespace psystem;

//////////////////////////////////////////////////////////////////////////////
/// @name Macro Definitions
//////////////////////////////////////////////////////////////////////////////

/** @brief Print a simple description of an exception */
#define UI_SIMPLE_OUTPUT_FOR_EXCEPTION(ex)                              \
    fprintf (stderr, "%s: %s\n", options::GetProgramName(), ex.what ()); \
    fflush (stdout);                                                    \
    fflush (stderr)

/** @brief Output an exception with "Too Much Information" (TMI) */
#define UI_TMI_OUTPUT_FOR_EXCEPTION(ex)                                 \
    fprintf (stderr, "%s: Exception caught at %s:%d (function %s): %s\n", \
             options::GetProgramName(),                                 \
             ex.source_file(), ex.line_number (),                       \
             ex.function(),    ex.what ());                             \
    fflush (stdout);                                                    \
    fflush (stderr)

/** @brief Depending on compilation settings, show exception output. */
#ifdef _DEBUG
#   define UI_OUTPUT_FOR_EXCEPTION(ex)                                  \
    UI_TMI_OUTPUT_FOR_EXCEPTION(ex)
#else
#   define UI_OUTPUT_FOR_EXCEPTION(ex)                                  \
    UI_SIMPLE_OUTPUT_FOR_EXCEPTION(ex)
#endif

/** @brief We require a vertion version of dbghelp.dll to function properly. */
#define DBG_HELP_DLL_VERSION            0x00060004

/// @}

namespace proclib
{
    namespace demangle
    {
        namespace gcc
        {
            char *__cxa_demangle (const char *, char *, size_t *, int *);
        }
    }
}

namespace pstack
{
    using namespace proclib::demangle::gcc;

/** This function was designed for use in the std::for_each STL construct.
 **
 ** @brief Entry-point for printing stack traces for a single process.
 ** @author  Matt Bisson
 ** @date    18 December, 2007
 ** @since   PStack 1.1
 ** @version PStack 1.3
 **/
class ptrace_entry : public std::unary_function<processId_t, void>
{
public:
    /** @brief This is my hook into std::for_each().
     **/
    void operator() (const processId_t& processId) throw ()
    {
        try
        {
            if (m_MultiProcess)
            {
                printf ("\n---------- PROCESS %d ----------\n", processId);
            }

            process_trace *p = process_trace::Create (
                processId, options::GetSearchAllStacks ());

            const process_trace::threadList_t& threads = p->getThreads ();

            for (process_trace::threadList_t::const_iterator it = threads.begin();
                 it != threads.end (); ++it)
            {
                printStack ((*it)->getThreadId (), (*it)->getStack ());
            }

            p->destroy ();
        }
        catch (psystem::exception::windows_exception& wex)
        {
            UI_OUTPUT_FOR_EXCEPTION(wex);
            m_ReturnCode = (int)wex.error_code ();
        }
        catch (psystem::exception::base_exception& bex)
        {
            UI_OUTPUT_FOR_EXCEPTION(bex);
            m_ReturnCode = -1;
        }
        catch (std::exception& ex)
        {
            UI_SIMPLE_OUTPUT_FOR_EXCEPTION(ex);
            m_ReturnCode = -2;
        }
        catch (...)
        {
            fprintf (stderr, "%s: An unknown exception occurred.\n",
                     options::GetProgramName());
            m_ReturnCode = -3;
        }
    }


    void printStack (
        proclib::threadId_t threadId,
        const proclib::thread_trace::stackFrames_t& frames)
        throw (psystem::exception::windows_exception)
    {
        printf ("\nCall stack (thread: %d):\n", threadId);
        printf ("Module\t\tAddress\t\t%s\n",
                (pstack::options::GetShowFrames () ? "Frame" : ""));

        for (proclib::thread_trace::stackFrames_t::const_iterator it =
                 frames.begin ();
             frames.end() != it;
             ++it)
        {
            if ((*it)->haveRelatedModule ())
            {
                printf ("%-15s ", (*it)->getModuleName ());
            }

            printf ("%08I64X", (*it)->getPCAddress ());

            if (pstack::options::GetShowFrames ())
            {
                printf ("\t%08I64X", (*it)->getFrameAddress ());
            }

            if (NULL != (*it)->getFunctionName ())
            {
                printf (" - ");

                if ((*it)->getModule().msvcModule ())
                {
                    printf ((*it)->getFunctionName ());
                }
                else
                {
                    /** @todo Get rid of libiberty dependence! */
                    /** @todo THIS IS A FUCKING MESS! */

                    char output[512];
                    size_t size = 512;
                    int error = 0;
                    __cxa_demangle (
                        (*it)->getFunctionName (), output, &size, &error);

                    if (0 == error)
                    {
                        printf (output);
                    }
                    else
                    {
                        printf ((*it)->getFunctionName ());
                    }
                }

                printf (" [+0x%02I64X]", (*it)->getCodeOffset ());
            }

            if ((*it)->haveSourceLine ())
            {
                printf (" - %s:%d",
                        (*it)->getSourceFile (), (*it)->getSourceLine ());
            }

            printf ("\n");
        }
    }


    /** @param multi_proc Specify true if there is 1 or more processes.
     ** @brief Construct the entry-point wrapper class.
     **/
    explicit ptrace_entry (bool multi_proc) throw ()
        : m_MultiProcess (multi_proc), m_ReturnCode (0)
    {
        // ...
    }

    /** If there is one exception from trying a stack trace, this will return
     ** the error.  If there are multiple exceptions, this will only return the
     ** last error code seen.  If no errors occur, the function returns 0.
     ** @brief Get the return value for main().
     **
     ** @return
     **   @li A Windows error may return any non-zero error.
     **   @li Other pstack-specific exceptions will return -1.
     **   @li std::exceptions will return -2.
     **   @li An unknown error returns -3.
     **   @li Upon success, the function returns 0.
     **/
    inline int getReturnCode () const throw ()
    {
        return m_ReturnCode;
    }

private:
    /** @brief This will be true if we are looking at mutliple process ID's */
    const bool m_MultiProcess;

    /** @brief The last error code we see or 0 */
    int        m_ReturnCode;
};

}

/** This function calls into the pstack::options class, then (if requested)
 ** passes control onto an iteration algorithm over the PIDs given on the
 ** command line.
 ** @brief The application's entry point.
 **
 ** @param argc How many arguments were passed in (assumed to be >0).
 ** @param argv The list of arguments.  For explanation on how they're used,
 **             see pstack::options.
 ** @return For an explanation, see proclib::ptrace_entry::getReturnCode().
 **/
int main (int argc, char *argv[])
    throw ()
{
    int returnCode = 0;

    try
    {
        if (options::InitOptions (argc, argv))
        {
            const options::pidList_t& pids = options::GetProcessIdList ();
            const ptrace_entry work_done = 
                std::for_each (pids.begin (), pids.end (),
                               ptrace_entry(pids.size() > 1));

            returnCode = work_done.getReturnCode ();
        }
    }
    catch (psystem::exception::ui_exception& uiex)
    {
        UI_OUTPUT_FOR_EXCEPTION(uiex);
        returnCode = -1;
    }
    catch (std::exception& ex)
    {
        UI_SIMPLE_OUTPUT_FOR_EXCEPTION(ex);
        returnCode = -2;
    }
    catch (...)
    {
        fprintf (stderr, "%s: An unknown exception occurred.\n",
                 options::GetProgramName());
        returnCode = -3;
    }

    options::FreeOptions ();
    return returnCode;
}
