
/** @file options.hpp
 ** @brief Declare the interface for configuration options for PStack.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PStack 1.0
 ** @version PStack 1.3
 **/

#ifndef __OPTIONS_HPP__
#define __OPTIONS_HPP__

#include <exception>
#include <set>
#include <proclib_ns.hpp>

#include "pstack_ns.hpp"

namespace psystem
{
    namespace exception
    {
        class null_pointer_exception;
        class ui_exception;
    }
}
namespace pstack
{

/** The options class is a singleton object that is meant to be accessed only
 ** through static member functions.  This object is initialized with the
 ** argc and argv command line options and parsed via the Windows argument
 ** style.
 **
 ** @brief Singleton block to store command-line preferences.
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PStack 1.0
 ** @version PStack 1.3
 **/
class options
{
public:
    //////////////////////////////////////////////////////////////////////////
    /// @name Data-type Declarations
    //////////////////////////////////////////////////////////////////////////

    /** @brief A list of tokens recieved from the command line. */
    typedef std::set<proclib::processId_t> pidList_t;

    //////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    //////////////////////////////////////////////////////////////////////////

    static void FreeOptions () throw ();
    static bool InitOptions (int argc, char *argv[])
        throw (psystem::exception::null_pointer_exception,
               psystem::exception::ui_exception);

    static bool               GetImageSymbols    ()
        throw (psystem::exception::null_pointer_exception);
    static const char        *GetProgramName     ()
        throw (psystem::exception::null_pointer_exception);
    static const pidList_t&   GetProcessIdList   ()
        throw (psystem::exception::null_pointer_exception);
    static bool               GetSearchAllStacks ()
        throw (psystem::exception::null_pointer_exception);
    static bool               GetShowFrames      ()
        throw (psystem::exception::null_pointer_exception);

    static bool               GetShowDebug       ()
        throw (psystem::exception::null_pointer_exception);
    static bool               GetShowError       ()
        throw (psystem::exception::null_pointer_exception);
    static bool               GetShowInfo        ()
        throw (psystem::exception::null_pointer_exception);

    static void PrintUsage       () throw ();
    static void PrintVersionInfo () throw ();

protected:
    //////////////////////////////////////////////////////////////////////////
    /// @name Protected Data
    //////////////////////////////////////////////////////////////////////////

    /** @brief This is our singleton object. */
    static options *g_OptionsBlock;

    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////
    explicit options (const char *progName)
        throw (psystem::exception::null_pointer_exception);

    //////////////////////////////////////////////////////////////////////////
    /// @name Protected Methods
    //////////////////////////////////////////////////////////////////////////
    void dumpArguments () const throw ();

    void setQuietMode () throw ();
    void setShowDebug () throw ();
    void setShowInfo  () throw ();

private:
    //////////////////////////////////////////////////////////////////////////
    /// @name Utility Methods
    //////////////////////////////////////////////////////////////////////////

    void tryPidArg    (const char *arg) throw (psystem::exception::ui_exception);
    void trySwitchArg (const char *arg)
        throw (bool, psystem::exception::ui_exception);

    //////////////////////////////////////////////////////////////////////////
    /// @name Private Member Data
    //////////////////////////////////////////////////////////////////////////

    /** @brief This is any argument that was not a flag, and can be parsed as
     **        a number */
    pidList_t    m_ExaminePids;

    /** @brief If true, the user wants to scan the executable image if no
     **        symbols are found from SymLoadModule. */
    bool         m_GetImageSymbols;

    /** @brief The currently executing application invocation name. */
    const char  *m_ProgramName;

    /** @brief Does the app examine all threads, or just the active one? */
    bool         m_ShowAllThreads;

    /** @brief Does the application show frame pointers? */
    bool         m_ShowFrames;

    /** @brief Show verbose debugging messages. */
    bool         m_ShowDebug;

    /** @brief Show error messages. */
    bool         m_ShowError;

    /** @brief Show verbose informational messages. */
    bool         m_ShowInfo;
};

}

#endif // __OPTIONS_HPP__
