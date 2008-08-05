
/** @file unimplemented_exception.hpp
 ** @brief Declare the ui_exception class.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/

#ifndef __UNIMPLEMENTED_EXCEPTION_HPP__
#define __UNIMPLEMENTED_EXCEPTION_HPP__

#include "exception/internal_exception.hpp"

namespace psystem
{
    namespace exception
    {

/** This exception should be raised when the program reaches a code path that
 ** cannot succeed because parts have not been fully implemented.
 **
 ** @brief Exception for a portion of the program that is incomplete.
 ** @author  Matt Bisson
 ** @date    26 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/
class unimplemented_exception : public internal_exception
{
    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////
public:
    unimplemented_exception (
        const char *file,  const char *function, int line, const char *msg, ...)
        throw ();

protected:
    unimplemented_exception (const char *file,  const char *function, int line)
        throw ();
};

    }
}

#endif
