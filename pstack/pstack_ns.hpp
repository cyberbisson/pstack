
/** @file pstack_ns.hpp
 ** @brief This file declares the pstack namespace.
 **
 ** This file and should be included by all files within that namespace.
 ** @author  Matt Bisson
 ** @date    9 July, 2008
 ** @since   PStack 1.3
 ** @version PStack 1.3
 **/

/** @mainpage Stack Printout (PStack) Reference
 **
 ** <h2>Overview</h2>
 **
 ** <p>PStack is a useful tool if you don't have proper debugging tools on a
 ** machine (for example, on a machine that you try to keep as clean as possible
 ** for QA purposes).  The app consists of one executable and has dependencies
 ** on only a few (Microsoft) DLL's.</p>
 **
 ** <p>Beyond this, the tool seems to get better stack trace information that
 ** most debuggers (including the MS debugger).  It can be told to upgrade its
 ** OS access rights (to debug processes running as a service, for example), and
 ** can search the executable on disk for debugging symbols that aren't present
 ** in memory.  This allows it to be more useful on GCC-generated executables
 ** than MSVC (which doesn't understand them at all).</p>
 **
 ** <p>The application runs fairly quickly, and with a program running 30
 ** threads, and an executable size of 150mb, it can return in well under a
 ** second with data (on 2007 technology, that is).</p>
 **
 ** <h2>Implementation</h2>
 **
 ** <p>This program relies heavily on C++ and the support library (originally
 ** designed for PStack), "proclib."  The application itself is rather simple.
 ** It just calls out to proclib for a list or threads, and iterates over them,
 ** printing their stack traces.</p>
 **/

/** @dir pstack
 ** @brief Home to the pstack namespace and the main() function.
 **/

#ifndef __PSTACK_NS_HPP__
#define __PSTACK_NS_HPP__

//////////////////////////////////////////////////////////////////////////////
// Macro definitions:
//////////////////////////////////////////////////////////////////////////////

/** @brief Informational name of this application. */
#define PSTACK_APP_NAME                 "PStack (Process Stack Printer)"

/** @brief Version number representing major functionality changes. */
#ifndef PSTACK_MAJOR_VERSION
#   define PSTACK_MAJOR_VERSION         1
#endif

/** @brief Version representing incremental bug-fixing and minor feature
 **        changes. */
#ifndef PSTACK_MINOR_VERSION
#   define PSTACK_MINOR_VERSION         3
#endif

/** @brief Corresponds to a nightly build number. */
#ifndef PSTACK_PATCH_LEVEL
#   define PSTACK_PATCH_LEVEL           0
#endif

/** If this is 0, this is a "special" build.
 ** @brief Corresponds to a nightly build number.
 **/
#ifndef PSTACK_BUILD_NUMBER
#   define PSTACK_BUILD_NUMBER          0
#endif

/** @brief This is used to make a note in the version info in the executable. */
#if (PSTACK_BUILD_NUMBER == 0)
#   define _PRIVATE_BUILD               1
#endif

/** @brief The namespace for <i>anything</i> needed by all of PStack.
 **
 ** @author  Matt Bisson
 ** @date    9 July, 2008
 ** @since   PStack 1.3
 ** @version PStack 1.3
 **/
namespace pstack
{

}

#endif // __PSTACK_NS_HPP__
