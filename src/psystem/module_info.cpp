// ===-- src/psystem/module_info.cpp ---------------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define a container for process and shared-library module information.
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

#include <cstring>

#include "psystem/framework/memory.hpp"
#include "psystem/module_info.hpp"

////////////////////////////////////////////////////////////////////////////////
/// @name Convenience Macros
////////////////////////////////////////////////////////////////////////////////

/// @brief The marker string at the beginning of a file name for an "extended"
///        path.
#define EXTENDED_PATH_PREFIX    TEXT("\\\\?\\")

/// @brief String length (inlcuding null terminator) of EXTENDED_PATH_PREFIX.
#define EXTENDED_PATH_PREFIX_SZ \
    (sizeof(EXTENDED_PATH_PREFIX) / sizeof(*EXTENDED_PATH_PREFIX))

namespace psystem {

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

module_info::module_info(
    psystem::shared_handle<> const& file_handle,
    CREATE_PROCESS_DEBUG_INFO const& info) noexcept
    : m_file_handle(file_handle),
      m_base_of_image(reinterpret_cast<address_t>(info.lpBaseOfImage))
{
}

module_info::module_info(
    psystem::shared_handle<> const& file_handle,
    LOAD_DLL_DEBUG_INFO const& info) noexcept
    : m_file_handle(file_handle),
      m_base_of_image(reinterpret_cast<address_t>(info.lpBaseOfDll))
{

}

module_info::module_info(module_info&& o) noexcept
    : m_file_handle(std::move(o.m_file_handle)),
      m_base_of_image(o.m_base_of_image),
      m_file_name(std::move(o.m_file_name)),
      m_file_name_lock(), // Can't move flag -- special logic required!
      m_name(std::move(o.m_name)),
      m_name_lock() // Can't move flag -- special logic required!
{

}

module_info::~module_info() noexcept { }

////////////////////////////////////////////////////////////////////////////////
// Public Interface
////////////////////////////////////////////////////////////////////////////////

address_t module_info::get_base_of_image() const noexcept
{
    return m_base_of_image;
}

std::string const& module_info::get_file_name() const noexcept
{
    std::call_once(
        m_file_name_lock, &init_file_name, m_file_handle.get(), &m_file_name);
    return m_file_name;
}

std::string const& module_info::get_name() const noexcept
{
    std::call_once(m_name_lock, &init_name, get_file_name(), &m_name);
    return m_name;
}

////////////////////////////////////////////////////////////////////////////////
// Private Utilities
////////////////////////////////////////////////////////////////////////////////

/*static*/ void
module_info::init_file_name(HANDLE file_handle, std::string *const out) noexcept
{
    // Since we cannot move a std::once_flag instance, we will assume that if
    // the output string already has a value, that we do not actually need to do
    // anything here, and that this instance was moved (and the flag was reset
    // in the move).
    if ((!out) || (!out->empty())) return;

    // Hate the ugly \\?\ at file start, so we'll remove it later
    std::unique_ptr<TCHAR[]> file_buffer;

    DWORD const flags = FILE_NAME_OPENED | VOLUME_NAME_DOS;

    /// @warning Using @c GetFinalPathNameByHandle(), a Vista and later API.
    DWORD const required_file_buf_size =
        1 + GetFinalPathNameByHandle(file_handle, nullptr, 0, flags);

    if (0 != required_file_buf_size)
    {
        file_buffer = std::make_unique<TCHAR[]>(required_file_buf_size);
        file_buffer[0] = 0;

        if (0 == GetFinalPathNameByHandle(
                file_handle, file_buffer.get(), required_file_buf_size, flags))
        {
            // Some error occurred, and I really give up...
            file_buffer.reset();
            out->clear();
            return;
        }
    }

    *out = ((0 == strncmp(
                 EXTENDED_PATH_PREFIX,
                 file_buffer.get(),
                 EXTENDED_PATH_PREFIX_SZ - 1))
            ? file_buffer.get() + EXTENDED_PATH_PREFIX_SZ - 1
            : file_buffer.get());
}

/*static*/ void
module_info::init_name(
    std::string const& file_name,
    std::string *const out) noexcept
{
    // Since we cannot move a std::once_flag instance, we will assume that if
    // the output string already has a value, that we do not actually need to do
    // anything here, and that this instance was moved (and the flag was reset
    // in the move).
    if ((!out) || (!out->empty())) return;

    auto start = file_name.find_last_of('\\') + 1;
    auto end = file_name.find_last_of('.');

    // We haven't found a path separator: the start is at index 0.
    if (std::string::npos == start) start = 0;

    // File extension either hasn't been found, or is going to be truncated
    // because it's somehow before the path separator.
    if ((std::string::npos == end) || (start >= end)) end = std::string::npos;

    *out = file_name.substr(start, end - start);
}

} // namespace psystem
