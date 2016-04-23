// ===-- include/psystem/symbol_file_module.hpp ----------- -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare a class that examines a COFF executable / library image for
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

#pragma once
#ifndef PSYSTEM_SYMBOL_FILE_MODULE_HPP
#define PSYSTEM_SYMBOL_FILE_MODULE_HPP

#include "psystem_ns.hpp"

#include <functional>

#include <psystem/framework/memory.hpp>
#include <psystem/framework/not_copyable.hpp>
#include <psystem/framework/unique_handle.hpp>

namespace psystem {

class dll_export_view;
class symbol_view;

/**
 * @brief Encapsulates a module (file) for the purpose of gathering debugging
 *        symbols.
 *
 * This class maps a physical file containing executable code or debugging
 * symbols into memory, and provides views into both debugging symbols and
 * exported (DLL) symbols.  Currently, this only supports executable and DLL
 * images.
 *
 * @author  Matt Bisson
 * @date    8 February, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
class symbol_file_module
    : psystem::not_copyable
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct an instance by loading @p file_name.
     *
     * This method maps an executable module into memory for later examination.
     * It makes no validation of the contents of the file during construction,
     * so later invocations may decide that @p file_name is not applicable for
     * their purposes.
     *
     * @param[in] file_name
     *     The path of a file that will be checked for symbols.  This may be a
     *     full path or simply a file; normal Win32 name resolution applies.
     *
     * @throws psystem::exception::system_exception
     *     If the file specified by @p file_name did not exist, or could not be
     *     mapped into memory.
     */
    explicit symbol_file_module(std::string const& file_name);

    /**
     * @brief Move from an object of this type.
     *
     * @param[in,out] o The instance from which data will be moved.
     *
     * @post
     *     Ownership of all "view" instances from @p o, transfer to this
     *     instance.
     */
    symbol_file_module(symbol_file_module&& o) noexcept;

    /**
     * @brief Clean up resources used by the symbol_file_module instance.
     *
     * @post
     *     The destruction of this class also destroys all "view" instances
     *     acquired from it.
     *
     * @todo Remove noexcept on DTOR
     */
    ~symbol_file_module() noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Prints executable image headers to the console.
     *
     * @throws psystem::exception::system_exception
     *     If the file backing this instance is not a Windows Portable
     *     Executable (PE), or the "optional" header is not present.
     * @todo Better exception.
     */
    void dump_module_info() const;

    /**
     * @brief Acquire the base memory address where this module has been loaded.
     *
     * Acquiring this "base" address gives a readable view of the raw
     * bits the exist in the module that this instance represents.
     *
     * @return
     *     The memory address pointing to the loaded module.  This never returns
     *     @c NULL.
     */
    uint8_t const *get_base_address() const noexcept;

    /**
     * @brief Obtain a "view" into the exported DLL symbols for this module.
     * @return This method may return @c NULL if no export table exists.
     *
     * @throws psystem::exception::system_exception
     *     If the file backing this instance is not a Windows Portable
     *     Executable (PE).
     * @todo Better exception.
     */
    dll_export_view const *get_dll_export_view();

    /**
     * @brief Obtain a "view" into the debugging symbols for this module.
     * @return This method may return @c NULL if there is no debugging
     *         information.
     *
     * @throws psystem::exception::system_exception
     *     If the file backing this instance is not a Windows Portable
     *     Executable (PE), or the "optional" header is not present.
     * @todo Better exception.
     */
    symbol_view const *get_symbol_view();

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    //
    // Resources consumed for the lifetime of this instance:
    //

    /// @brief Address of the bytes for the memory mapped file.
    std::unique_ptr< uint8_t, std::function<void(uint8_t*)> > m_view;

    /// @brief System handle to the memory mapping.
    psystem::unique_handle<> m_mapping_handle;

    /// @brief Open handle to the file-system file (to be mapped).
    psystem::unique_handle<> m_file_handle;

    //
    // Lazily loaded resources:
    //

    /// @brief Loaded view of debugging symbols (may be @c NULL).
    std::unique_ptr<symbol_view> m_symbol_view;

    /// @brief Loaded view of DLL exports (may be @c NULL).
    std::unique_ptr<dll_export_view> m_dll_export_view;
};

} // namespace psystem

#endif // PSYSTEM_SYMBOL_FILE_MODULE_HPP
