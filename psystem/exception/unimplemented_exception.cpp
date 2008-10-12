
/** @file unimplemented_exception.cpp
 ** @brief Declare the ui_exception class.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/

#include <stdarg.h>
#include "unimplemented_exception.hpp"

using namespace psystem::exception;

/** @brief This constructor initializes the object with a contextual message
 **        given by the caller.
 **
 ** @param file The source file where the exception was thrown (or NULL).
 ** @param function The offending function's name (or NULL).
 ** @param line The line number in <i>file</i> where the exception occurred.
 ** @param msg A "printf" style format to describe this exception.
 **/
psystem::exception::unimplemented_exception::unimplemented_exception (
    const char *file,  const char *function, int line, const char *msg, ...)
    throw ()
    : internal_exception (file, function, line)
{
    va_list args;

    va_start (args, msg);
    set_context (msg, args);
    va_end (args);
}

/** @brief This is a private constructor for derived classes.
 **
 ** It will not initialize the structures returned from the
 ** exception::what() method.
 **
 ** @param file The source file where the exception was thrown (or NULL).
 ** @param function The offending function's name (or NULL).
 ** @param line The line number in <i>file</i> where the exception occurred.
 **/
psystem::exception::unimplemented_exception::unimplemented_exception (
    const char *file,  const char *function, int line)
    throw ()
    : internal_exception (file, function, line)
{

}
