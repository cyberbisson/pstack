
/** @file internal_exception.cpp
 ** @brief Defome exceptions for unexpected behavior in program internals.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/

#include <stdarg.h>
#include "internal_exception.hpp"

using namespace psystem::exception;

/** @brief This constructor initializes the object with a contextual message
 **        given by the caller.
 **
 ** @param file The source file where the exception was thrown (or NULL).
 ** @param function The offending function's name (or NULL).
 ** @param line The line number in <i>file</i> where the exception occurred.
 ** @param msg A "printf" style format to describe this exception.
 **/
psystem::exception::internal_exception::internal_exception (
    const char *file,  const char *function, int line, const char *msg, ...)
    throw ()
    : base_exception (file, function, line)
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
psystem::exception::internal_exception::internal_exception (
    const char *file,  const char *function, int line)
    throw ()
    : base_exception (file, function, line)
{

}
