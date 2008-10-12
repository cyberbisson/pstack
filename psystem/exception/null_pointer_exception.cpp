
/** @file null_pointer_exception.cpp
 ** @brief Define the null_pointer_exception class.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/

#include <stdarg.h>
#include "null_pointer_exception.hpp"

using namespace psystem::exception;

/** @brief This constructor initializes the object with a contextual message
 **        given by the caller
 **
 ** @param file The source file where the exception was thrown (or NULL).
 ** @param function The offending function's name (or NULL).
 ** @param line The line number in <i>file</i> where the exception occurred.
 **/
psystem::exception::null_pointer_exception::null_pointer_exception (
    const char *file, const char *function, int line)
    throw ()
    : internal_exception (file, function, line)
{
    set_context ("NULL pointer at %s:%i in %s()", file, line, function);
}
