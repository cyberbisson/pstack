
/** @file null_pointer_exception.hpp
 ** @brief Declare the null_pointer_exception class.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/

#ifndef __NULL_POINTER_EXCEPTION_HPP__
#define __NULL_POINTER_EXCEPTION_HPP__

#include "exception/internal_exception.hpp"

namespace psystem
{
    namespace exception
    {

/** This exception should be raised when the program must access a pointer that
 ** is currently NULL.
 **
 ** @brief Exception raised before a NULL pointer is accessed.
 ** @author  Matt Bisson
 ** @date    26 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/
class null_pointer_exception : public internal_exception
{
    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////
public:
    null_pointer_exception (const char *file, const char *function, int line)
        throw ();
};

    }
}

#endif
