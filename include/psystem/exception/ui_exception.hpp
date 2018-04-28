// ===-- include/psystem/exception/ui_exception.hpp ------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declares an exception type to present user errors.
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
#ifndef PSYSTEM_EXCEPTION_UI_EXCEPTION_HPP
#define PSYSTEM_EXCEPTION_UI_EXCEPTION_HPP

#include "base_exception.hpp"

namespace psystem {
namespace exception {

/**
 * @brief An exception class to present unexpected user input as errors.
 *
 * This class is mostly useful to distinguish invalid user input from other
 * catastropic errors at the catch site.  A layer that interacts with the user
 * should catch this exception type and present an error message to give
 * feedback that the given input made no sense to the application.
 *
 * @author  Matt Bisson
 * @date    26 November, 2007 -- 5 August, 2014
 * @since   PSystem 1.0
 * @version PSystem 2.0
 */
class ui_exception
    : public psystem::exception::base_exception
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct an exception instance with a formatted message.
     *
     * @param[in] file     The source file where the exception occurred.
     * @param[in] function The function in the source where the exception
     *                     occurred.
     * @param[in] line     The line in @p file where the exception occurred.
     * @param[in] message
     *     This should be in a @c printf message/args style calling convention.
     */
    template <typename... MessageArgs>
    ui_exception(
        std::string const& file,
        std::string const& function,
        uint32_t line,
        MessageArgs&&... message) noexcept
        : base_exception(
            file, function, line, std::forward<MessageArgs>(message)...)
    { }
};

} // namespace exception
} // namespace psystem

#endif // PSYSTEM_EXCEPTION_UI_EXCEPTION_HPP
