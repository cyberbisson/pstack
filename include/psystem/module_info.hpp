// ===-- include/psystem/module_info.hpp ------------------ -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare a container for process and shared-library module information.
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

#pragma once
#ifndef PSYSTEM_MODULE_INFO_HPP
#define PSYSTEM_MODULE_INFO_HPP

#include "psystem_ns.hpp"

#include <mutex>
#include <string>

#include <psystem/framework/shared_handle.hpp>

namespace psystem {

/**
 * @brief Contains information about an executable image or shared library.
 *
 * In order to have a useful debugging experience, a debugger needs to be aware
 * of all the modules used by an application, as well as where in memory they
 * reside.  This provides information about individual symbols, as well as a
 * more reliable stack trace.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @todo Get module names
 * @todo Move operations
 * @todo Pass in needed data to be more platform compatible?
 */
class module_info
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct a module_info instance for a process's executable.
     *
     * @param[in] file_handle
     *     The handle to the file that relates to this code module.
     * @param[in] info
     *     The full set of information about the module.
     */
    module_info(
        psystem::shared_handle<> const& file_handle,
        CREATE_PROCESS_DEBUG_INFO const& info) noexcept;

    /**
     * @brief Construct a module_info instance for a shared library.
     *
     * @param[in] file_handle
     *     The handle to the file that relates to this code module.
     * @param[in] info
     *     The full set of information about the module.
     */
    module_info(
        psystem::shared_handle<> const& file_handle,
        LOAD_DLL_DEBUG_INFO const& info) noexcept;

    /**
     * @brief Move from an object of this type.
     *
     * @param[in,out] o The instance from which data will be moved.
     */
    module_info(module_info&& o) noexcept;

    /// @brief Clean up resources used by the module_info instance.
    ~module_info() noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Access the base memory address for this module.
     *
     * This information is useful for determining symbol information based on a
     * memory address (e.g., a PC location in a stack trace).  Once the module
     * is determined, the symbol information can be gathered.
     *
     * @return A memory address to the loaded module (as it exists in the
     *         debuggee process's address space).
     */
    address_t get_base_of_image() const noexcept;

    /**
     * @brief Access the module's full file name.
     * @return The path and name of the file that makes up this module.  The
     *     return value is valid until this instance is destroyed.
     */
    std::string const& get_file_name() const noexcept;

    /**
     * @brief Access the module's base name.
     * @return The "base" name of the module.  This is not a full path.  The
     *     return value is valid until this instance is destroyed.
     */
    std::string const& get_name() const noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Private Utilites
    ////////////////////////////////////////////////////////////////////////////
private:
    /**
     * @brief Lazy initialization used by the get_file_name() method.
     *
     * Given a file handle, this populates the @p out parameter with the name of
     * that file.
     *
     * @param[in] file_handle
     *     An open file to examine.
     * @param[in,out] out
     *     The buffer to receive the name associated with @p file_handle.  This
     *     may be @c nullptr (the method does nothing).
     */
    static void init_file_name(HANDLE file_handle, std::string *out) noexcept;

    /**
     * @brief Lazy initialization used by the get_name() method.
     *
     * Given a file name, this populates the @p out parameter with the
     * module-name associated with the file.
     *
     * @param[in] file_name The module's file name.
     * @param[in,out] out   The buffer to receive the module name.  This may be
     *                      @c nullptr (the method does nothing).
     */
    static void init_name(
        std::string const& file_name,
        std::string *out) noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief Shared ownership of the handle to the memory-mapped module.
    psystem::shared_handle<> m_file_handle;

    /// @brief The base memory address into the module's data.
    address_t m_base_of_image;

    /**
     * @brief The file name of the library / executable that defines this
     *        module.
     *
     * This is expensive to generate, and is only created lazily.
     */
    std::string mutable m_file_name;

    /**
     * @brief Mutex for lazy generation of module_info::m_file_name.
     *
     * The module_info::m_file_name data is generated by the @c const member
     * function, get_file_name().  Mutable data altered in a @c const context
     * should always be protected by a threading guard.
     */
    std::once_flag mutable m_file_name_lock;

    /**
     * @brief The brief name of the module.
     *
     * The name is generated from m_file_name, so it will be generated lazily.
     */
    std::string mutable m_name;

    /**
     * @brief Mutex for lazy generation of module_info::m_name.
     *
     * The module_info::m_name data is generated by the @c const member
     * function, get_name().  Mutable data altered in a @c const context
     * should always be protected by a threading guard.
     */
    std::once_flag mutable m_name_lock;
};

} // namespace psystem

#endif // PSYSTEM_MODULE_INFO_HPP
