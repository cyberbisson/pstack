
/** @file windows_exception.hpp
 ** @brief Declare the interface for presenting Windows-specific problems.
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/

#ifndef __WINDOWS_EXCEPTION_HPP__
#define __WINDOWS_EXCEPTION_HPP__

#include <exception>
#include <string>

#include "exception/base_exception.hpp"

//////////////////////////////////////////////////////////////////////////////
/// @name Convenience Macros
//////////////////////////////////////////////////////////////////////////////

/** @relates psystem::exception::windows_exception
 ** @brief Populate and throw a windows_exception */
#define THROW_WINDOWS_EXCEPTION(err, msg)                       \
    {                                                           \
        throw psystem::exception::windows_exception (            \
            __FILE__, __FUNCTION__, __LINE__, err, msg) ;       \
    }

/** @relates psystem::exception::windows_exception
 ** @brief Populate and throw a windows_exception with a printf-style message */
#define THROW_WINDOWS_EXCEPTION_F(err, msg, ...)                        \
    {                                                                   \
        throw psystem::exception::windows_exception (                    \
            __FILE__, __FUNCTION__, __LINE__, err, msg, __VA_ARGS__) ;  \
    }

/// @}

namespace psystem
{
    namespace exception
    {

/** @brief An Exception class that automatically translates a Windows error
 **        (DWORD) to some human-readable string.
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   PSystem 1.0
 ** @version PSystem 1.3
 **/
class windows_exception : public psystem::exception::base_exception
{
public:
    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////
    windows_exception (
        const char *file,  const char *function, int line,
        DWORD       error, const char *msg, ...)
        throw ();

    //////////////////////////////////////////////////////////////////////////
    /// @name Acessors
    //////////////////////////////////////////////////////////////////////////

    /** @brief Access the Windows error code. */
    inline const DWORD error_code () const throw ()
    {
        return m_ErrCode;
    }

    //////////////////////////////////////////////////////////////////////////
    /// @name Utility Methods
    //////////////////////////////////////////////////////////////////////////
    static std::string GetWindowsError (DWORD error) throw ();

private:
    //////////////////////////////////////////////////////////////////////////
    // @name Private Member Data
    //////////////////////////////////////////////////////////////////////////

    /** @brief We'll hang on to the Windows error code if anyone needs it. */
    DWORD m_ErrCode;
};

    } // exception
} // psystem

#endif //  __WINDOWS_EXCEPTION_HPP__
