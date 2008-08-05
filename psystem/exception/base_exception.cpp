
/** @file base_exception.cpp
 ** @brief Defines the basic code needed by psystem exception classes.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/

#include <stdarg.h>
#include <exception>

#include "base_exception.hpp"

using namespace psystem::exception;

/** @brief Pass-through to exception::~excetion()
 **/
psystem::exception::base_exception::~base_exception ()
    throw ()
{
    // Nothing to delete...
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
psystem::exception::base_exception::base_exception (
    const char *file,  const char *function, int line)
    throw ()
    : std::exception (),
      m_Function   (function),
      m_LineNumber (line),
      m_SourceFile (file)
{
    // ?
}

/** @brief Set the internal data that is returned via what().
 **
 ** Prints the context information to m_Context.  This will be returned via the
 ** what() function.
 ** @param msg A printf style format string.
 **/
void psystem::exception::base_exception::set_context (const char *msg, ...)
    throw ()
{
    va_list args;

    va_start (args, msg);
    set_context (msg, args);
    va_end (args);
}

/** @brief Given a format string, this will fill the internal m_Context variable.
 ** @param msg A printf style format string.
 ** @param args The variable argument list relating to msg.
 **/
void psystem::exception::base_exception::set_context (
    const char *msg, va_list args)
    throw ()
{
#ifdef _MSC_VER
    size_t  size = 0;
    char   *buf  = NULL;

    do
    {
        if (NULL != buf) delete [] buf;
        size += 128;

        if ((~0 - 128) <= size)
        {
            // Just give up if we get a buffer this huge.
            delete [] buf;
            m_Context = "<Error message too large>";
            return;
        }

        buf = new char [size];
        if (NULL == buf) break;
    }
    while (-1 == vsnprintf (buf, size, msg, args));
#else
    size_t  size = vsnprintf (NULL, 0, msg, args) + 1;
    char   *buf  = new char [size];

    if (NULL != bufsize)
    {
        vsnprintf (buf, size, msg, args);
    }
#endif

    m_Context = buf;
    delete [] buf;
}
