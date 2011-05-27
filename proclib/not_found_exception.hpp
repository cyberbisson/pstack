
/** @file not_found_exception.hpp
 ** @brief Declare the not_found_exception class.
 **
 ** @author  Matt Bisson
 ** @date    25 May, 2011
 ** @since   Proclib 1.3
 ** @version Proclib 1.3
 **/

#ifndef __NOT_FOUND_EXCEPTION_HPP__
#define __NOT_FOUND_EXCEPTION_HPP__

#include "proclib_ns.hpp"
#include <exception/internal_exception.hpp>

namespace proclib
{

/** This is typically used for looking up symbol names and module names.
 **
 ** @brief An exception that indicates whatever was being searched for was not
 **     found.
 **
 ** @author  Matt Bisson
 ** @date    25 May, 2011
 ** @since   Proclib 1.3
 ** @version Proclib 1.3
 **/
class not_found_exception : public psystem::exception::internal_exception
{
    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////
public:
    not_found_exception (
        const char *file,  const char *function, int line, const char *msg, ...)
        throw ();

    not_found_exception (const char *file,  const char *function, int line)
        throw ();
};

}

#endif
