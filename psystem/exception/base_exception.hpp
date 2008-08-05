
/** @file base_exception.hpp
 ** @brief Declares the base interface for exceptions thrown from psystem
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/

#ifndef __BASE_EXCEPTION_HPP__
#define __BASE_EXCEPTION_HPP__

#include "exception_ns.hpp"

#include <stdarg.h>
#include <exception>
#include <string>

namespace psystem
{
    namespace exception
    {

/** This exception will never be raised directly.  It provides base
 ** functionality for all exception raised in PSystem-based applications.
 **
 ** @brief Exception class that all PSystem exceptions must inherit.
 ** @author  Matt Bisson
 ** @date    26 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/
class base_exception : public std::exception
{
    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////
protected:
    explicit base_exception (const char *file,  const char *function, int line)
        throw ();
public:
    virtual ~base_exception () throw ();

public:
    //////////////////////////////////////////////////////////////////////////
    /// @name Virtual method overrides
    //////////////////////////////////////////////////////////////////////////

    /** @brief Find the contextual information left in this exception object.
     **
     ** This is usually left here by the throw-er.
     ** @return A C-style character string describing the general cause of the
     **         current error.
     **/
    virtual const char *what () const throw ()
    {
        return m_Context.c_str ();
    }

    //////////////////////////////////////////////////////////////////////////
    /// @name Acessors
    //////////////////////////////////////////////////////////////////////////

    /** @brief Get the string that contains the function name where this
     **        exception was thrown. **/
    inline const char *function () const throw ()
    {
        return m_Function.c_str ();
    }

    /** @brief Get the line number where the exception was thrown.
     **
     ** This will be the source line in the file retrieved from getSourceFile()
     **/
    inline int line_number () const throw ()
    {
        return m_LineNumber;
    }

    /** @brief Get the source file that originally threw the exception.
     **/
    inline const char *source_file () const throw ()
    {
        return m_SourceFile.c_str ();
    }


protected:
    //////////////////////////////////////////////////////////////////////////
    /// @name Protected methods
    //////////////////////////////////////////////////////////////////////////

    void set_context (const char *msg, ...) throw ();
    void set_context (const char *msg, va_list args) throw ();

private:
    //////////////////////////////////////////////////////////////////////////
    /// @name Private member data
    //////////////////////////////////////////////////////////////////////////

    /** @brief This is accessed by what() as the only real description of the
     **        exception objects. */
    std::string m_Context;

    /** @brief Function that threw the exception */
    std::string m_Function;

    /** @brief Line in m_SourceFile where the exception was thrown. */
    int         m_LineNumber;

    /** @brief Name of the source module that threw an exception */
    std::string m_SourceFile;

    /// @}
};

    }
}

#endif
