
/** @file addr2ln_ns.hpp
 ** @brief This file declares the addr2ln namespace.
 **
 ** This file and should be included by all files within that namespace.
 ** @author  Matt Bisson
 ** @date    9 July, 2008
 ** @since   Addr2ln 0.1
 ** @version Addr2ln 0.1
 **/

/** @page addr2ln Address-to-line (Addr2ln) Reference
 ** @todo Something cool here.
 **/

/** @dir addr2ln
 ** @brief Home to the addr2ln namespace and the main() function.
 **/

#ifndef __ADDR2LN_NS_HPP__
#define __ADDR2LN_NS_HPP__

//////////////////////////////////////////////////////////////////////////////
// Macro definitions:
//////////////////////////////////////////////////////////////////////////////

/** @brief Informational name of this application. */
#define ADDR2LN_APP_NAME                "Addr2ln (Address-to-line Printer)"

/** @brief Version number representing major functionality changes. */
#ifndef ADDR2LN_MAJOR_VERSION
#   define ADDR2LN_MAJOR_VERSION        0
#endif

/** @brief Version representing incremental bug-fixing and minor feature
 **        changes. */
#ifndef ADDR2LN_MINOR_VERSION
#   define ADDR2LN_MINOR_VERSION        1
#endif

/** @brief Corresponds to a nightly build number. */
#ifndef ADDR2LN_PATCH_LEVEL
#   define ADDR2LN_PATCH_LEVEL          0
#endif

/** If this is 0, this is a "special" build.
 ** @brief Corresponds to a nightly build number.
 **/
#ifndef ADDR2LN_BUILD_NUMBER
#   define ADDR2LN_BUILD_NUMBER         0
#endif

/** @brief This is used to make a note in the version info in the executable. */
#if (ADDR2LN_BUILD_NUMBER == 0)
#   define _PRIVATE_BUILD               1
#endif

/** @brief The namespace for <i>anything</i> needed by all of Addr2ln.
 **
 ** @author  Matt Bisson
 ** @date    9 July, 2008
 ** @since   Addr2ln 0.1
 ** @version Addr2ln 0.1
 **/
namespace addr2ln
{

}

#endif // __ADDR2LN_NS_HPP__
