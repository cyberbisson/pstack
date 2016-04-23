// ===-- src/addr2ln/addr2ln.hpp -------------------------- -*- C++ -*- --=== //
// Copyright (c) 2016 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief The main entry point for the Addr2Ln executable.
 *
 * @author  Matt Bisson
 * @date    7 July, 2008 -- 4 March, 2016
 * @since   Addr2ln 0.1
 * @version Addr2ln 0.1
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#include <cstring>

#include <psystem/framework/iomanip.hpp>
#include <psystem/framework/memory.hpp>

#include <addr2ln/addr2ln_options.hpp>

#include <psystem/dll_export_iterator.hpp>
#include <psystem/dll_export_view.hpp>
#include <psystem/image_symbol_iterator.hpp>
#include <psystem/symbol.hpp>
#include <psystem/symbol_file_module.hpp>
#include <psystem/symbol_view.hpp>

#include <psystem/exception/base_exception.hpp>
#include <psystem/exception/system_exception.hpp>
#include <psystem/exception/ui_exception.hpp>

using psystem::format::hex;
using psystem::format::ptr;

/// @todo Remove a bunch of unneeded casts for pointer arithmetic throughout the
/// code.

////////////////////////////////////////////////////////////////////////////////
// Local Function Declarations
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Look for a symbol in a module's debug information.
 *
 * @param[in] sym_view
 *     The "view" instance to the debugging symbol information.  This may be
 *     @c nullptr.  If so, the function will interpret this as a no-op.
 * @param[in] module_base
 *     (Optional) A pointer to the address that specifies the base-address for
 *     @p file_name if it were loaded (e.g., as a DLL).  This may be @c nullptr
 *     if you wish to use the "default" base address, which is usually encoded
 *     in the module itself.
 * @param[in] address
 *     The address to check for a symbol.
 *
 * @return
 *     A pointer to the symbol that was found at (or near) the address, or
 *     @c nullptr if none was found.
 *
 * @throws psystem::exception::system_exception
 *     If the file could not be opened, or if it is not a recognized file type.
 */
static std::unique_ptr<psystem::symbol> check_debug_symbols(
    psystem::symbol_view const *sym_view,
    psystem::address_t const *module_base,
    psystem::address_t const address);

/**
 * @brief Assuming a DLL, look for a symbol in the DLL exports table.
 *
 * @param[in] exp_view
 *     The "view" instance any DLL export information.  This may be @c nullptr.
 *     If so, the function will interpret this as a no-op.
 * @param[in] module_base
 *     (Optional) A pointer to the address that specifies the base-address for
 *     @p file_name if it were loaded (e.g., as a DLL).  This may be @c nullptr
 *     if you wish to use the "default" base address, which is usually encoded
 *     in the module itself.
 * @param[in] address
 *     The address to check for a symbol.
 *
 * @return
 *     A pointer to the symbol that was found at (or near) the address, or
 *     @c nullptr if none was found.
 *
 * @throws psystem::exception::system_exception
 *     If the file could not be opened, or if it is not a recognized file type.
 */
static std::unique_ptr<psystem::symbol> check_dll_exports(
    psystem::dll_export_view const *exp_view,
    psystem::address_t const *module_base,
    psystem::address_t const address);

/**
 * @brief Open an executable module and dump the list of symbols to @c stdout.
 *
 * This is a debugging function.  If the specified file does not point to a
 * recognized file type, this method simply throws an exception.  If it contains
 * no symbols, the function succeeds, but does not print any output.
 *
 * @param[in] file_name The file to open, examine, and dump contents.
 * @throws psystem::exception::system_exception
 *     If the file could not be opened, or if it is not a recognized file type.
 */
static void dump_symbols(std::string const& file_name);

/**
 * @brief Given an executable file, find the symbol that resides at the given
 *        address.
 *
 * This function checks both the debugging information and the DLL exports (if
 * either are present).
 *
 * As a side-effect of this method, the given file may be mapped into memory and
 * un-mapped.  The method caches nothing.
 *
 * @param[in] file_name
 *     The file to open and examine.
 * @param[in] module_base
 *     (Optional) A pointer to the address that specifies the base-address for
 *     @p file_name if it were loaded (e.g., as a DLL).  This may be @c nullptr
 *     if you wish to use the "default" base address, which is usually encoded
 *     in the module itself.
 * @param[in] address
 *     The address to check for a symbol.
 *
 * @return
 *     A pointer to the symbol that was found at (or near) the address, or
 *     @c nullptr if none was found.
 *
 * @throws psystem::exception::system_exception
 *     If the file could not be opened, or if it is not a recognized file type.
 */
static std::unique_ptr<psystem::symbol> find_symbol(
    std::string const& file_name,
    psystem::address_t const *module_base,
    psystem::address_t const address);

////////////////////////////////////////////////////////////////////////////////
// Local Function Definitions
////////////////////////////////////////////////////////////////////////////////

/*static*/ std::unique_ptr<psystem::symbol>
check_debug_symbols(
    psystem::symbol_view const *const sym_view,
    psystem::address_t const *module_base,
    psystem::address_t const address)
{
    if (!sym_view) { return nullptr; }

#ifdef DEBUG
    std::cout << "Checking debug symbols...\n";
#endif

    auto const dll_base =
        (module_base) ? *module_base : sym_view->m_module_base;

    IMAGE_SYMBOL const* nearest_sym = nullptr;

    for (IMAGE_SYMBOL const& sym : *sym_view)
    {
        if ((!sym.Value) || (!ISFCN(sym.Type))) continue;

        auto const cur_addr = dll_base + sym.Value;
        if (cur_addr == address)
        {
            // An exact match; we're done.
            nearest_sym = &sym;
            break;
        }
        if (cur_addr > address)
        {
            // Address is before the symbol -- can't be right
            continue;
        }

        if (nearest_sym &&
            cur_addr > (dll_base + nearest_sym->Value))
        {
            // Current symbol is not as close as "nearest" from prior iteration
            continue;
        }

        nearest_sym = &sym;
    }

    if (!nearest_sym)
    {
        return nullptr;
    }

    std::string sym_name;
    if (nearest_sym->N.Name.Short)
    {
        char short_name[9];

        memcpy(short_name, nearest_sym->N.ShortName, 8);
        short_name[8] = '\0';
        sym_name = short_name;
    }
    else
    {
        sym_name =
            sym_view->m_string_start + nearest_sym->N.Name.Long;
    }

    return std::make_unique<psystem::symbol>(
        dll_base + nearest_sym->Value,
        address - (dll_base + nearest_sym->Value),
        std::move(sym_name));
}

/*static*/ std::unique_ptr<psystem::symbol>
check_dll_exports(
    psystem::dll_export_view const *const exp_view,
    psystem::address_t const *module_base,
    psystem::address_t const address)
{
    if (!exp_view) { return nullptr; }

#ifdef DEBUG
    std::cout << "Checking DLL exports...\n";
#endif

    auto const dll_base =
        (module_base)
        ? *module_base
        : exp_view->get_symbol_base_address();

    psystem::dll_export_iterator const it_end = exp_view->end();
    psystem::dll_export_iterator nearest_it = it_end;
    for (psystem::dll_export_iterator it = exp_view->begin();
         it != it_end;
         ++it)
    {
        if (*it.m_idx_with_name != it.m_current_idx) continue;

        // We found exactly what we wanted, so short-circuit.
        if (dll_base + *it.m_export_rva == address)
        {
            nearest_it = it;
            break;
        }

        // The start of the symbol is beyond the address we want, so it can't be
        // correct.
        if (dll_base + *it.m_export_rva > address) continue;

        if (nearest_it != it_end &&
            ((*it.m_export_rva < *nearest_it.m_export_rva) ||
             (*it.m_export_rva == *nearest_it.m_export_rva)))
        {
            // Current symbol is not as close as "nearest" from prior
            // iteration...
            //
            // ... OR we have duplicate addresses with different names.
            // Let's just choose the first one.
            continue;
        }

        nearest_it = it;
    }

    if (nearest_it == it_end)
    {
        return nullptr;
    }

    return std::make_unique<psystem::symbol>(
        dll_base + *nearest_it.m_export_rva,
        address -(dll_base + *nearest_it.m_export_rva),
        nearest_it.get_export_name());
}

/*static*/ void
dump_symbols(std::string const& file_name)
{
    psystem::symbol_file_module dll(file_name);

    std::cout << " -- " << file_name << " --\n";
    dll.dump_module_info();

    psystem::symbol_view const *const sym_view = dll.get_symbol_view();
    if (sym_view)
    {
        for (IMAGE_SYMBOL const& sym : *sym_view)
        {
            if ((!sym.Value) || (!ISFCN(sym.Type))) continue;

            std::cout << "RVA: " << ptr(sym_view->m_module_base + sym.Value);

            if (sym.N.Name.Short)
            {
                char short_name[9];

                memcpy(short_name, sym.N.ShortName, 8);
                short_name[8] = '\0';

                std::cout << " - " << short_name;
            }
            else
            {
                std::cout << " - " << sym_view->m_string_start + sym.N.Name.Long;
            }

            std::cout <<'\n';
        }
    }

    std::cout << "--- EXPORTS ---\n";
    psystem::dll_export_view const *const exp_view = dll.get_dll_export_view();
    if (exp_view)
    {
        psystem::dll_export_iterator const it_end = exp_view->end();
        for (psystem::dll_export_iterator it = exp_view->begin();
             it != it_end;
             ++it)
        {
            if (*it.m_idx_with_name != it.m_current_idx) continue;

            std::cout << "RVA: "
                      << ptr(*it.m_export_rva) << " - " << it.get_export_name()
                      << '\n';
        }
    }
}

/*static*/ std::unique_ptr<psystem::symbol>
find_symbol(
    std::string const& file_name,
    psystem::address_t const *module_base,
    psystem::address_t const address)
{
#ifdef DEBUG
    std::cout << "---: " << ptr(address) << '\n';
#endif

    psystem::symbol_file_module dll(file_name);

    auto debug_sym =
        check_debug_symbols(dll.get_symbol_view(), module_base, address);
    auto dll_sym =
        check_dll_exports(dll.get_dll_export_view(), module_base, address);

    if (!debug_sym && !dll_sym)
    {
        return nullptr;
    }
    if (!debug_sym)
    {
        return dll_sym;
    }
    else if (!dll_sym)
    {
        return debug_sym;
    }

    return ((debug_sym->get_code_offset() <= dll_sym->get_code_offset())
            ? std::move(debug_sym)
            : std::move(dll_sym));
}

////////////////////////////////////////////////////////////////////////////////
// Application Entry-point
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief The main entry point for the Addr2Ln application.
 *
 * See addr2ln::addr2ln_options for details on supported command-line options.
 *
 * @param[in] argc The count of command-line arguments.
 * @param[in] argv The array of strings that specify command-line arguments.
 *
 * @return
 *     This is the return code that is passed to the parent process.  The value
 *     is as follows:
 *     @li 0:  Successful execution of all specified workflows.
 *     @li -1: Invalid command-line option passed.
 *     @li -2: An "expected" exception caused a failure to execute the workflow.
 *     @li -3: An unexpected exception caused a failure to execute the workflow.
 *     @li -4: An unknown error caused a failure to execute the workflow.
 *     @li >0: A system-native error occured.  The result is the system error
 *             code.
 */
int
main(int argc, char const *argv[])
try
{
    using namespace addr2ln;

    // Options' lifetime coexists with main process flow
    std::unique_ptr<addr2ln_options> options;

    try
    {
        // Only main has CTOR access... make_unique not usable here.
        options.reset(new addr2ln_options(argc, argv));
    }
    catch (psystem::exception::ui_exception const& uex)
    {
        ASSERT(argv);
        ASSERT(argv[0]);

        std::cerr << argv[0] << ": " << uex << '\n';
        return -1;
    }
    catch (bool)
    {
        return 0;
    }

    /* HERE IS SOME TEST DATA BASED ON MY LOCAL SYSTEM (Windows 7 Workstation):
     *
     * .\\out\\Debug-x64\\test_application.exe
     *       0x000000013fc787c3 - std::_Cnd_waitX[+0x33]
     * C:\\windows\\system32\\msvcr120d.dll
     *       0x000007fed91ba357 - endthreadex[+0x1d7]
     * C:\\windows\\system32\\kernel32.dll
     *       0x0000000077815a4d - BaseThreadInitThunk[+0xd]
     * C:\\windows\\system32\\ntdll.dll
     *       0x0000000077a4b831 - RtlUserThreadStart[+0x21]
     * C:\\cygwin64\\bin\\cygwin1.dll
     *       ?                  - ?
     *
     * Process ID:       20236
     * Active thread ID: 21708
     * DLL Name:         hFile: "C:\Windows\System32\ntdll.dll"
     * Base of DLL:      0x0000000077a20000
     * Debug Info Offs:  0x0
     * Debug Info Size:  0
     *
     * static: (nt_header->OptionalHeader)
     * BaseOfCode = 0x0000000000001000
     * ImageBase  = 0x0000000078e50000
     *
     * - Found NtWaitForSingleObject at: 0x0000000078e9d9f0
     * + ntdll           0x0000000077a6d9fa - NtWaitForSingleObject [+0xa]
     * = NtWaitForSingleObject @ 0x0000000077a6d9ee
     * ---> Incorrect by 0x0000000001430002
     */
    // TRY: addr2ln "C:\windows\system32\ntdll.dll" /b 77a20000 77a6d9fa

#ifdef DUMP_SYMBOLS
    dump_symbols(addr2ln_options::get_module_name());
#endif

    for (auto const addr : addr2ln_options::get_address_list())
    {
        auto const found = find_symbol(
            addr2ln_options::get_module_name(),
            addr2ln_options::get_module_base_address(),
            addr);
        if (!found)
        {
            std::cout << ptr(addr) << " - NOT FOUND.\n";
            continue;
        }

        std::cout << ptr(addr) << " - " << found->get_name();
        if (found->get_code_offset() > 0)
        {
            std::cout << " [+" << hex(found->get_code_offset()) << "]";
        }
        std::cout << '\n';
    }

    return 0;
}
catch (psystem::exception::system_exception const& sex)
{
    std::cerr << argv[0] << ": " << sex << '\n';
    return sex.error_code();
}
catch (psystem::exception::base_exception const& bex)
{
    std::cerr << argv[0] << ": " << bex << '\n';
    return -2;
}
catch (std::exception const& ex)
{
    std::cerr
        << argv[0] << ": " << ex.what() << '\n';
    return -3;
}
catch (...)
{
    return -4;
}
