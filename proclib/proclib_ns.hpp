
/** @file proclib_ns.hpp
 ** @brief This file should be included by every file in the proclib namespace.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.0
 ** @version Proclib 1.3
 **/

/** @page proclib Stack Printout (Proclib) Reference
 ** Yeah, you love proclib.
 ** @todo Write something here.
 ** @todo Make a bunch of exception classes (based on the psystem exceptions)
 **       that are more descriptive.  Right now, everything returns a
 **       windows_exception, pretty much...
 **/

/** @dir proclib
 ** @brief The proclib C++ source lives under this directory structure.
 **/

#ifndef __PROCLIB_NS_HPP__
#define __PROCLIB_NS_HPP__

/** @brief Informational name of this application. */
#define PROCLIB_APP_NAME                "PROCLIB (Process Utilities Library)"

/** @brief Version number representing major functionality changes. */
#ifndef PROCLIB_MAJOR_VERSION
#   define PROCLIB_MAJOR_VERSION        1
#endif

/** @brief Version representing incremental bug-fixing and minor feature
 **        changes. */
#ifndef PROCLIB_MINOR_VERSION
#   define PROCLIB_MINOR_VERSION        3
#endif

/** @brief Corresponds to a nightly build number. */
#ifndef PROCLIB_PATCH_LEVEL
#   define PROCLIB_PATCH_LEVEL          0
#endif

/** If this is 0, this is a "special" build.
 ** @brief Corresponds to a nightly build number.
 **/
#ifndef PROCLIB_BUILD_NUMBER
#   define PROCLIB_BUILD_NUMBER         0
#endif

/** @brief This is used to make a note in the version info in the executable. */
#if (PROCLIB_BUILD_NUMBER == 0)
#   define _PRIVATE_BUILD               1
#endif

#ifndef _COMPILING_RESOURCE

#include <psystem_ns.hpp>

/** @brief The namespace for <i>anything</i> needed by all of Proclib.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.0
 ** @version Proclib 1.3
 **/
namespace proclib
{
    //////////////////////////////////////////////////////////////////////////
    /// @name "Generic" Handles
    //////////////////////////////////////////////////////////////////////////

    /** @brief Datatype for uniquely identifying a process. */
    typedef DWORD   processId_t;

    /** @brief Datatype for uniquely identifying a thread. */
    typedef DWORD   threadId_t;

    /** @brief Datatype for abstracting OS-specific data for a process. */
    typedef HANDLE  hProcess_t;

    /** @brief Datatype for abstracting OS-specific data for a thread. */
    typedef HANDLE  hThread_t;

    /** @brief Datatype for abstracting OS-specific data for a module image. */
    typedef HMODULE hModule_t;

    //////////////////////////////////////////////////////////////////////////
    /// @name Global Data Types
    //////////////////////////////////////////////////////////////////////////

    /** This has to be light weight and point to a valid symbol in some
     ** process_module.
     ** @brief A simple data container for an in-memory symbol (from an
     **     executable image). */
    typedef struct
    {
        /** @brief The (starting) address of the symbol in memory. */
        psystem::memAddress_t address;

        /** @brief A pointer to the symbol's name. */
        const char *name;
    } symbol_t;
}
#endif // _COMPILING_RESOURCE

#endif // __PROCLIB_NS_HPP__
