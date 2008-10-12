
/** @file options.cpp
 ** @brief Define the interface for configuration options for PStack.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PStack 1.0
 ** @version PStack 1.3
 */

#include <exception/null_pointer_exception.hpp>
#include <exception/ui_exception.hpp>

#include "options.hpp"

using pstack::options;
using psystem::exception::null_pointer_exception;
using psystem::exception::ui_exception;

pstack::options *pstack::options::g_OptionsBlock = NULL;

/** @todo Remove this hack */
extern bool g_ImageSymbolsHack;

//////////////////////////////////////////////////////////////////////////////
// Macro definitions:
//////////////////////////////////////////////////////////////////////////////

/** @brief Used to print a help option. */
#define PRINT_OPTION(opt,desc)          printf ("  %-11s %s\n", opt, desc);

/** @brief Print the continuation of a description in the help option output. */
#define PRINT_INDENTED(desc)            \
    printf ("              %s\n", desc);

//////////////////////////////////////////////////////////////////////////////
// Public interface:
//////////////////////////////////////////////////////////////////////////////

/** @brief If the singleton object exists, it must be freed.
 **
 ** It is safe to call this method multiple times.  There is one caveat here --
 ** the options block is <b>not reference counted</b>, so if you free it once,
 ** and/or initialize it multiple times, the behavior is undefined.
 **/
void pstack::options::FreeOptions () throw ()
{
    if (NULL == g_OptionsBlock) return;

    delete g_OptionsBlock;
    g_OptionsBlock = NULL;
}

/** @brief Interpret command line options into the application's options block.
 **
 ** If the singleton preferences have already been parsed from the command line,
 ** this function does nothing.  If they have not been seen yet, this function
 ** runs through the long-getopt funtion and retrieves them into internal
 ** structures.
 **
 ** @param argc Command line argument count
 ** @param argv Array of pointers to strings received from the command line.
 ** @return If this function returns false, the caller should exit.  If it
 **         returns true, the caller should happily continue.
 ** @throws null_pointer_exception If argv was NULL.
 ** @throws ui_exception If a command-line argument was given that is not
 **         recognized by this interface.
 ** @throws bad_alloc If the options class could not be allocated.
 **/
bool pstack::options::InitOptions (int argc, char *argv[])
    throw (psystem::exception::null_pointer_exception,
           psystem::exception::ui_exception,
           std::bad_alloc)
{
    // Only init the options block if it doesn't already exist.
    if (NULL != g_OptionsBlock) return true;

    // Quick sanity check, but let's not sanity check *argv, etc...
    if (NULL == argv)
    {
        throw null_pointer_exception (__FILE__, __FUNCTION__, __LINE__);
    }

    g_OptionsBlock = new options (argv[0]);
    if (NULL == g_OptionsBlock) throw std::bad_alloc ();

    for (int i = 1; i < argc; ++i)
    {
        switch (*argv[i])
        {
        case '/':
        case '-':                       // I'm not telling people to use '-'...
            try
            {
                g_OptionsBlock->trySwitchArg (argv[i]);
            }
            catch (bool& res)
            {
                // Special exception denotes I want to barf out of this app (but
                // in an OK way).
                return res;
            }
            break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            // Hex, Octal, or Decimal args all start with 0-9.
            g_OptionsBlock->tryPidArg (argv[i]);
            break;

        default:
            throw ui_exception (
                __FILE__, __FUNCTION__, __LINE__, "Invalid argument - \"%s\"",
                argv[i]);
        }
    }

#ifdef _DEBUG
    g_OptionsBlock->dumpArguments ();
#endif

    if (g_OptionsBlock->m_ExaminePids.empty())
    {
        throw ui_exception (
            __FILE__, __FUNCTION__, __LINE__, "No processes specified.");
    }

    /** @todo Remove hack */
    g_ImageSymbolsHack = g_OptionsBlock->m_GetImageSymbols;

    return true;
}

/** @brief Determine if the application should scan the exe file for symbols.
 ** @return If true, the user wants to scan the executable image if no symbols
 **         are found from SymLoadModule64().
 ** @throws null_pointer_exception If the singleton object is invalid.
 **/
bool pstack::options::GetImageSymbols ()
    throw (psystem::exception::null_pointer_exception)
{
    if (NULL == g_OptionsBlock)
    {
        throw null_pointer_exception (__FILE__, __FUNCTION__, __LINE__);
    }

    return g_OptionsBlock->m_GetImageSymbols;
}

/** @brief Return the string used to invoke this application.
 ** @return A pointer to the invocation string.
 ** @throws null_pointer_exception If the singleton object is invalid.
 **/
const char *pstack::options::GetProgramName ()
    throw (psystem::exception::null_pointer_exception)
{
    if (NULL == g_OptionsBlock)
    {
        throw null_pointer_exception (__FILE__, __FUNCTION__, __LINE__);
    }

    return g_OptionsBlock->m_ProgramName;
}

/** @brief Returns the list of process IDs that need to be examined.
 **
 ** @throws null_pointer_exception If the singleton object is invalid.
 **/
const pstack::options::pidList_t& pstack::options::GetProcessIdList ()
    throw (psystem::exception::null_pointer_exception)
{
    if (NULL == g_OptionsBlock)
    {
        throw null_pointer_exception (__FILE__, __FUNCTION__, __LINE__);
    }

    return g_OptionsBlock->m_ExaminePids;
}

/** @brief Does the user want to see frame pointers?
 **
 ** @throws null_pointer_exception If the singleton object is invalid.
 **/
bool pstack::options::GetShowFrames ()
    throw (psystem::exception::null_pointer_exception)
{
    if (NULL == g_OptionsBlock)
    {
        throw null_pointer_exception (__FILE__, __FUNCTION__, __LINE__);
    }

    return g_OptionsBlock->m_ShowFrames;
}

/** @brief Does the user want to see debugging information?
 **
 ** @throws null_pointer_exception If the singleton object is invalid.
 **/
bool pstack::options::GetShowDebug ()
    throw (psystem::exception::null_pointer_exception)
{
    if (NULL == g_OptionsBlock)
    {
        throw null_pointer_exception (__FILE__, __FUNCTION__, __LINE__);
    }

    return g_OptionsBlock->m_ShowDebug;
}

/** @brief Does the user want to see error information?
 **
 ** @throws null_pointer_exception If the singleton object is invalid.
 **/
bool pstack::options::GetShowError ()
    throw (psystem::exception::null_pointer_exception)
{
    if (NULL == g_OptionsBlock)
    {
        throw null_pointer_exception (__FILE__, __FUNCTION__, __LINE__);
    }

    return g_OptionsBlock->m_ShowError;
}

/** @brief Does the user want to see verbose informational messages?
 **
 ** @throws null_pointer_exception If the singleton object is invalid.
 **/
bool pstack::options::GetShowInfo ()
    throw (psystem::exception::null_pointer_exception)
{
    if (NULL == g_OptionsBlock)
    {
        throw null_pointer_exception (__FILE__, __FUNCTION__, __LINE__);
    }

    return g_OptionsBlock->m_ShowInfo;
}

/** @brief Returns true if the user wants stack traces for each thread in the
 **        process.
 **
 ** @throws null_pointer_exception If the singleton object is invalid.
 **/
bool pstack::options::GetSearchAllStacks ()
    throw (psystem::exception::null_pointer_exception)
{
    if (NULL == g_OptionsBlock)
    {
        throw null_pointer_exception (__FILE__, __FUNCTION__, __LINE__);
    }

    return g_OptionsBlock->m_ShowAllThreads;
}

/** @brief Print a detailed help message describing command line arguments, etc.
 **/
void pstack::options::PrintUsage () throw ()
{
    const char * const progName = ((NULL != g_OptionsBlock)
                                   ? g_OptionsBlock->m_ProgramName
                                   : PSTACK_APP_NAME);

    printf ("Prints a running application's stack information.\n\n");

    printf ("%s [/A | /O] [/F] [/I] [/P] process [process ...]\n\n", "PSTACK",
            progName);

    PRINT_OPTION (
        "process",
        "Numerical ID of the process to scan.");
    PRINT_OPTION (
        "/A",
        "Show stack traces for all threads in the process (this");
    PRINT_INDENTED("is the default).  This will conflict with the /O option.");

    PRINT_OPTION (
        "/O",
        "Display a stack trace only for the active thread.  This");
    PRINT_INDENTED("will conflict with the /A option.");

    PRINT_OPTION (
        "/F",
        "Display frame pointers with the stack trace.");

    PRINT_OPTION (
        "/I",
        "If no symbols are found, scan the executable image.");

    PRINT_OPTION (
        "/P",
        "Use administrative privileges if possible.");

    PRINT_OPTION (
        "/V",
        "Display version info and exit.");
    PRINT_OPTION (
        "/?",
        "Display this help info and exit.");

    printf ("\nAll options apply to all process IDs given regardless of order."
            "\nThe last option specified of conflicting options is used.\n");
    printf ("\nYou can specify multiple process IDs, and they can be given in"
            "\ndecimal, octal (by prefixing the number with \'0\'), or"
            " hexadecimal\n(by prefixing the number with \"0x\").\n");
}

/** @brief Print a brief version information string.
 **
 ** This will print major and minor versions, build number, and compile date.
 **/
void pstack::options::PrintVersionInfo () throw ()
{
    printf ("\n%s [Version %u.%u.%04u (built on %s)]\n"
            "Copyright (c) 2008 Matt Bisson.  All rights reserved.\n",
            PSTACK_APP_NAME, PSTACK_MAJOR_VERSION, PSTACK_MINOR_VERSION,
            PSTACK_BUILD_NUMBER, __DATE__);
}

//////////////////////////////////////////////////////////////////////////////
// Construction / Destruction:
//////////////////////////////////////////////////////////////////////////////

/** @brief The constructor sets up the defaults for the preferences object.
 ** @param progName The invocation name of this application.  This object must
 **                 exist throughout the lifetime of the application, as it will
 **                 not be copied into the data structure.
 ** @throws null_pointer_exception if progName is NULL.
 **/
pstack::options::options (const char *progName)
    throw (psystem::exception::null_pointer_exception)
    : m_ProgramName (progName),
      m_GetImageSymbols (false),
      m_ShowAllThreads  (true),
      m_ShowFrames      (false),
      m_ShowDebug       (false),
      m_ShowError       (true),
      m_ShowInfo        (false)
{
    if (NULL == progName)
    {
        throw null_pointer_exception (__FILE__, __FUNCTION__, __LINE__);
    }
}

//////////////////////////////////////////////////////////////////////////////
// Protected methods:
//////////////////////////////////////////////////////////////////////////////

/** @brief Print the internals of this data structure to STDOUT.
 **/
void pstack::options::dumpArguments () const throw ()
{
#ifndef DEBUG_CMD_LINE_ARGS
    if (!m_ShowDebug) return;
#endif

    printf ("ARGS: \n");
    printf ("\tDEBUG:    %s\n", ((m_ShowDebug)  ? "on" : "off"));
    printf ("\tINFO:     %s\n", ((m_ShowInfo)   ? "on" : "off"));
    printf ("\tERROR:    %s\n", ((m_ShowError)  ? "on" : "off"));
    printf ("\\tFRAMES:  %s\n", ((m_ShowFrames) ? "on" : "off"));
    printf ("\tTHREADS:  show %s\n", ((m_ShowAllThreads) ? "all" : "one"));
    printf ("\tSYMBOLS:  find in %s\n",
            ((m_GetImageSymbols) ? "memory" : "memory/file"));
    printf ("\n");

    printf ("\tPID LIST: ");
    {
        const pidList_t::const_iterator kEnd = m_ExaminePids.end ();
        for (pidList_t::const_iterator it = m_ExaminePids.begin ();
             kEnd != it;
             ++it)
        {
            printf ("%d ", *it);
        }
        printf ("\n");
    }
}

/** @brief Modify the output flags to match "quiet" mode.
 **/
void pstack::options::setQuietMode () throw ()
{
    m_ShowDebug = m_ShowError = m_ShowInfo = false;
}

/** @brief Modify the output flags to match "debug" mode.
 **
 ** Basically turns on all output modes.
 **/
void pstack::options::setShowDebug () throw ()
{
    m_ShowDebug = m_ShowError = m_ShowInfo = true;
}

/** @brief Toggles the show "information" flag.
 **/
void pstack::options::setShowInfo () throw ()
{
    m_ShowInfo = true;
}

//////////////////////////////////////////////////////////////////////////////
// Utility methods:
//////////////////////////////////////////////////////////////////////////////

/** Given a text string, this function will attempt to convert this into a
 ** process ID.  It accepts input in decimal, hex, or octal (using the standard
 ** C syntax).
 **
 ** @brief Tries to convert an argument to a numerical process ID
 ** @param arg Convert me!
 ** @throws ui_exception If the given process ID was larger than the processId_t
 **                      datatype.
 ** @throws ui_exception If arg was simply not a number.
 **/
void pstack::options::tryPidArg (const char *arg)
    throw (psystem::exception::ui_exception)
{
    char                 *end;
    proclib::processId_t  newPid = strtoul (arg, &end, 0);

    if (ULONG_MAX == newPid)
    {
        throw ui_exception (
            __FILE__, __FUNCTION__, __LINE__, "Process ID is too large - \"%s\"",
            arg);
    }
    else if ((0 == newPid) && (end != (arg + strlen (arg))))
    {
        throw ui_exception (
            __FILE__, __FUNCTION__, __LINE__, "Invalid argument - \"%s\"", arg);
    }

    m_ExaminePids.insert (newPid);
}

/** Given a text string, check and see if it matches any of the arguments that
 ** we know.  This can accept arguments with either a '-' or '/' as the first
 ** character (ALTHOUGH it never actually checks that this is the case -- it
 ** relies on the caller for now).  It should be able to accept long arguments as
 ** well as switches, but there are no such arguments now.
 **
 ** This function has side-effects that set the internal state of the options
 ** object.
 **
 ** @brief Set the options state based on the given argument.
 ** @param arg Chew this up, and bring it into our system.
 ** @throws ui_exception The user has given an argument that we don't understand.
 ** @throws bool If we want to quit without raising a real exception.
 **         CATCH THIS.
 **/
void pstack::options::trySwitchArg (const char *arg)
    throw (bool, psystem::exception::ui_exception)
{
    ++arg;                              // Get rid of the '/'

    if (('\0' != *arg) && ('\0' == *(arg + 1)))
    {
        switch (*arg)
        {
        case 'A':
        case 'a':
            if (!m_ShowAllThreads) m_ShowAllThreads = true;
            return;

        case 'F':
        case 'f':
            if (!m_ShowFrames) m_ShowFrames = true;
            return;

        case 'I':
        case 'i':
            if (!m_GetImageSymbols) m_GetImageSymbols = true;
            return;

        case 'O':
        case 'o':
            if (m_ShowAllThreads) m_ShowAllThreads = false;
            return;

        case 'P':
        case 'p':
            /** @todo Use administrative privs if possible */
            return;

        case 'V':
        case 'v':
            PrintVersionInfo ();
            throw false;

        case '?':
        case 'H':
        case 'h':
            PrintUsage ();
            throw false;

        default:
            throw ui_exception (
                __FILE__, __FUNCTION__, __LINE__, "Invalid switch - \"%c\"",
                *arg);
        }
    }

    throw ui_exception (
        __FILE__, __FUNCTION__, __LINE__, "Invalid option - \"%s\"", --arg);
}
