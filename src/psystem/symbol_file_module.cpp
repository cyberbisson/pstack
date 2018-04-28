// ===-- src/psystem/symbol_file_module.cpp --------------- -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define a class that examines a COFF executable / library image for
 *        debugging symbols.
 *
 * @author  Matt Bisson
 * @date    8 February, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#include "psystem/symbol_file_module.hpp"

#include <psystem/framework/iomanip.hpp>

#include <psystem/dll_export_view.hpp>
#include <psystem/symbol_view.hpp>

#include <psystem/exception/system_exception.hpp>

using psystem::format::hex;
using psystem::format::mask;
using psystem::format::ptr;

////////////////////////////////////////////////////////////////////////////////
// Static Utilities
////////////////////////////////////////////////////////////////////////////////

/// @brief Data local to this compilation unit
namespace {

/**
 * @def PRINT_CHARACTERISTIC_FLAG(flag)
 * @brief Define a local macro to analyze and display bit flags.
 *
 * This macro is only available within the print_nt_image_header() function.  It
 * references local variables therein.
 *
 * @param flag The @c IMAGE_FILE flag to output (if found in the mask).
 */

/**
 * @brief Output details for a DOS executable image to the console.
 * @param[in] hdr The data-structure to output.
 */
static void
print_dos_image_header(IMAGE_DOS_HEADER const& hdr)
{
    std::cout << "DOS Image Header:\n";
    std::cout << "\tMagic Number: " << hex(hdr.e_magic) << '\n';
    std::cout << "\tInitial IP:   " << hex(hdr.e_ip) << '\n';
    std::cout << "\tInitial SP:   " << hex(hdr.e_sp) << '\n';
    std::cout << "\tNT Header:    " << hex(hdr.e_lfanew) << '\n';
}

/**
 * @brief Output details for an NT executable image to the console.
 *
 * @tparam T The type of structure differs based on platform.  Specify it here.
 * @param[in] hdr The data-structure to output.
 */
template<typename T>
void
print_nt_image_header(T const& hdr)
{
    std::cout << "PE Image Header:\n";
    std::cout << "\tSignature:    " << hex(hdr.Signature) << '\n';

    std::cout << "\tMachine:      " << hex(hdr.FileHeader.Machine);
    switch (hdr.FileHeader.Machine)
    {
    case IMAGE_FILE_MACHINE_I386:
        std::cout << " (x86)\n";
        break;
    case IMAGE_FILE_MACHINE_IA64:
        std::cout << " (Intel Itanium)\n";
        break;
    case IMAGE_FILE_MACHINE_AMD64:
        std::cout << " (x86_64)\n";
        break;
    default:
        std::cout << " (Unknown)\n";
        break;
    }

    std::cout << "\tSections:     "
              << hdr.FileHeader.NumberOfSections << '\n';
    std::cout << "\tSymbol Count: "
              << hdr.FileHeader.NumberOfSymbols << '\n';
    std::cout << "\tCOFF Symbols: "
              << hex(hdr.FileHeader.PointerToSymbolTable) << '\n';

#define PRINT_CHARACTERISTIC_FLAG(flag)                         \
    if ((hdr.FileHeader.Characteristics & IMAGE_FILE_##flag)    \
        == IMAGE_FILE_##flag)                                   \
    {                                                           \
        std::cout << #flag " ";                                 \
    }
    std::cout << "\tAttribs:      ";
    PRINT_CHARACTERISTIC_FLAG(RELOCS_STRIPPED);
    PRINT_CHARACTERISTIC_FLAG(EXECUTABLE_IMAGE);
    PRINT_CHARACTERISTIC_FLAG(LINE_NUMS_STRIPPED);
    PRINT_CHARACTERISTIC_FLAG(LOCAL_SYMS_STRIPPED);
    PRINT_CHARACTERISTIC_FLAG(AGGRESIVE_WS_TRIM);
    PRINT_CHARACTERISTIC_FLAG(LARGE_ADDRESS_AWARE);
    PRINT_CHARACTERISTIC_FLAG(BYTES_REVERSED_LO);
    PRINT_CHARACTERISTIC_FLAG(32BIT_MACHINE);
    PRINT_CHARACTERISTIC_FLAG(DEBUG_STRIPPED);
    PRINT_CHARACTERISTIC_FLAG(REMOVABLE_RUN_FROM_SWAP);
    PRINT_CHARACTERISTIC_FLAG(NET_RUN_FROM_SWAP);
    PRINT_CHARACTERISTIC_FLAG(SYSTEM);
    PRINT_CHARACTERISTIC_FLAG(DLL);
    PRINT_CHARACTERISTIC_FLAG(UP_SYSTEM_ONLY);
    PRINT_CHARACTERISTIC_FLAG(BYTES_REVERSED_HI);
#undef PRINT_CHARACTERISTIC_FLAG
    std::cout << mask(hdr.FileHeader.Characteristics) << '\n';
}

/**
 * @brief Output "optional" details for an NT executable image to the console.
 *
 * @tparam T
 *     The type of structure differs based on platform.  Specify it here.
 * @param[in] hdr
 *     The data-structure to output.
 * @param[in] is_dll
 *     DLL images have a bit more information, so gather that information, and
 *     pass it through this parameter.
 */
template<typename T>
void print_optional_image_header(T const& hdr, bool const is_dll)
{
    std::cout << "PE Optional Header:\n";
    std::cout << "\tMagic Number: " << hex(hdr.Magic) << '\n';
    std::cout << "\tLinker Ver:   "
              << static_cast<uint16_t>(hdr.MajorLinkerVersion) << '.'
              << static_cast<uint16_t>(hdr.MinorLinkerVersion) << '\n';
    std::cout << "\tOS Ver:       "
              << hdr.MajorOperatingSystemVersion << '.'
              << hdr.MinorOperatingSystemVersion << '\n';
    std::cout << "\tImage Ver:    "
              << hdr.MajorImageVersion << '.'
              << hdr.MinorImageVersion << '\n';
    std::cout << "\tSubsys Ver:   "
              << hdr.MajorSubsystemVersion << '.'
              << hdr.MinorSubsystemVersion << '\n';

    std::cout << "\tCode Base:    " << hex(hdr.BaseOfCode) << '\n';
    /// @todo Only for IMAGE_OPTIONAL_HEADER32
//  std::cout << "\tData Base:    " << hex(hdr.BaseOfData) << '\n';
    std::cout << "\tImage Base:   "
              << ptr(reinterpret_cast<void*>(hdr.ImageBase)) << '\n';
    std::cout << "\tChecksum:     " << hex(hdr.CheckSum) << '\n';
    std::cout << "\tSubsystem:    " << hdr.Subsystem << '\n';

    if (is_dll)
    {
        std::cout << "\tDLL Chars:    " << mask(hdr.Subsystem) << '\n';
    }

    std::cout << "\tDirs:         " << hdr.NumberOfRvaAndSizes << '\n';
}

} // namespace <anon>

namespace psystem {

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

symbol_file_module::symbol_file_module(std::string const& file_name)
{
    psystem::unique_handle<> file_handle(
        ::CreateFile(
            file_name.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            0));
    if (!file_handle)
    {
        THROW_WINDOWS_EXCEPTION_F(
            GetLastError(), "Cannot open \"%s\"", file_name.c_str());
    }

    psystem::unique_handle<> mapping_handle(
        ::CreateFileMapping(
            file_handle.get(),
            nullptr,
            PAGE_READONLY,
            0,
            0,
            nullptr));
    if (!mapping_handle)
    {
        THROW_WINDOWS_EXCEPTION_F(
            GetLastError(),
            "Cannot create memory map for file \"%s\"", file_name.c_str());
    }

    std::unique_ptr< uint8_t, std::function<void(uint8_t*)> > view(
        reinterpret_cast<uint8_t*>(
            MapViewOfFile(mapping_handle.get(), FILE_MAP_READ, 0, 0, 0)),
        [](uint8_t* base_address) noexcept
        {
            UnmapViewOfFile(base_address);
        });
    if (!view)
    {
        THROW_WINDOWS_EXCEPTION(
            GetLastError(), "Unable to create view of memory-mapped file.");
    }

    m_view           = std::move(view);
    m_mapping_handle = std::move(mapping_handle);
    m_file_handle    = std::move(file_handle);
}

symbol_file_module::symbol_file_module(symbol_file_module&& o) noexcept
    : m_view(std::move(o.m_view)),
      m_mapping_handle(std::move(o.m_mapping_handle)),
      m_file_handle(std::move(o.m_file_handle)),
      m_symbol_view(std::move(o.m_symbol_view)),
      m_dll_export_view(std::move(o.m_dll_export_view))
{
}

symbol_file_module::~symbol_file_module() noexcept
{
}

////////////////////////////////////////////////////////////////////////////////
// Public Interface
////////////////////////////////////////////////////////////////////////////////

void
symbol_file_module::dump_module_info() const
{
    uint8_t const *const base_address = m_view.get();

    auto const *const dos_header =
        reinterpret_cast<IMAGE_DOS_HEADER const*>(base_address);
    if (IMAGE_DOS_SIGNATURE != dos_header->e_magic)
    {
        /// @todo Better exception
        throw psystem::exception::system_exception(
            __FILE__, __FUNCTION__, __LINE__, 0,
            "Not a Windows executable");
    }

    print_dos_image_header(*dos_header);

    auto const *const nt_header = reinterpret_cast<IMAGE_NT_HEADERS64 const*>(
        base_address + dos_header->e_lfanew);
    if (IMAGE_NT_SIGNATURE != nt_header->Signature)
    {
        /// @todo Better exception
        throw psystem::exception::system_exception(
            __FILE__, __FUNCTION__, __LINE__, 0,
            "Not a Portable Executable (PE) image");
    }

    print_nt_image_header(*nt_header);

    auto const *const symbol_start =
        base_address
        + nt_header->FileHeader.PointerToSymbolTable;
    std::cout << "\tSymbol Start: " << ptr(symbol_start) << '\n';

    auto const *const string_start =
        symbol_start
        + (static_cast<uint64_t>(nt_header->FileHeader.NumberOfSymbols)
           * sizeof(IMAGE_SYMBOL));
    std::cout << "\tString Table: " << ptr(string_start) << '\n';

    IMAGE_OPTIONAL_HEADER64 const& opt_header = nt_header->OptionalHeader;
    if (IMAGE_NT_OPTIONAL_HDR64_MAGIC != opt_header.Magic)
    {
        /// @todo Better exception
        throw psystem::exception::system_exception(
            __FILE__, __FUNCTION__, __LINE__, 0,
            "PE optional image header not present");
    }

    print_optional_image_header(
        opt_header,
        (nt_header->FileHeader.Characteristics & IMAGE_FILE_DLL)
        == IMAGE_FILE_DLL);
}

uint8_t const *
symbol_file_module::get_base_address() const noexcept
{
    return m_view.get();
}

dll_export_view const *
symbol_file_module::get_dll_export_view()
{
    if (m_dll_export_view)
    {
        return m_dll_export_view.get();
    }

    auto const *const base_address = m_view.get();

    auto const *const dos_header =
        reinterpret_cast<IMAGE_DOS_HEADER const*>(base_address);
    if (IMAGE_DOS_SIGNATURE != dos_header->e_magic)
    {
        /// @todo Better exception
        throw psystem::exception::system_exception(
            __FILE__, __FUNCTION__, __LINE__, 0,
            "Not a Windows executable");
    }

    auto const *const nt_header = reinterpret_cast<IMAGE_NT_HEADERS64 const*>(
        base_address + dos_header->e_lfanew);
    if (IMAGE_NT_SIGNATURE != nt_header->Signature)
    {
        /// @todo Better exception
        throw psystem::exception::system_exception(
            __FILE__, __FUNCTION__, __LINE__, 0,
            "Not a Portable Executable (PE) image");
    }

    if (nt_header->OptionalHeader.NumberOfRvaAndSizes > 0)
    {
        IMAGE_DATA_DIRECTORY const *const export_dir_ptr =
            &(nt_header->OptionalHeader
              .DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);

        if (0 >= export_dir_ptr->Size)
        {
            return nullptr;
        }

        IMAGE_SECTION_HEADER const *img_section =
            IMAGE_FIRST_SECTION(nt_header);
        while (export_dir_ptr->VirtualAddress >
               (img_section->Misc.VirtualSize + img_section->VirtualAddress))
        {
            ++img_section;
        }

        auto const va_offset =
            static_cast<integral_address_t>(img_section->PointerToRawData) -
            img_section->VirtualAddress;

        auto const export_dir =
            reinterpret_cast<IMAGE_EXPORT_DIRECTORY const *>(
                base_address +
                export_dir_ptr->VirtualAddress +
                va_offset);

        auto const idx_with_name = reinterpret_cast<uint16_t const*>(
            base_address +
            export_dir->AddressOfNameOrdinals +
            va_offset);

        auto const export_rva = reinterpret_cast<uint32_t const*>(
            base_address +
            export_dir->AddressOfFunctions +
            va_offset);

        auto const export_name_rva = reinterpret_cast<uint32_t const*>(
            base_address +
            export_dir->AddressOfNames +
            va_offset);

        auto const number_of_functions = export_dir->NumberOfFunctions;
        auto const number_of_names = export_dir->NumberOfNames;

        m_dll_export_view = std::make_unique<dll_export_view>(
            base_address,
            reinterpret_cast<uint8_t const*>(
                nt_header->OptionalHeader.ImageBase),
            va_offset,
            idx_with_name,
            export_rva,
            export_name_rva,
            number_of_functions,
            number_of_names);
        return m_dll_export_view.get();
    }

    return nullptr;
}

symbol_view const *
symbol_file_module::get_symbol_view()
{
    auto const *const base_address = m_view.get();

    auto const *const dos_header =
        reinterpret_cast<IMAGE_DOS_HEADER const*>(base_address);
    if (IMAGE_DOS_SIGNATURE != dos_header->e_magic)
    {
        /// @todo Better exception
        throw psystem::exception::system_exception(
            __FILE__, __FUNCTION__, __LINE__, 0,
            "Not a Windows executable");
    }

    auto const *const nt_header =
        reinterpret_cast<IMAGE_NT_HEADERS64 const*>(
            base_address + dos_header->e_lfanew);
    if (IMAGE_NT_SIGNATURE != nt_header->Signature)
    {
        /// @todo Better exception
        throw psystem::exception::system_exception(
            __FILE__, __FUNCTION__, __LINE__, 0,
            "Not a Portable Executable (PE) image");
    }

    IMAGE_OPTIONAL_HEADER64 const& opt_header = nt_header->OptionalHeader;
    if (IMAGE_NT_OPTIONAL_HDR64_MAGIC != opt_header.Magic)
    {
        /// @todo Better exception
        throw psystem::exception::system_exception(
            __FILE__, __FUNCTION__, __LINE__, 0,
            "PE optional image header not present");
    }

    if (((nt_header->FileHeader.Characteristics &
          IMAGE_FILE_LOCAL_SYMS_STRIPPED) == 0) &&
        (0 < nt_header->FileHeader.NumberOfSymbols) &&
        (nt_header->FileHeader.PointerToSymbolTable))
    {
        auto const *const symbol_start =
            base_address + nt_header->FileHeader.PointerToSymbolTable;

        auto const *const string_start =
            symbol_start
            + (static_cast<uint64_t>(nt_header->FileHeader.NumberOfSymbols)
               * sizeof(IMAGE_SYMBOL));

        m_symbol_view = std::make_unique<symbol_view>(
            reinterpret_cast<address_t>(symbol_start),
            reinterpret_cast<char const *>(string_start),
            opt_header.BaseOfCode + opt_header.ImageBase);
    }

    return m_symbol_view.get();
}

} // namespace psystem
