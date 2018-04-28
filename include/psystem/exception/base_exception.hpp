// ===-- include/psystem/exception/base_exception.hpp ----- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declares the root of the exception class hierarchy for PSystem.
 *
 * @author  Matt Bisson
 * @date    26 November, 2007 -- 5 August, 2014
 * @since   PSystem 1.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#pragma once
#ifndef PSYSTEM_EXCEPTION_BASE_EXCEPTION_HPP
#define PSYSTEM_EXCEPTION_BASE_EXCEPTION_HPP

#include "exception_ns.hpp"

#include <exception>
#include <iosfwd>
#include <psystem/framework/not_assignable.hpp>

namespace psystem {
namespace exception {

/**
 * @brief Base exception class from which all PSystem exceptions must inherit.
 *
 * This exception should never be thrown directly.  It provides base
 * functionality for all exceptions raised in PSystem-based applications.  It
 * is important to note that this derives from @c std::exception, and
 * applications should not throw something unless it derives from this (unless
 * they want to see @c std::terminate get called).
 *
 * Derived classes may allow construction with no message, a simple text
 * message, or a @c printf style formatted message.  Here is a simple example:
@code{.cpp}
class my_exception : public psystem::exception::base_exception
{
public:
    template <typename... MessageArgs>
    ui_exception(
        std::string const& file, std::string const& function, uint32_t line,
        MessageArgs&&... message) noexcept
        : base_exception(
            file, function, line,
            std::forward<MessageArgs>(message)...) { }
};

// This can be thrown with just a string:
throw my_exception(
    __FILE__, __FUNCTION__, __LINE__, "Some error occurred!");

// ... or with formatted details:
throw my_exception(
    __FILE__, __FUNCTION__, __LINE__,
    "Some system error occurred -- %d (%s)",
    errno, strerror(errno));
@endcode
 *
 * @author  Matt Bisson
 * @date    26 November, 2007 -- 5 August, 2014
 * @since   PSystem 1.0
 * @version PSystem 2.0
 *
 * @todo Every exception should support the copy and assignment operations.
 */
class base_exception
    : public std::exception,
      public psystem::not_assignable /// @todo Remove not_assignable
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
protected:
    /**
     * @brief Construct a base_exception instance with no message.
     *
     * @param[in] file     The source file where the exception occurred.
     * @param[in] function The function in the source where the exception
     *                     occurred.
     * @param[in] line     The line in @p file where the exception occurred.
     */
    base_exception(
        std::string file, std::string function, uint32_t line) noexcept;

    /**
     * @brief Construct a base_exception instance with a formatted message.
     *
     * @param[in] file     The source file where the exception occurred.
     * @param[in] function The function in the source where the exception
     *                     occurred.
     * @param[in] line     The line in @p file where the exception occurred.
     * @param[in] message
     *     This should be in a @c printf message/args style calling convention.
     */
    template <typename... MessageArgs>
    base_exception(
        std::string file,
        std::string function,
        uint32_t line,
        MessageArgs&&... message) noexcept
        : m_what(format_message(std::forward<MessageArgs>(message)...)),
          m_function(std::move(function)),
          m_line_number(line),
          m_source_file(std::move(file))
    { }

public:
    /// @brief Destroy the exception.
    virtual ~base_exception() noexcept override { };

    ////////////////////////////////////////////////////////////////////////////
    /// @name Virtual Method Overrides
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Return a message describing the exception that occurred.
     *
     * @return
     *     A pointer to a null-terminated C string with content related to the
     *     exception.  This points to valid memory until the underlying
     *     base_exception instance is destroyed.
     */
    virtual char const *what() const noexcept override;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Accessors
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Return the function where the exception occurred.
     *
     * @return A reference to the std::string containing the function name.  The
     *         reference is valid until the exception instance is destroyed.
     */
    std::string const& function() const noexcept;

    /**
     * @brief Return the line on which the exception occurred.
     *
     * @return A line number relative to the start of the source file.
     * @see source_file()
     */
    uint32_t line_number() const noexcept;

    /**
     * @brief Return the name of the source file where the exception occurred.
     *
     * @return A reference to the std::string containing the file name.  The
     *         reference is valid until the exception instance is destroyed.
     */
    std::string const& source_file() const noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Private Utilities
    ////////////////////////////////////////////////////////////////////////////
private:
    /**
     * @brief A functional @c sprintf() replacement.
     *
     * This method is slightly more useful to us than @c sprintf() because it
     * returns a @c std::string directly.  This lets us skip all the details of
     * finding the right size for an output buffer, allocating it, passing it
     * around, and so on.  All we're really interested in is a @c std::string.
     *
     * @param[in] msg The @c printf format string.
     * @param[in] ... A variable length @c cstdarg argument pack used to
     *                populate the result, given @p msg.
     *
     * @return This returns a std::string instance of the formatted string.
     *
     * @internal
     *     This could be implemented with perfect forwarding, but I don't want
     *     to include that much implementation detail in the header.  With a
     *     non-template function, I can keep the definition away from every
     *     compilation unit.
     * @todo Should this be moved somewhere more global?
     */
    static std::string format_message(char const *msg, ...) noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief A message describing the exception condition.
    std::string const m_what;

    /// @brief The function where the exception occurred.
    std::string const m_function;

    /// @brief The line number (in m_source_file) where the exception occurred.
    uint32_t const m_line_number;

    /// @brief The source file where the exception occurred.
    std::string const m_source_file;
};

} // namespace exception
} // namespace psystem

////////////////////////////////////////////////////////////////////////////////
/// @name Related Free Functions
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Serialize exception information to an output stream.
 *
 * This operator enables trivial output of the exception to a @c std::ostream
 * instance.  The output will be a human-readable string with (at least) the
 * message contained in the exception.
 *
 * @param[in,out] out
 *     The stream to modify with serialized exception information.
 * @param[in] ex
 *     The exception to serialize.
 *
 * @return The modified output stream after serialization.
 */
std::ostream& operator<<(
    std::ostream& out,
    psystem::exception::base_exception const& ex) noexcept;

/// @}

#endif // PSYSTEM_EXCEPTION_BASE_EXCEPTION_HPP
