
/** @file proclib_ns.hpp
 ** @brief This file should be included by every file in the proclib namespace.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.0
 ** @version Proclib 1.3
 **/

/** @page proclib Process Debugging Library (Proclib) Reference
 **
 ** <b>Proclib</b> defines a number of interfaces that can essentially be used to debug a running application and read symbols from an executable file.
 **
 ** This library follows a number of rules that some may or may not agree with.
 ** Here is my thinking:
 **
 ** <ul>
 ** <li><i>Throw exceptions for any error:</i> There are definite downsides to throwing exceptions&mdash;mostly performance-related.  Exceptions can trigger a chain-reaction of thousands of machine instructions.  Pretty much any error that we&rsquo;re running into in this library will either be fatal, or will happen only a few times during execution of any algorithm.  I like using exceptions whenever I can because an exception immediately halts the algorithm and (as the caller of the algorithm that throws) unless you care about the error, you can transmit that same error to the caller.  Doing this with return codes a) prevents you from being able to return whatever you want, and b) means that you have to habitually check the error codes, and if it&rsquo;s not successful, you again have to figure out what specific error happened (and decide if you care).</li>
 **
 ** <li><i>Declare all exceptions:</i> Unfortunately, C++ doesn&rsquo;t enforce that you deal with exceptions at compile time, but I&rsquo;m OK with the program failing catastrophically if something totally unexpected happens.  It&rsquo;s good to know exactly what errors any algorithm can present you, and this is one way to do it.  I think on a larger-scale project, I might not be able to maintain this guarantee about what exceptions are thrown.</li>
 **
 ** <li><i>Declare an exception class for every kind of exception:</i> Having a single exception class that either doesn&rsquo;t give you enough information, or makes you call a subsequent method to get the root cause seems to defeat the purpose&hellip;</li>
 **
 ** <li><i>Never throw in destructors:</i> If the C++ runtime is in the process of unwinding the stack (and destroying objects on the stack as it goes), and one of the objects being cleaned up throws an exception, the runtime <em>must</em> invoke <code>terminate()</code> on the application.  One of the main benefits of <em>both</em> destructors and exceptions is to ensure proper execution and cleaning of resources during failure scenarios.  We&rsquo;ll just make show to never throw during an exception and take <code>terminate()</code> right off the table.</li>
 **
 ** <li><i>Never use &ldquo;<code>using</code>&rdquo; in headers:</i> A header can be used anywhere in any module, so by placing a <code>using</code> statement in your header, you are muddying the waters of the module that includes you.  Perhaps your header uses &ldquo;<code>std</code>,&rdquo; and is included in a module that defines a class named, <code>vector</code>.  Even one second thinking about namespace collisions is one second too many, I think.</li>
 **
 ** <li><i>Use forward declarations, rather than include headers:</i> Getting header files intertwined is one way to drastically increase compile times.  If every file somehow includes every other file, than changing just a single header will cause a complete rebuild.  This is obviously an exaggerated case, but it should be considered.</li>
 **
 ** <li><i>Fully qualify namespaces in function definitions:</i> I like to do this for two reasons.  First, I don&rsquo;t like to have my entire CPP file surrounded with curly-braces.  Second, I don&rsquo;t want to accidentally start writing code in the middle of a namespace because I didn&rsquo;t see the declaration at the top of the file.  I like to be explicit.  These are minor concerns, obviously.</li>
 **
 ** <li><i>Make <code>typedef</code>s for primitive types that are tied to an algorithm:</i> It&rsquo;s useful to have a type for each algorithm (e.g., <code>hProcess_t</code> for a handle to a process) instead of tying yourself to a native abstraction (e.g., <code>HPROCESS</code> or <code>HANDLE</code>), or worse yet, a native type that may change depending on the underlying hardware (<code>int</code>).</li>
 **
 ** <li><i>Order of <code>include</code> statements in CPP files:</i> Include system headers, then other internal headers (all with angle brackets), then include the header specific to that module.  Angle brackets are especially important in making sure our &ldquo;dependencies&rdquo; are only found in the include paths, rather than accidentally picked up in the local directory.  Quotes should be used for the header that&rsquo;s specific to the compiled module in case we want to have a separate, &ldquo;internal&rdquo; header sitting in the local directory.</li>
 **
 ** <li><i>Coding standards:</i> <code>CamelCase</code> for static methods.  Prefix <code>m_</code> for member data, <code>s_</code> for static, <code>g_</code> for globals, and postfix <code>_t</code> for data-types.  Preprocessor macros are <code>UPPER_CASE</code>.  Use <code>lower_case</code> with underscores for everything else.</li>
 ** </ul>
 **
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
 **
 ** @todo Make ALL handles references.
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

    /** @brief The process architectures we understand.
     **/
    typedef enum
    {
        ARCH_X86       = 0,             /**< @brief Intel x86 */
        ARCH_INTEL_IPF = 1,             /**< @brief Intel Itanium */
        ARCH_X64       = 2              /**< @brief Intel x64 */
    } architecture_t;

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
