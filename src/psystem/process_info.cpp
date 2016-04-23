// ===-- src/psystem/process_info.cpp --------------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define functionality for a container object for process information.
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

#include "psystem/process_info.hpp"

#include <codecvt>
#include <psystem/thread_info.hpp>

/**
 * @brief Convert UTF-16 text to "native" strings.
 *
 * The idea behind this function is that the @c TCHAR type varies based on the
 * presence of the @c UNICODE preprocessor macro.  This same macro selects which
 * character type the various system API will use.  The function converts from
 * UTF-16 to the compile-time configured data type that is compatible with the
 * selected set of system API.  This may involve a conversion to UTF-8, or it
 * may simply pass the data back unchanged.
 *
 * @param[in] data A UTF-16 string of character data.
 * @return A new @c std::basic_string instance with data that is compatible with
 *         the current set of system API.
 * @throws std::range_error On failure to convert the data.
 *
 * @todo Move somewhere more global.
 */
static psystem::tstring
utf16_to_tchar(wchar_t const *data)
{
    ASSERT(data);

#ifdef UNICODE
    return data;
#else
    // No "user-supplied byet-error string" to vc -- we want the exception.
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cv;

    auto const ret = cv.to_bytes(data);
    return ret;
#endif
}

/**
 * @brief Convert UTF-8 text to "native" strings.
 *
 * The idea behind this function is that the @c TCHAR type varies based on the
 * presence of the @c UNICODE preprocessor macro.  This same macro selects which
 * character type the various system API will use.  The function converts from
 * UTF-8 to the compile-time configured data type that is compatible with the
 * selected set of system API.  This may involve a conversion to UTF-16, or it
 * may simply pass the data back unchanged.
 *
 * @param[in] data A UTF-8 string of character data.
 * @return A new @c std::basic_string instance with data that is compatible with
 *         the current set of system API.
 * @throws std::range_error On failure to convert the data.
 *
 * @todo Move somewhere more global.
 */
static psystem::tstring
utf8_to_tchar(char const *data)
{
    ASSERT(data);

#ifdef UNICODE
    // No "user-supplied byet-error string" to vc -- we want the exception.
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> cv;

    auto const ret = cv.from_bytes(data);
    return ret;
#else
    return data;
#endif
}

namespace psystem {

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

process_info::process_info(
    process_id_t pid, CREATE_PROCESS_DEBUG_INFO const& info) noexcept
    : m_process_id(pid),
      m_process_handle(info.hProcess)
{
}

process_info::~process_info() noexcept
{
}

////////////////////////////////////////////////////////////////////////////////
// Public Accessors
////////////////////////////////////////////////////////////////////////////////

HANDLE
process_info::get_process_handle() const noexcept
{
    return m_process_handle;
}

process_id_t
process_info::get_process_id() const noexcept
{
    return m_process_id;
}

////////////////////////////////////////////////////////////////////////////////
// Public Interface
////////////////////////////////////////////////////////////////////////////////

bool
process_info::add_thread(std::unique_ptr<thread_info> thread) noexcept
{
    ASSERT(thread);

    thread_id_t tid = thread->get_thread_id();

    /// @todo This will destroy the thread instance if tid already exists!
    auto result = m_threads.insert(make_pair(tid, std::move(thread)));
    return result.second;
}

void
process_info::remove_thread(thread_id_t tid) noexcept
{
    m_threads.erase(tid);
}

} // namespace psystem
