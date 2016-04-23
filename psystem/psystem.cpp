
/** @file psystem.cpp
 ** @brief Provide method definitions for functions that are global to the
 **     psystem namespace.
 **
 ** @author  Matt Bisson
 ** @date    24 May, 2011
 ** @since   PSystem 1.3
 ** @version PSystem 1.3
 **/

#include <string>

#include "psystem_ns.hpp"

/** @brief A text buffer for our simplistic string formatting routine. */
static std::string s_TextBuffer;

/** @brief A vprintf-style formatting routine.
 **
 ** This uses a static buffer to return the <code>const char*</code>.  It is
 ** therefore not thread-safe.  Don't use it.
 **
 ** @param msg the printf format string.
 ** @param args a variable list of parameters to output.
 **
 ** @return a character array that lives in the static space.  Use it right away
 **     as it is not protected against other threads / simultaneous uses.
 **/
static const char *format_text (
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
            return "<Message too large>";
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

    s_TextBuffer = buf;
    delete [] buf;
    return s_TextBuffer.c_str ();
}

/** @brief A simple printf-style string formatting routine.
 **
 ** This uses a static buffer to return the <code>const char*</code>.  It is
 ** therefore not thread-safe.  Don't use it.
 **
 ** @param msg the printf format string.
 ** @param ... a variable list of parameters to output.
 **
 ** @return a character array that lives in the static space.  Use it right away
 **     as it is not protected against other threads / simultaneous uses.
 **/
const char *psystem::format_text (const char *msg, ...)
    throw ()
{
    va_list args;

    va_start (args, msg);
    const char *ret = ::format_text (msg, args);
    va_end (args);

    return ret;
}
