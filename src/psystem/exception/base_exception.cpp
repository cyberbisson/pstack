// ===-- src/psystem/exception/base_exception.cpp --------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Defines the root of the exception class hierarchy for PSystem.
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

#include "psystem/exception/base_exception.hpp"

#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <psystem/framework/memory.hpp>

namespace psystem {
namespace exception {

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

base_exception::base_exception(
    std::string file,
    std::string function,
    uint32_t const line) noexcept
    : m_what(),
      m_function(std::move(function)),
      m_line_number(line),
      m_source_file(std::move(file))
{

}

////////////////////////////////////////////////////////////////////////////////
// Public Interface
////////////////////////////////////////////////////////////////////////////////

char const*
base_exception::what() const noexcept
{
    return m_what.c_str();
}

std::string const&
base_exception::function() const noexcept
{
    return m_function;
}

uint32_t
base_exception::line_number() const noexcept
{
    return m_line_number;
}

std::string const&
base_exception::source_file() const noexcept
{
    return m_source_file;
}

////////////////////////////////////////////////////////////////////////////////
// Private Utilities
////////////////////////////////////////////////////////////////////////////////

/*static*/ std::string
base_exception::format_message(char const *const msg, ...) noexcept
{
    ASSERT(msg);

    std::string ret;

    va_list args;

#ifdef _MSC_VER
    size_t size = 0;
    std::unique_ptr<char[]> buf;

    do
    {
        // Value undefined after vsnprintf(), keep resetting...
        if (0 != size) { va_end(args); }
        va_start(args, msg);

        size += 128;

        if ((~0 - 128) <= size)
        {
            // Just give up if we get a buffer this huge.
            ret = "<Error message too large>";
            va_end(args);
            return ret;
        }

        buf.reset(new char[size]);
        if (!buf) break;
    }
    while (-1 == vsnprintf_s(buf.get(), size, size, msg, args));
#else
    va_start(args, msg);

    size_t const size = vsnprintf(nullptr, 0, msg, args) + 1;
    va_end(args);

    auto buf = std::make_unique<char[]>(size);

    if (buf)
    {
        va_start(args, msg); // Value undefined after first vsnprintf()
        vsnprintf(buf.get(), size, msg, args);
    }
#endif
    ret = buf.get();
    va_end(args);

    return ret;
}

} // namespace exception
} // namespace psystem

////////////////////////////////////////////////////////////////////////////////
// Related Free Functions
////////////////////////////////////////////////////////////////////////////////

std::ostream&
operator<<(std::ostream& out, psystem::exception::base_exception const& ex)
    noexcept
{
#ifdef _DEBUG
    out << "Exception caught at " << ex.source_file() << ':' << ex.line_number()
        << " (" << ex.function() << "): " << ex.what();
#else
    out << ex.what();
#endif

    return out;
}
