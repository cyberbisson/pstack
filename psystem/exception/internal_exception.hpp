
/** @file internal_exception.hpp
 ** @brief Declare exceptions for unexpected behavior in program internals.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/

#ifndef __INTERNAL_EXCEPTION_HPP__
#define __INTERNAL_EXCEPTION_HPP__

#include "exception/base_exception.hpp"

namespace psystem
{
    namespace exception
    {

/** This exception should be raised when the currently running code finds
 ** unexpected or flawed behavior.
 **
 ** @brief Exception for a flaw during the program execution.
 ** @author  Matt Bisson
 ** @date    26 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/
class internal_exception : public base_exception
{
    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////
public:
    internal_exception (
        const char *file,  const char *function, int line, const char *msg, ...)
        throw ();

protected:
    internal_exception (const char *file,  const char *function, int line)
        throw ();
};
    }
}

#endif
