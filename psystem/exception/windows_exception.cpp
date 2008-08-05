
/** @file windows_exception.cpp
 ** @brief Define the interface for presenting Windows-specific problems.
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/

#include <windows.h>

#include "windows_exception.hpp"

/** @brief Constructor for a Windows system error code.
 **
 ** This object constructs your exception.  A more convenient way to throw this
 ** excetion is through the THROW_WINDOWS_EXCEPTION macros.
 **
 ** @param file The source file where the exception was thrown (or NULL).
 ** @param function The offending function's name (or NULL).
 ** @param line The line number in <i>file</i> where the exception occurred.
 ** @param error The error code returned from Windows (often this will come
 **              from GetLastError()).
 ** @param msg A "printf" style format to describe this exception.
 **/
psystem::exception::windows_exception::windows_exception (
    const char *file,  const char *function, int line,
    DWORD     error, const char *msg, ...) throw ()
    : base_exception (file, function, line),
      m_ErrCode (error)
{
    va_list     args;
    std::string message;

    ////////////////////////////////
    // Form the user's message:
    ////////////////////////////////

    va_start (args, msg);

#ifdef _MSC_VER
    size_t bufsize = 0;
    char *buffer = NULL;

    do
    {
        if (NULL != buffer) delete [] buffer;
        bufsize += 128;

        if ((~0 - 128) <= bufsize)
        {
            // Just give up if we get a buffer this huge.
            bufsize = 0;
            delete [] buffer;
            break;
        }

        buffer = new char [bufsize];
        if (NULL == buffer)
        {
            // Can't throw in this function, so bail.
            bufsize = 0;
            break;
        } 
    }
    while (-1 == vsnprintf (buffer, bufsize, msg, args));

    if (0 < bufsize)
    {
        message = buffer;

        delete [] buffer;
        buffer = NULL;
    }
    else
    {
        message = "System error";

        delete [] buffer;
        buffer = NULL;
    }
#else
    size_t  bufsize = vsnprintf (NULL, (size_t)0, msg, args) + 1;
    char   *buffer  = new char [bufsize];

    if ((0 < bufsize) && (NULL != buffer))
    {
        vsnprintf (buffer, bufsize, msg, args);
        message = buffer;

        delete [] buffer;
        buffer = NULL;
    }
    else
    {
        message = "System error";
    }
#endif

    va_end (args);

    ////////////////////////////////
    // Create a human readable error string
    ////////////////////////////////
    message.append (" - ");
    message.append (GetWindowsError (error));

    set_context (message.c_str ());
}

/** @brief Given a Windows DWORD error, return a string with the text error
 **        message.
 **/
std::string psystem::exception::windows_exception::GetWindowsError (DWORD error)
    throw ()
{
    std::string  outStr;
    char        *buffer = NULL;

    // If all else fails, get nice error message from Windows.
    DWORD hr = FormatMessageA (
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_MAX_WIDTH_MASK | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        error,
        0, // langid
        (char*)(&buffer),
        0,
        NULL);

    if ((NULL != buffer) && (SUCCEEDED(hr)))
    {
        outStr = buffer;
    }
    else if (0 != error)
    {
        char errNum[16];
        sprintf (errNum, "%08X", error);

        outStr.assign ("Code 0x");
        outStr.append (errNum);
    }

    if (NULL != buffer) LocalFree (buffer);
    return outStr;
}
