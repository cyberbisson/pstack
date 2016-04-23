// ===-- include/pstack/debug_event_dumper.hpp ------------ -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare an event listener to provide verbose information about events.
 *
 * @author  Matt Bisson
 * @date    14 February, 2008 -- 5 August, 2014
 * @since   PStack 1.2
 * @version PStack 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#include "pstack/debug_event_dumper.hpp"

#include <cstring>
#include <iostream>

#include <psystem/framework/iomanip.hpp>
#include <psystem/framework/memory.hpp>

using psystem::format::hex;
using psystem::format::mask;
using psystem::format::ptr;

////////////////////////////////////////////////////////////////////////////////
/// @name Convenience Macros
////////////////////////////////////////////////////////////////////////////////

/// @brief The marker string at the beginning of a file name for an "extended"
///        path.
#define EXTENDED_PATH_PREFIX    TEXT("\\\\?\\")

/// @brief String length (inlcuding null terminator) of EXTENDED_PATH_PREFIX.
#define EXTENDED_PATH_PREFIX_SZ \
    (sizeof(EXTENDED_PATH_PREFIX) / sizeof(*EXTENDED_PATH_PREFIX))

/// @def PRINT_EXCEPTION_CODE
/// @brief Inline macro to serialize exception code values into text.
/// @param code The exception code constant.
/// @see print_exception_record()

////////////////////////////////////////////////////////////////////////////////
/// @name Compilation-Unit Utilities
////////////////////////////////////////////////////////////////////////////////

/// @brief Data local to this compilation unit.
namespace {

/// @brief Do not recurse through exception records any more than this number
///        of times.
int const kMaxExceptionRecordLevels = 8;

} // namespace <anon>

/**
 * @brief Find the name of a file when given a handle to that memory-mapped file.
 *
 * @param[in] h_file
 *     The handle to the memory-mapped file to gather a name for.
 * @return A file name, or the empty string if the file name cannot be retrieved.
 *
 * @todo Merge this with the @c module_info.cpp version?
 */
static std::unique_ptr<TCHAR[]>
get_file_name_from_handle(HANDLE const h_file) noexcept
{
    std::unique_ptr<TCHAR[]> fileBuffer;

    DWORD const flags = FILE_NAME_OPENED | VOLUME_NAME_DOS;

    // Note: GetFinalPathNameByHandle() is a VISTA and later API.
    DWORD const required_file_buf_size = 1 + GetFinalPathNameByHandle(
        h_file, nullptr, 0, flags);

    if (0 != required_file_buf_size)
    {
        fileBuffer.reset(new TCHAR[required_file_buf_size]);
        fileBuffer[0] = 0;

        if (0 == GetFinalPathNameByHandle(
                h_file, fileBuffer.get(), required_file_buf_size, flags))
        {
            // Some error occurred, and I really give up...
            fileBuffer.reset();
        }
    }

    return fileBuffer;
}

/**
 * @brief Print the data that exists for every debugger event.
 *
 * @param[in] pid The process ID of the process that generated the event.
 * @param[in] tid The thread ID for the active thread in the event.
 */
static void
print_common_data(
    psystem::process_id_t const pid,
    psystem::thread_id_t const tid)
{
    std::cout << "\tProcess ID:       " << pid << '\n';
    std::cout << "\tActive thread ID: " << tid << '\n';
}

/**
 * @brief Print an @c EXCEPTION_RECORD structure.
 *
 * @c EXCEPTION_RECORD is a recursively defined structure, so this method will
 * recurse through the structure @c kMaxExceptionRecordLevels - @p count times
 * (until it finds a @c nullptr value for the next @c EXCEPTION_RECORD).
 *
 * @param[in] ex_rec The exception record to display.
 * @param[in] count  The number of recursions already performed.  The initial
 *                   call should accept the default parameter (of 0).
 */
static void
print_exception_record(
    EXCEPTION_RECORD const& ex_rec,
    int const               count = 0) noexcept
{
    if (count > kMaxExceptionRecordLevels)
    {
        std::cout << "\tException Record: Truncated...\n";
        return;
    }

    std::cout << "\tException Record: (Level: " << count << ")\n";

    std::cout << "\t\tException Code:  " << mask(ex_rec.ExceptionCode) << " ";
    switch (ex_rec.ExceptionCode)
    {
#define PRINT_EXCEPTION_CODE(code) case code: std::cout << #code; break
        PRINT_EXCEPTION_CODE(EXCEPTION_ACCESS_VIOLATION);
        PRINT_EXCEPTION_CODE(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
        PRINT_EXCEPTION_CODE(EXCEPTION_BREAKPOINT);
        PRINT_EXCEPTION_CODE(EXCEPTION_DATATYPE_MISALIGNMENT);
        PRINT_EXCEPTION_CODE(EXCEPTION_FLT_DENORMAL_OPERAND);
        PRINT_EXCEPTION_CODE(EXCEPTION_FLT_DIVIDE_BY_ZERO);
        PRINT_EXCEPTION_CODE(EXCEPTION_FLT_INEXACT_RESULT);
        PRINT_EXCEPTION_CODE(EXCEPTION_FLT_INVALID_OPERATION);
        PRINT_EXCEPTION_CODE(EXCEPTION_FLT_OVERFLOW);
        PRINT_EXCEPTION_CODE(EXCEPTION_FLT_STACK_CHECK);
        PRINT_EXCEPTION_CODE(EXCEPTION_FLT_UNDERFLOW);
        PRINT_EXCEPTION_CODE(EXCEPTION_ILLEGAL_INSTRUCTION);
        PRINT_EXCEPTION_CODE(EXCEPTION_IN_PAGE_ERROR);
        PRINT_EXCEPTION_CODE(EXCEPTION_INT_DIVIDE_BY_ZERO);
        PRINT_EXCEPTION_CODE(EXCEPTION_INT_OVERFLOW);
        PRINT_EXCEPTION_CODE(EXCEPTION_INVALID_DISPOSITION);
        PRINT_EXCEPTION_CODE(EXCEPTION_NONCONTINUABLE_EXCEPTION);
        PRINT_EXCEPTION_CODE(EXCEPTION_PRIV_INSTRUCTION);
        PRINT_EXCEPTION_CODE(EXCEPTION_SINGLE_STEP);
        PRINT_EXCEPTION_CODE(EXCEPTION_STACK_OVERFLOW);
#undef PRINT_EXCEPTION_CODE
    }
    std::cout << '\n';

    std::cout << "\t\tException Flags: " << mask(ex_rec.ExceptionFlags) << ' ';
    if (ex_rec.ExceptionFlags & EXCEPTION_NONCONTINUABLE)
    {
        std::cout << "EXCEPTION_NONCONTINUABLE";
    }
    std::cout << '\n';

    std::cout << "\t\tException Addr:  " << ptr(ex_rec.ExceptionAddress) << '\n';
    std::cout << "\t\tException Rec:   " << ptr(ex_rec.ExceptionRecord) << '\n';

    // Recurse onward...
    if (ex_rec.ExceptionRecord)
    {
        print_exception_record(*ex_rec.ExceptionRecord, count + 1);
    }
}

/// @}

////////////////////////////////////////////////////////////////////////////////
// base_debug_event_listener Interface
////////////////////////////////////////////////////////////////////////////////

namespace pstack {

bool
debug_event_dumper::create_process_event(
    psystem::process_id_t const pid, psystem::thread_id_t const tid,
    psystem::shared_handle<> const& file_handle,
    CREATE_PROCESS_DEBUG_INFO const& info) noexcept
{
    std::cout << "DEBUG EVENT: CREATE_PROCESS_DEBUG_EVENT\n";
    print_common_data(pid, tid);

    if ((info.lpImageName) && ('\0' != *(static_cast<char*>(info.lpImageName))))
    {
        std::cout << "\tDLL Name:         ";
        if (0 != info.fUnicode)
        {
            std::cout << reinterpret_cast<char const*>(info.lpImageName);
        }
        else
        {
            std::wcout << reinterpret_cast<wchar_t const*>(info.lpImageName);
        }
        std::cout << '\n';
    }
    else
    {
        std::cout << "\tProcess Name:     ";

        std::unique_ptr<TCHAR[]> fileBuffer =
            get_file_name_from_handle(file_handle.get());
        if (fileBuffer)
        {
            std::cout
                << "hFile: \""
                // Strip "\\?\" (if we find it) w/o converting to std::string
                << ((0 == strncmp(
                         EXTENDED_PATH_PREFIX,
                         fileBuffer.get(),
                         EXTENDED_PATH_PREFIX_SZ - 1))
                    ? fileBuffer.get() + EXTENDED_PATH_PREFIX_SZ - 1
                    : fileBuffer.get())
                << "\"\n";
        }
        else
        {
            std::cout << "(NONE)\n";
        }
    }

    std::cout << "\tBase of Image:    " << ptr(info.lpBaseOfImage) << '\n';
    std::cout << "\tThread Start Add: " << ptr(info.lpStartAddress) << '\n';
    std::cout << "\tThread Loc Base:  " << ptr(info.lpThreadLocalBase) << '\n';

    std::cout << "\tDebug Info Offs:  " << hex(info.dwDebugInfoFileOffset)
              << '\n';
    std::cout << "\tDebug Info Size:  " << info.nDebugInfoSize << '\n';

    std::cout << std::flush;
    return base_debug_event_listener::create_process_event(
        pid, tid, file_handle, info);
}

bool
debug_event_dumper::create_thread_event(
    psystem::process_id_t const pid, psystem::thread_id_t const tid,
    CREATE_THREAD_DEBUG_INFO const& info) noexcept
{
    std::cout << "DEBUG EVENT: CREATE_THREAD_DEBUG_EVENT\n";
    print_common_data(pid, tid);

    std::cout << "\tThread Start Add: " << ptr(info.lpStartAddress) << '\n';
    std::cout << "\tThread Loc Base:  " << ptr(info.lpThreadLocalBase) << '\n';

    std::cout << std::flush;
    return base_debug_event_listener::create_thread_event(pid, tid, info);
}

bool
debug_event_dumper::exception_event(
    psystem::process_id_t const pid, psystem::thread_id_t const tid,
    EXCEPTION_DEBUG_INFO const& info) noexcept
{
    std::cout << "DEBUG EVENT: EXCEPTION_DEBUG_EVENT\n";
    print_common_data(pid, tid);

    std::cout << "\tFirst Chance:     " << info.dwFirstChance << '\n';
    print_exception_record(info.ExceptionRecord);

    /// @todo There are also extra "parameters" that we're ignoring.

    std::cout << std::flush;
    return base_debug_event_listener::exception_event(pid, tid, info);
}

bool
debug_event_dumper::exit_process_event(
    psystem::process_id_t const pid, psystem::thread_id_t const tid,
    EXIT_PROCESS_DEBUG_INFO const& info) noexcept
{
    std::cout << "DEBUG EVENT: EXIT_PROCESS_DEBUG_EVENT\n";
    print_common_data(pid, tid);
    std::cout << "\tExit Code:        " << info.dwExitCode << '\n';

    std::cout << std::flush;
    return base_debug_event_listener::exit_process_event(pid, tid, info);
}

bool
debug_event_dumper::exit_thread_event(
    psystem::process_id_t const pid, psystem::thread_id_t const tid,
    EXIT_THREAD_DEBUG_INFO const& info) noexcept
{
    std::cout << "DEBUG EVENT: EXIT_THREAD_DEBUG_EVENT\n";
    print_common_data(pid, tid);
    std::cout << "\tExit Code:        " << info.dwExitCode << '\n';

    std::cout << std::flush;
    return base_debug_event_listener::exit_thread_event(pid, tid, info);
}

bool
debug_event_dumper::load_dll_event(
    psystem::process_id_t const pid, psystem::thread_id_t const tid,
    psystem::shared_handle<> const& file_handle,
    LOAD_DLL_DEBUG_INFO const& info) noexcept
{
    std::cout << "DEBUG EVENT: LOAD_DLL_DEBUG_EVENT\n";
    print_common_data(pid, tid);

    if ((info.lpImageName) && ('\0' != *(static_cast<char*>(info.lpImageName))))
    {
        std::cout << "\tDLL Name:         ";
        if (0 != info.fUnicode)
        {
            std::cout << reinterpret_cast<char const*>(info.lpImageName);
        }
        else
        {
            std::wcout << reinterpret_cast<wchar_t const*>(info.lpImageName);
        }
        std::cout << '\n';
    }
    else
    {
        std::cout << "\tDLL Name:         ";

        std::unique_ptr<TCHAR[]> fileBuffer =
            get_file_name_from_handle(file_handle.get());
        if (fileBuffer)
        {
            std::cout
                << "hFile: \""
                // Strip "\\?\" (if we find it) w/o converting to std::string
                << ((0 == strncmp(
                         EXTENDED_PATH_PREFIX,
                         fileBuffer.get(),
                         EXTENDED_PATH_PREFIX_SZ - 1))
                    ? fileBuffer.get() + EXTENDED_PATH_PREFIX_SZ - 1
                    : fileBuffer.get())
                << "\"\n";
        }
        else
        {
            std::cout << "(NONE)\n";
        }
    }

    std::cout << "\tBase of DLL:      " << ptr(info.lpBaseOfDll) << '\n';
    std::cout << "\tDebug Info Offs:  " << hex(info.dwDebugInfoFileOffset)
              << '\n';
    std::cout << "\tDebug Info Size:  " << info.nDebugInfoSize << '\n';

    std::cout << std::flush;
    return base_debug_event_listener::load_dll_event(
        pid, tid, file_handle, info);
}

bool
debug_event_dumper::output_debug_string_event(
    psystem::process_id_t const pid, psystem::thread_id_t const tid,
    OUTPUT_DEBUG_STRING_INFO const& info) noexcept
{
    std::cout << "DEBUG EVENT: OUTPUT_DEBUG_STRING_EVENT\n";
    print_common_data(pid, tid);

    /// @todo Read from other process's memory...

    std::cout << std::flush;
    return base_debug_event_listener::output_debug_string_event(pid, tid, info);
}

bool
debug_event_dumper::rip_event(
    psystem::process_id_t const pid, psystem::thread_id_t const tid,
    RIP_INFO const& info) noexcept
{
    std::cout << "DEBUG EVENT: RIP_EVENT\n";
    print_common_data(pid, tid);

    std::cout << "\tError Code:       " << hex(info.dwError) << '\n';
    std::cout << "\tError Type:       " << hex(info.dwType);
    switch (info.dwType)
    {
    case SLE_ERROR:      std::cout << "SLE_ERROR";      break;
    case SLE_MINORERROR: std::cout << "SLE_MINORERROR"; break;
    case SLE_WARNING:    std::cout << "SLE_WARNING";    break;
    }
    std::cout << '\n';

    std::cout << std::flush;
    return base_debug_event_listener::rip_event(pid, tid, info);
}

bool
debug_event_dumper::unload_dll_event(
    psystem::process_id_t const pid, psystem::thread_id_t const tid,
    UNLOAD_DLL_DEBUG_INFO const& info) noexcept
{
    std::cout << "DEBUG EVENT: UNLOAD_DLL_DEBUG_EVENT\n";
    print_common_data(pid, tid);
    std::cout << "\tBase of DLL:      " << ptr(info.lpBaseOfDll) << '\n';

    std::cout << std::flush;
    return base_debug_event_listener::unload_dll_event(pid, tid, info);
}

} // namespace pstack
