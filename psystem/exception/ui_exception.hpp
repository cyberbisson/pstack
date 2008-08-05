
/** @file ui_exception.hpp
 ** @brief Declare the ui_exception class.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/

#ifndef __UI_EXCEPTION_HPP__
#define __UI_EXCEPTION_HPP__

#include "exception/base_exception.hpp"

namespace psystem
{
    namespace exception
    {

/** This exception is raised when the user takes an invalid action.  This is
 ** usually something like an invalid command-line argument.
 **
 ** @brief Exception any kind of user-interation error.
 ** @author  Matt Bisson
 ** @date    26 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/
class ui_exception : public base_exception
{
    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////
public:
    ui_exception (
        const char *file,  const char *function, int line, const char *msg, ...)
        throw ();

protected:
    ui_exception (const char *file,  const char *function, int line)
        throw ();
};

    }
}

#endif
