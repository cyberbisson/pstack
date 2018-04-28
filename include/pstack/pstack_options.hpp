// ===-- include/pstack/pstack_options.hpp ---------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare the command-line options interface for the PStack application.
 *
 * @author  Matt Bisson
 * @date    25 November, 2007 -- 5 August, 2014
 * @since   PStack 1.0
 * @version PStack 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#pragma once
#ifndef PSTACK_PSTACK_OPTIONS_HPP
#define PSTACK_PSTACK_OPTIONS_HPP

#include "pstack_ns.hpp"

#include <set>
#include <psystem/framework/memory.hpp>
#include <psystem/framework/not_copyable.hpp>

/// @cond
// Declare this so the friend relationship below will work
int main(int argc, char const *argv[]);
/// @endcond

namespace pstack {

/**
 * @brief Parse and maintain options that alter the execution of PStack.
 *
 * None of the "Option Observers" will function in this interface until one
 * (and only one) instance of pstack_options has been instantiated.  Only the
 * @c main method is allowed to construct an instance of this class.  This
 * pstack_options is a singleton, and the lifetime is limited to a predictable
 * scope in @c main.
 *
 * @internal
 *     Since this uses the PImpl pattern, we will make this class
 *     @c not_copyable, rather than redefining the copy constructor to do a deep
 *     copy of the "impl."  Copying isn't useful at the moment anyway.
 *
 * @author  Matt Bisson
 * @date    25 November, 2007 -- 5 August, 2014
 * @since   PStack 1.0
 * @version PStack 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */
class pstack_options final
    : psystem::not_copyable
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Type Definitions
    ////////////////////////////////////////////////////////////////////////////
public:
    /// @brief The a container for list of user-supplied process IDs.
    using pid_list = std::set<psystem::process_id_t>;

    // Forward declaration of the PImpl details.
    struct options_data;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Option Observers
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Should PStack search modules on disk for symbols rather than
     *        using built-in libraries?
     *
     * The built-in debugging symbol libraries do not always find the symbols
     * they need to generate a detailed stack trace.  MinGW executables often
     * cause this issue.  This flag tells PStack to use our own home-grown
     * solution.
     *
     * @return @c true if this option is active.  The default is @c false.
     * @todo Not using this yet.
     */
    static bool get_image_symbols() noexcept;

    /**
     * @brief Access the list of process IDs specified on the command-line.
     * @return A collecion of PIDs.  It is valid for the lifetime of the
     *         pstack_options instance.
     */
    static pid_list const& process_id_list() noexcept;

    /**
     * @brief Access the name this program was invoked with.
     * @return The program name.  It is valid for the lifetime of the
     *         pstack_options instance.
     */
    static psystem::tstring const& program_name() noexcept;

    /**
     * @brief Should PStack output stack traces for all threads?
     * @return @c true if this option is active.  The default is @c true.
     */
    static bool search_all_threads() noexcept;

    /// @brief Should PStack output frame pointers for each stack frame?
    /// @return @c true if this option is active.  The default is @c false.
    static bool show_frames() noexcept;

    /// @brief Should the user output verbose debug information?
    /// @return @c true if this option is active.  The default is @c false.
    static bool show_debug() noexcept;

    /// @brief Should the user output errors in the flow of execution?
    /// @return @c true if this option is active.  The default is @c true.
    static bool show_error() noexcept;

    /// @brief Should the user output extra informational messaging?
    /// @return @c true if this option is active.  The default is @c false.
    static bool show_info() noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Informational Functions
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Print command-line help information to the screen.
     *
     * Essentially, this is instructing the user what parameters this
     * pstack_options class understands.
     */
    static void print_usage() noexcept;

    /**
     * @brief Output information about the PStack's version to the screen.
     *
     * This also displays PSystem information, as PStack relies upon that
     * system.
     */
    static void print_version_info() noexcept;

private:
    /**
     * @brief Print command-line help information to the screen.
     *
     * Essentially, this is instructing the user what parameters this
     * pstack_options class understands.  Because print_usage() relies on the
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
     * @brief Construct the pstack_options instance.
     *
     * This method is private because it is only intended to be called from the
     * @c main function (@c main is a friend of this class).  This is because
     * the constructor has a side-effect of populating a global (singleton)
     * pointer with @c this.  When @c main allocates the pstack_options
     * instance, we at least know the lifetime of the singleton coincides with
     * it.
     *
     * @param[in] argc The command-line argument count.
     * @param[in] argv The array of command-line arguments.
     *
     * @throws psystem::exception::ui_exception
     *     Under the following conditions:
     *     @li If an argument looks like a process ID, but is not convertable
     *         (lexically) from a string to an integer, or is out of the range
     *         of [0, @c ULONG_MAX).
     *     @li If an argument is a switch that is not valid for PStack.
     *     @li If no PIDs were specified after all arguments are processed.
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
     *
     */
    pstack_options(int argc, char const *argv[]);

public:
    /**
     * @brief Deallocate the options instance.
     * @post Invoking the static interface results in undefined behavior.
     */
    ~pstack_options() noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Private Utilities
    ////////////////////////////////////////////////////////////////////////////
private:
    /**
     * @brief Output all the command-line data to @c stdout.
     *
     * @param[in] d The internal data of the pstack_options instance.
     */
    static void dump_arguments(pstack_options::options_data const& d) noexcept;

    /**
     * @brief Try to interpret a CLI parameter as a process ID.
     *
     * This method attempts to convert a string using the C++ numerical
     * (literal) token's syntax.  That is, strings that begin with @c "0x" will
     * be interpreted as hexadecimal, those that begin with @c "0" will be
     * assumed to be octal, and all others will be assumed to be decimal.
     *
     * @param[in] arg The string to parse.  It cannot be @c nullptr.
     * @throws psystem::exception::ui_exception
     *     If @p arg input does not consist entirely of text that can be
     *     converted to a psystem::process_id_t data type.  The resulting value
     *     must also be a positive integer that is less than @c ULONG_MAX.
     */
    void try_pid(char const *arg);

    /**
     * @brief Try to alter the options data based on a single character CLI
     *        switch.
     *
     * If the user asks for version information or command-line option help,
     * this method will print that to the command line.
     *
     * @param[in] arg The single-letter command-line switch to parse.
     *
     * @throws psystem::exception::ui_exception
     *     If the user specified a switch that is not valid for PStack.
     * @throws false
     *     In the event of a non-fatal operation that interrupts the option
     *     parsing (e.g., printing the help screen), this method throws a
     *     @c bool with the value @c false.
     *
     * @todo Either throw a better exception type (help_exception) or perhaps
     *       alter the flow so that a return value can present the same
     *       behavior.
     */
    void try_switch(char arg);

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
    // Since we don't want to mysteriously inject main() into the pstack
    // namespace, we make sure to declare it above, and be explicit about the
    // global namespace.
    friend int ::main(int argc, char const *argv[]);
    /// @endcond
};

} // namespace pstack

#endif // PSTACK_PSTACK_OPTIONS_HPP
