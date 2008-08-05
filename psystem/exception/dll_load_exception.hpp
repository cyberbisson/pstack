
/** @file dll_load_exception.hpp
 ** @brief Declare exceptions for problems with shared libraries.
 **
 ** @author  Matt Bisson
 ** @date    17 July, 2008
 ** @since   PSystem 1.3
 ** @version PSystem 1.3
 **/

#ifndef __DLL_LOAD_EXCEPTION_HPP__
#define __DLL_LOAD_EXCEPTION_HPP__

#include "exception/internal_exception.hpp"

namespace psystem
{
    namespace exception
    {

/** @brief Exception to describe a problem loading or executing DLL functions.
 ** @author  Matt Bisson
 ** @date    17 July, 2008
 ** @since   PSystem 1.3
 ** @version PSystem 1.3
 **/
class dll_load_exception : public internal_exception
{
    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////
public:
    dll_load_exception (
        const char *file,  const char *function, int line, const char *msg, ...)
        throw ();

protected:
    dll_load_exception (const char *file,  const char *function, int line)
        throw ();
};
    }
}

#endif
