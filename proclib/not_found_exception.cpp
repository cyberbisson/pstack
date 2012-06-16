
/** @file not_found_exception.cpp
 ** @brief Declare the not_found_exception class.
 **
 ** @author  Matt Bisson
 ** @date    25 May, 2011
 ** @since   Proclib 1.3
 ** @version Proclib 1.3
 **/

#include <stdarg.h>
#include "not_found_exception.hpp"

using namespace proclib;

/** @brief This constructor initializes the object with a contextual message
 **        given by the caller.
 **
 ** @param[in] file The source file where the exception was thrown (or NULL).
 ** @param[in] function The offending function's name (or NULL).
 ** @param[in] line The line number in <i>file</i> where the exception occurred.
 ** @param[in] msg A "printf" style format to describe this exception.
 **/
proclib::not_found_exception::not_found_exception (
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
 ** @param[in] file The source file where the exception was thrown (or NULL).
 ** @param[in] function The offending function's name (or NULL).
 ** @param[in] line The line number in <i>file</i> where the exception occurred.
 **/
proclib::not_found_exception::not_found_exception (
    const char *file,  const char *function, int line)
    throw ()
    : internal_exception (file, function, line)
{

}
