// ===-- src/psystem/exception/system_exception.cpp ------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Defines an exception that understands system error codes.
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

#include "psystem/exception/system_exception.hpp"

#include <cstdio>
#include <functional>
#include <psystem/framework/memory.hpp>

namespace psystem {
namespace exception {

////////////////////////////////////////////////////////////////////////////////
// Virtual Method Overrides
////////////////////////////////////////////////////////////////////////////////

char const *
system_exception::what() const noexcept
{
    return m_what.c_str();
}

////////////////////////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////////////////////////

DWORD
system_exception::error_code() const noexcept
{
    return m_error_code;
}

////////////////////////////////////////////////////////////////////////////////
// Private Utilities
////////////////////////////////////////////////////////////////////////////////

/*static*/ std::string
system_exception::get_windows_error(DWORD const error_code) noexcept
{
    std::string ret;

    std::unique_ptr< char, std::function<void(char*)> > buffer(
        nullptr, [](char *ptr){ if (ptr) { ::LocalFree(ptr); } });
    char *tmp_buffer = nullptr;

#ifdef _WIN32
    // Get nice error message from Windows.
    DWORD const hr = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS |
        FORMAT_MESSAGE_MAX_WIDTH_MASK | FORMAT_MESSAGE_FROM_SYSTEM,
        nullptr,
        error_code,
        0, // langid
        reinterpret_cast<char*>(&tmp_buffer),
        0,
        nullptr);
#else
    DWORD const hr = 0; /// @todo *NIX version of FormatMessage...
#endif // _WIN32

    buffer.reset(tmp_buffer);

    if ((buffer) && (SUCCEEDED(hr)))
    {
        ret = buffer.get();
    }
    else if (0 != error_code)
    {
        char errNum[16];
#ifdef _MSC_VER
        sprintf_s(errNum, 16, "%08X", static_cast<uint32_t>(error_code));
#else
        sprintf(errNum, "%08X", static_cast<uint32_t>(error_code));
#endif

        ret.assign("Code 0x");
        ret.append(errNum);
    }

    return ret;
}

} // namespace exception
} // namespace psystem
