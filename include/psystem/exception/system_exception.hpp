// ===-- include/psystem/exception/system_exception.hpp --- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declares an exception that understands system error codes.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#pragma once
#ifndef PSYSTEM_EXCEPTION_BASE_SYSTEM_EXCEPTION_HPP
#define PSYSTEM_EXCEPTION_BASE_SYSTEM_EXCEPTION_HPP

#include "exception_ns.hpp"
#include "base_exception.hpp"

//////////////////////////////////////////////////////////////////////////////
/// @name Convenience Macros
//////////////////////////////////////////////////////////////////////////////

/**
 * @brief Throw a Windows API error as an exception with a brief description.
 *
 * The result of the psystem::exception::system_exception::what() method will be
 * something like "[error message]: [msg]".
 *
 * @param err The Windows API error code.
 * @param msg A brief description of the error (other than the message that will
 *            be formated automatically).
 *
 * @relates psystem::exception::system_exception
 */
#define THROW_WINDOWS_EXCEPTION(err, msg)                       \
    {                                                           \
        throw psystem::exception::system_exception(             \
            __FILE__, __FUNCTION__, __LINE__, err, msg) ;       \
    }

/**
 * @brief Throw a Windows API error as an exception with a formatted
 *        description.
 *
 * The result of the psystem::exception::system_exception::what() method will be
 * something like "[error message]: [formatted msg]".
 *
 * @param err The Windows API error code.
 * @param msg The @c printf style format string.
 * @param ... Arguments to populate the message given by @p msg.
 *
 * @relates psystem::exception::system_exception
 */
#define THROW_WINDOWS_EXCEPTION_F(err, msg, ...)                        \
    {                                                                   \
        throw psystem::exception::system_exception(                     \
            __FILE__, __FUNCTION__, __LINE__, err, msg, __VA_ARGS__) ;  \
    }

/// @}

namespace psystem {
namespace exception {

/**
 * @brief An exception type to encapsulate errors presented by the system API.
 *
 * Most notably, this exception type takes a numerical error code (e.g.,
 * @c errno), and produces a coherent message based on that error for the
 * result of what().
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @see THROW_WINDOWS_EXCEPTION
 * @see THROW_WINDOWS_EXCEPTION_F
 *
 * @todo Make a @c windows_exception that derives from this; allow errno in a
 *       different exception type.
 * @todo Remove the duplicate m_what members.
 */
class system_exception
    : public psystem::exception::base_exception
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Construct an exception with a formatted description of the error.
     *
     * @param[in] file     The source file where the exception occurred.
     * @param[in] function The function in the source where the exception
     *                     occurred.
     * @param[in] line     The line in @p file where the exception occurred.
     * @param[in] error_code
     *     This is an OS-specific error code that will be translated into a
     *     textual description of the problem.  Presumably, this is an @c errno
     *     or @c GetLastError() code that was raised as the result of a system
     *     API failure.
     * @param[in] message
     *     This should be in a @c printf message/args style calling convention.
     */
public:
    template <typename... MessageArgs>
    system_exception(
        std::string const& file,
        std::string const& function,
        uint32_t line,
        DWORD error_code,
        MessageArgs&&... message) noexcept
        : base_exception(
            file, function, line, std::forward<MessageArgs>(message)...),
          m_error_code(error_code),
          m_what(std::string(base_exception::what())
                 + ": "
                 + get_windows_error(error_code))
    { }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Virtual Method Overrides
    ////////////////////////////////////////////////////////////////////////////
public:
    /// @copydoc psystem::exception::base_exception::what()
    /// @todo Remove me
    virtual char const *what() const noexcept override;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Accessors
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Return the Windows error code that caused the exception.
     *
     * @return Check the Windows API for possible error codes thrown by Win32
     *         functions.
     */
    DWORD error_code() const noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Private Utilities
    ////////////////////////////////////////////////////////////////////////////
private:
    /**
     * @brief Given a Windows system error code, retrieve a textual description
     *        of the error.
     *
     * @param[in] error_code The Windows error code (usually retrieved from the
     *                       Win32 @c GetLastError() function).
     * @return A @c std::string containing the error message.
     */
    static std::string get_windows_error(DWORD error_code) noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief The Windows error code that caused this exception.
    DWORD const m_error_code;

    /// @todo Remove me.
    std::string const m_what;
};

} // namespace exception
} // namespace psystem

#endif // PSYSTEM_EXCEPTION_BASE_SYSTEM_EXCEPTION_HPP
