
/** @file psystem_ns.hpp
 ** @brief This file should be included by every file in the psystem namespace.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/

/** @page psystem PSystem (Proclib Subsystem) Reference
 **
 ** <p>The PSystem library supports the Proclib library (and any applcations
 ** based on it).  This library contains basic exception classes, filesystem
 ** abstractions, and even a set of classes designed to help with DLL loading
 ** and dependencies.</p>
 **
 ** <p>Anything that does not directly deal with debugging, etc., should be left
 ** out of the PStack library, and should fall into PSystem.</p>
 **/

/** @dir psystem
 ** @brief The PSystem C++ source lives under this directory structure.
 **/

#ifndef __PSYSTEM_NS_HPP__
#define __PSYSTEM_NS_HPP__

#include <windows.h>

#ifdef _MSC_VER
#   define strcmpi _strcmpi
#endif

#ifndef MAX
/** @brief Find and return the greater of two variables */
#   define MAX(a,b) (((a) > (b)) ? a : b)
#endif
#ifndef MIN
/** @brief Find and return the lesser of two variables */
#   define MIN(a,b) (((a) < (b)) ? a : b)
#endif

/** @def ASSERT
 ** @brief If _DEBUG is defined, this will test a condition and abort if it does
 **        not evaluate to true.
 **
 ** If _DEBUG is not defined, this does nothing at all.
 ** @param cond Any expression that evaluates to a boolean value.
 **/
#ifndef ASSERT
#   ifdef _DEBUG
#       define ASSERT(cond)                                             \
    {                                                                   \
        if (!(cond))                                                    \
        {                                                               \
            fprintf (stderr, "Asserting at %s:%d\n", __FILE__, __LINE__); \
            abort ();                                                   \
        }                                                               \
    }
#   else
#       define ASSERT(cond) 
#   endif // _DEBUG
#endif // ASSERT

/** @brief Informational name of this application. */
#define PSYSTEM_APP_NAME                "PSYSTEM (Proclib Subsystem)"

/** @brief Version number representing major functionality changes. */
#ifndef PSYSTEM_MAJOR_VERSION
#   define PSYSTEM_MAJOR_VERSION        1
#endif

/** @brief Version representing incremental bug-fixing and minor feature
 **        changes. */
#ifndef PSYSTEM_MINOR_VERSION
#   define PSYSTEM_MINOR_VERSION        3
#endif

/** @brief Corresponds to a nightly build number. */
#ifndef PSYSTEM_PATCH_LEVEL
#   define PSYSTEM_PATCH_LEVEL          0
#endif

/** If this is 0, this is a "special" build.
 ** @brief Corresponds to a nightly build number.
 **/
#ifndef PSYSTEM_BUILD_NUMBER
#   define PSYSTEM_BUILD_NUMBER         0
#endif

/** @brief This is used to make a note in the version info in the executable. */
#if (PSYSTEM_BUILD_NUMBER == 0)
#   define _PRIVATE_BUILD               1
#endif

#ifndef _COMPILING_RESOURCE
/** @brief The namespace for <i>anything</i> needed by all of PSystem.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/
namespace psystem
{
    //////////////////////////////////////////////////////////////////////////
    /// @name Generic Data Types
    //////////////////////////////////////////////////////////////////////////

    /** @brief Nobody should have to know how large the memory address is. */
    typedef DWORD64 memAddress_t;

    /** @brief Why is there no "byte" primitive type in C++? */
    typedef unsigned char byte_t;
}
#endif

#endif // __PSYSTEM_NS_HPP__
