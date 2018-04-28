// ===-- include/addr2ln/addr2ln_options.hpp -------------- -*- C++ -*- --=== //
// Copyright (c) 2016 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare the command-line options interface for the Addr2Ln
 *        application.
 *
 * @author  Matt Bisson
 * @date    14 August, 2016
 * @since   Addr2ln 0.1
 * @version Addr2ln 0.1
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#pragma once
#ifndef ADDR2LN_ADDR2LN_OPTIONS_HPP
#define ADDR2LN_ADDR2LN_OPTIONS_HPP

#include "addr2ln_ns.hpp"

#include <set>
#include <string>

#include <psystem/framework/memory.hpp>
#include <psystem/framework/not_copyable.hpp>

/// @cond
// Declare this so the friend relationship below will work
int main(int argc, char const *argv[]);
/// @endcond

namespace addr2ln {

/**
 * @brief Parse and maintain options that alter the execution of Addr2Ln.
 *
 * None of the "Option Observers" will function in this interface until one
 * (and only one) instance of addr2ln_options has been instantiated.  Only the
 * @c main method is allowed to construct an instance of this class.  This
 * addr2ln_options is a singleton, and the lifetime is limited to a predictable
 * scope in @c main.
 *
 * @internal
 *     Since this uses the PImpl pattern, we will make this class
 *     @c not_copyable, rather than redefining the copy constructor to do a deep
 *     copy of the "impl."  Copying isn't useful at the moment anyway.
 *
 * @author  Matt Bisson
 * @date    14 August, 2016
 * @since   Addr2ln 0.1
 * @version Addr2ln 0.1
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */
class addr2ln_options final
    : psystem::not_copyable
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Type Definitions
    ////////////////////////////////////////////////////////////////////////////
public:
    /// @brief The a container for list of user-supplied addresses to look up.
    using address_list = std::set<psystem::address_t>;

    // Forward declaration of the PImpl details.
    struct options_data;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Option Observers
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Access the address to use as the module-base address for look-ups.
     * @return This may be @c nullptr if unspecified.
     */
    static psystem::address_t const *get_module_base_address() noexcept;

    /**
     * @brief Access the list of addresses for Addr2Ln to process.
     * @return A collection of addresses.  It is valid for the lifetime of the
     *         addr2ln_options instance.
     */
    static address_list const& get_address_list() noexcept;

    /**
     * @brief Access the name of the desired executable module that will be
     *        examined.
     * @return The module name.  It is valid for the lifetime of the
     *         addr2ln_options instance.
     */
    static std::string const& get_module_name() noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Informational Functions
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Print command-line help information to the screen.
     *
     * Essentially, this is instructing the user what parameters this
     * addr2ln_options class understands.
     */
    static void print_usage() noexcept;

    /**
     * @brief Output information about the Addr2Ln's version to the screen.
     *
     * This also displays PSystem information, as Addr2Ln relies upon that
     * system.
     */
    static void print_version_info() noexcept;

private:
    /**
     * @brief Print command-line help information to the screen.
     *
     * Essentially, this is instructing the user what parameters this
     * addr2ln_options class understands.  Because print_usage() relies on the
     * option instance being initialized, this version exists to provide the
     * required information without the requirement.  The presence of @p
     * prog_name is illogical as a public API, hence the private access
     * restriction.
     *
     * @param[in] prog_name The name of the application from the command-line.
     */
    static void print_usage(const char *const prog_name) noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
private:
    /**
     * @brief Construct the addr2ln_options instance.
     *
     * This method is private because it is only intended to be called from the
     * @c main function (@c main is a friend of this class).  This is because
     * the constructor has a side-effect of populating a global (singleton)
     * pointer with @c this.  When @c main allocates the addr2ln_options
     * instance, we at least know the lifetime of the singleton coincides with
     * it.
     *
     * @param[in] argc The command-line argument count.
     * @param[in] argv The array of command-line arguments.
     *
     * @throws psystem::exception::ui_exception
     *     Under the following conditions:
     *     @li If an argument should be an address, but is not convertable
     *         (lexically) from a string to a hexadecimal integer, or is out of
     *         the range of [0, @c ULONG_MAX).
     *     @li If an argument is a switch that is not valid for Addr2Ln.
     *     @li If no module or addresses were specified after all arguments are
     *         processed.
     * @throws false
     *     In the event of a non-fatal operation that interrupts the option
     *     parsing (e.g., printing the help screen), this method throws a
     *     @c bool with the value @c false.
     *
     * @post Invoking the static interface is possible after successful
     *       completion of the constructor.
     * @warning
     *     Only @c main can create and destroy the options data.  Make sure to
     *     do both operations in a single-threaded context.  This assumption
     *     prevents us from having to guard the global data, check for
     *     @c nullptr, and so on.
     */
    addr2ln_options(int argc, char const *argv[]);

public:
    /**
     * @brief Deallocate the options instance.
     * @post Invoking the static interface results in undefined behavior.
     */
    ~addr2ln_options() noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Private Utilities
    ////////////////////////////////////////////////////////////////////////////
private:
    /**
     * @brief Output all the command-line data to @c stdout.
     *
     * @param[in] d The internal data of the addr2ln_options instance.
     */
    static void dump_arguments(addr2ln_options::options_data const& d) noexcept;

    /**
     * @brief Try to alter the options data based on a single character CLI
     *        switch.
     *
     * If the user asks for version information or command-line option help,
     * this method will print that to the command line.
     *
     * @param[in] i
     *     The index (in @p argv) to the command-line parameter that should be
     *     examined.
     * @param[in] argc
     *     The total count of arguments that exist within @p argv.
     * @param[in] argv
     *     The array of character arrays that represent what the user passed to
     *     Addr2Ln on the command-line.
     *
     * @return
     *     The new index to examine after try_switch returns.  Some switches may
     *     require @em n extra command-line arguments.  In those cases, this
     *     method will examine the extra arguments, and return @p i + @em n.  In
     *     all other cases, it will simply return @p i.
     *
     * @throws psystem::exception::ui_exception
     *     If the user specified a switch that is not valid for Addr2Ln.
     * @throws false
     *     In the event of a non-fatal operation that interrupts the option
     *     parsing (e.g., printing the help screen), this method throws a
     *     @c bool with the value @c false.
     *
     * @pre
     *     @p i must be less than @p argc, and @c argv[i] must contain at least
     *     two characters.
     */
    int try_switch(int i, int argc, char const *argv[]);

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief A pointer to the options data (PImpl pattern).
    std::unique_ptr<options_data> m_data;

    /// @cond
    // Doxygen doesn't parse "int ::main" (it sees "int::main").  Skip this bit.

    // Section 7.3.1.2 [namespace.memdef] --
    // "If a friend declaration in a nonlocal class first declares a class or
    // function, the friend class or function is a member of the innermost
    // enclosing namespace."
    //
    // Since we don't want to mysteriously inject main() into the addr2ln
    // namespace, we make sure to declare it above, and be explicit about the
    // global namespace.
    friend int ::main(int argc, char const *argv[]);
    /// @endcond
};

} // namespace addr2ln

#endif // ADDR2LN_ADDR2LN_OPTIONS_HPP
