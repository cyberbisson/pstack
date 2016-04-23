// ===-- src/psystem/framwork/shared_library.hpp ---------- -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define basic functionality for loading shared libraries on-demand.
 *
 * @author  Matt Bisson
 * @date    19 November, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 *
 * @todo Create ImgHelp and DbgHelp subclasses for psystem::shared_library.
 *
 * @todo Create an init_psystem function to explicitly load the
 * libraries, and check their required versions.
 *
 * @todo Statically declare the imagehelp and dbghelp instances so
 * they are unloaded regardless of whether they are loaded.
 *
 * @todo Do we MutEx around load and unload to protect clearing out of
 * m_functions?
 *
 * @todo Do we load DLLs lazily instead of explicitly?  This would
 * require checking the shared_handle every time a function is called.
 * Similarly, do we load functions lazily?  Do determine laziness via
 * preproc flag or something?
 */

#include "psystem/framework/shared_library.hpp"

#include <psystem/framework/iomanip.hpp>
#include <psystem/framework/memory.hpp>

#include <psystem/exception/system_exception.hpp>

using psystem::format::hex;
using psystem::format::mask;

/**
 * @brief Dumps version information for an executable to @c stdout.
 *
 * @param[in] ffi The structure containing version information to be displayed.
 */
static void
dump_version_info(VS_FIXEDFILEINFO const& ffi) noexcept
{
    std::cout << "\tSignature:       " << hex(ffi.dwSignature) << '\n';

    std::cout << "\tVersion Struct:  "
              << (ffi.dwStrucVersion >> 16)    << '.'
              << (ffi.dwStrucVersion & 0xFFFF) << '\n';
    std::cout << "\tFile Version:    "
              << (ffi.dwFileVersionMS >> 16)    << '.'
              << (ffi.dwFileVersionMS & 0xFFFF) << '.'
              << (ffi.dwFileVersionLS >> 16)    << '.'
              << (ffi.dwFileVersionLS & 0xFFFF) << '\n';
    std::cout << "\tProduct Version: "
              << (ffi.dwProductVersionMS >> 16)    << '.'
              << (ffi.dwProductVersionMS & 0xFFFF) << '.'
              << (ffi.dwProductVersionLS >> 16)    << '.'
              << (ffi.dwProductVersionLS & 0xFFFF) << '\n';

    std::cout << "\tFile-Flags Mask: " << mask(ffi.dwFileFlagsMask) << " ( ";
    {
        auto const file_flags = ffi.dwFileFlags & ffi.dwFileFlagsMask;

        if (VS_FF_DEBUG        & file_flags) std::cout << "DEBUG ";
        if (VS_FF_PRERELEASE   & file_flags) std::cout << "PRERELEASE ";
        if (VS_FF_PATCHED      & file_flags) std::cout << "PATCHED ";
        if (VS_FF_PRIVATEBUILD & file_flags) std::cout << "PRIVATEBUILD ";
        if (VS_FF_INFOINFERRED & file_flags) std::cout << "INFOINFERRED ";
        if (VS_FF_SPECIALBUILD & file_flags) std::cout << "SPECIALBUILD ";
    }
    std::cout << ")\n";

    std::cout << "\tIntended OS:     ";
    switch (ffi.dwFileOS)
    {
    case VOS_DOS:           std::cout << "MS-DOS"; break;
    case VOS_OS216:         std::cout << "OS/2 16-bit"; break;
    case VOS_OS232:         std::cout << "OS/2 32-bit"; break;
    case VOS_NT:            std::cout << "Windows NT"; break;
    case VOS_WINCE:         std::cout << "Windows CE"; break;
    case VOS__WINDOWS16:    std::cout << "Windows 16-bit"; break;
    case VOS__PM16:         std::cout << "Presentation Mgr 16-bit"; break;
    case VOS__PM32:         std::cout << "Presentation Mgr 32-bit"; break;
    case VOS__WINDOWS32:    std::cout << "Windows 32-bit"; break;
    case VOS_DOS_WINDOWS16: std::cout << "Windows on MS-DOS 16-bit"; break;
    case VOS_DOS_WINDOWS32: std::cout << "Windows on MS-DOS 32-bit"; break;
    case VOS_OS216_PM16:    std::cout << "OS/2 on Present. Mgr 16-bit"; break;
    case VOS_OS232_PM32:    std::cout << "OS/2 on Present. Mgr 32-bit"; break;
    case VOS_NT_WINDOWS32:  std::cout << "Windows NT 32-bit"; break;
    case VOS_UNKNOWN:
    default:
        std::cout << "Unknown (" << mask(ffi.dwFileOS) << ")";
    }
    std::cout << '\n';

    std::cout << "\tFile Type:       ";
    switch (ffi.dwFileType)
    {
    case VFT_APP:        std::cout << "Application";    break;
    case VFT_DLL:        std::cout << "DLL";            break;
    case VFT_STATIC_LIB: std::cout << "Static Library"; break;

        // I don't care enough (yet) to fill in the subtypes...
    case VFT_DRV:
        std::cout << "Device Driver\n";
        std::cout << "\tFile Subtype:    " << hex(ffi.dwFileSubtype);
        break;
    case VFT_FONT:
        std::cout << "Font\n";
        std::cout << "\tFile Subtype:    " << hex(ffi.dwFileSubtype);
        break;
    case VFT_VXD:
        std::cout << "Virtual Device\n";
        std::cout << "\tFile Subtype:    " << hex(ffi.dwFileSubtype);
        break;
    case VFT_UNKNOWN:
    default:
        std::cout << "Unknown (" << hex(ffi.dwFileType) << ")";
        break;
    }
    std::cout << '\n';

    std::cout << "\tDate:            ";
    {
        uint64_t file_date = ffi.dwFileDateMS;
        file_date <<= sizeof(ffi.dwFileDateMS);
        file_date |= ffi.dwFileDateLS;
        std::cout << hex(file_date) << '\n';
    }
}

namespace psystem {

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

shared_library::shared_library(
    std::string library_name,
    size_t const num_functions) noexcept
    : m_functions(num_functions, nullptr),
      m_library_name(std::move(library_name))
{
}

shared_library::~shared_library() noexcept
{
}

////////////////////////////////////////////////////////////////////////////////
// Public Interface
////////////////////////////////////////////////////////////////////////////////

shared_library::library_version
shared_library::get_version() const
{
    auto const version_info_sz =
        ::GetFileVersionInfoSize(m_library_name.c_str(), nullptr);
    if (0 == version_info_sz)
    {
        THROW_WINDOWS_EXCEPTION_F(
            GetLastError(), "Error getting DLL version info (size): \"%s\"",
            m_library_name.c_str());
    }

    std::unique_ptr<uint8_t[]> version_info_buf(new uint8_t[version_info_sz]);

    if (!::GetFileVersionInfo(
            m_library_name.c_str(),
            0,
            version_info_sz,
            version_info_buf.get()))
    {
        THROW_WINDOWS_EXCEPTION_F(
            GetLastError(), "Error getting DLL version information: \"%s\"",
            m_library_name.c_str());
    }

    VS_FIXEDFILEINFO *file_info      = nullptr;
    UINT              file_info_size = 0;
    if (!::VerQueryValue(
            version_info_buf.get(),
            TEXT("\\"),
            reinterpret_cast<void**>(&file_info), &file_info_size))
    {
        THROW_WINDOWS_EXCEPTION_F(
            GetLastError(), "Error querying DLL version information: \"%s\"",
            m_library_name.c_str());
    }

    // Check the signature to be sure we're looking at proper data.
    if (VS_FFI_SIGNATURE != file_info->dwSignature)
    {
        /// @todo Better exception
        throw psystem::exception::system_exception(
            __FILE__, __FUNCTION__, __LINE__, 0,
            "DLL has incorrect signature (is not a valid file): \"%s\"",
            m_library_name.c_str());
    }

    static_assert(
        sizeof(file_info->dwFileVersionMS) == sizeof(library_version),
        "Library version type does not match Windows API type.");

#ifdef _DEBUG
    std::cout << "Version info for \"" << m_library_name << "\"\n";
    dump_version_info(*file_info);
#endif

    return file_info->dwFileVersionMS;
}

bool
shared_library::is_loaded() const noexcept
{
    return static_cast<bool>(m_library_handle);
}

void
shared_library::load()
{
    ASSERT(!is_loaded());

    /// @bug shared_handle::decrement causes us to lose the deleter (as the
    /// control block is deleted).  The managed_handle_proxy (triggered from
    /// operator&), however only calls the version of shared_handle::reset()
    /// that does not take a custom deleter.
    HMODULE module_handle = ::LoadLibrary(m_library_name.c_str());
    if (!module_handle)
    {
        THROW_WINDOWS_EXCEPTION_F(
            GetLastError(), "Cannot load DLL: \"%s\"", m_library_name.c_str());
    }

    m_library_handle.reset(module_handle, &::FreeLibrary);
}

void
shared_library::unload() noexcept
{
    std::fill(m_functions.begin(), m_functions.end(), nullptr);
    m_library_handle.reset();
}

////////////////////////////////////////////////////////////////////////////////
// Inherited Interface
////////////////////////////////////////////////////////////////////////////////

FARPROC
shared_library::get_function(char const *export_name) const
{
    ASSERT(export_name);
    ASSERT(is_loaded());

    FARPROC const proc_address =
        ::GetProcAddress(m_library_handle.get(), export_name);
    if (!proc_address)
    {
        THROW_WINDOWS_EXCEPTION_F(
            GetLastError(), "Cannot find \"%s\" in DLL \"%s\"",
            export_name, m_library_name.c_str());
    }

    return proc_address;
}

} // namespace psystem
