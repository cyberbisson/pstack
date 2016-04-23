// ===-- src/addr2ln/addr2ln_options.hpp ------------------ -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define the command-line options interface for the Addr2ln application.
 *
 * @author  Matt Bisson
 * @date    14 August, 2016
 * @since   Addr2ln 0.1
 * @version Addr2ln 0.1
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 *
 * @todo Consolidate macros between option class types.
 */

#include "addr2ln/addr2ln_options.hpp"

#include <climits>
#include <iostream>

#include <psystem/framework/iomanip.hpp>
#include <psystem/exception/ui_exception.hpp>

using psystem::format::ptr;

////////////////////////////////////////////////////////////////////////////////
/// @name Convenience Macros
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Creates a string for a command-line flag.
 *
 * This is done by by appending the platform-specific option flag starting token
 * (i.e., '-' in a "-h" flag) to the @p opt parameter.
 *
 * @param opt The option flag --- it should be a single letter to qualify as a
 *            "flag."  This must be a quoted C string literal.
 */
#ifndef _WIN32
#   define MAKE_OPTION_FLAG(opt) "-" opt
#else
#   define MAKE_OPTION_FLAG(opt) "/" opt
#endif

/**
 * @brief Used to print a help option.
 *
 * @param opt  The option (as the user would specify it).
 * @param desc A description of the option.
 */
#define PRINT_OPTION(opt,desc)                       \
    std::cout                                        \
        << "  " << std::setw(11) << std::left << opt \
        << ' ' << desc << '\n'

/**
 * @brief Used to print a help option for a "flag."
 *
 * A "flag" in this context is a single-letter command-line argument.
 *
 * @param opt  The single letter for the flag.
 * @param desc A description of the option.
 */
#define PRINT_OPTION_FLAG(opt,desc)                                     \
    std::cout                                                           \
        << "  " << std::setw(11) << std::left << MAKE_OPTION_FLAG(opt)  \
        << ' ' << desc << '\n'

/**
 * @brief Print the continuation of a description in the help option output.
 * @param desc The text to print after the indentation.
 */
#define PRINT_INDENTED(desc)                            \
    std::cout << "              " << desc << '\n'
/// @}

/// @brief Data local to this compilation unit
namespace {

/**
 * @brief The options block is supposed to be a singleton.
 *
 * @internal
 *     As much as I hate singletons, I don't really want to be passing options
 *     all over the system just to occasionally get a parameter.  Also, this is
 *     a C pointer instead of a shared_ptr for two reasons: first, the instance
 *     should be created by @c main, so we have no access to the container for
 *     that instance, and secondly because I want this static data to be
 *     trivially constructed in the compilation unit.
 */
addr2ln::addr2ln_options *g_options = nullptr;

/**
 * @brief Translate a string into an address.
 *
 * This method assumes that the string is a hexadecimal number.
 *
 * @param[in] text The string of text to translate.
 * @return The numerical address specified by @p text.
 *
 * @throws psystem::exception::ui_exception
 *     If @p text input does not consist entirely of text that can be converted
 *     to a psystem::address_t data type.  The resulting value must also be a
 *     positive integer that is less than @c ULONG_MAX.
 *
 * @todo The conversion routine does not necessarily map to a full-width
 *       pointer, but whatever size integer @c strtoul returns.
 */
static psystem::address_t
fetch_address(char const *text)
{
    ASSERT(text);

    char *end;
    psystem::address_t addr = strtoul(text, &end, 0x10);

    if (!end || '\0' != *end || text == end)
    {
        throw psystem::exception::ui_exception(
            __FILE__, __FUNCTION__, __LINE__,
            "Invalid argument; expected an address - \"%s\"", text);
    }

    if (ULONG_MAX == addr)
    {
        throw psystem::exception::ui_exception(
            __FILE__, __FUNCTION__, __LINE__, "Address is too large - \"%s\"",
            text);
    }

    return addr;
}

} // namespace <anon>

namespace addr2ln {

/**
 * @brief The full set of options information available to
 *        addr2ln::addr2ln_options.
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
struct addr2ln_options::options_data final
{
    /**
     * @brief Construct the options data with default values.
     * @param[in] prog_name The name by which the applications was invoked.
     */
    explicit options_data(char const *prog_name) noexcept
        : program_name(prog_name) /// @todo Convert to TCHAR*
    { ASSERT(prog_name); }

    /// @brief The name of the executable that is running.
    psystem::tstring program_name;

    /**
     * @brief The address to use as the module-base address for look-ups.
     *
     * This may be @c nullptr if unspecified.
     */
    std::unique_ptr<psystem::address_t> module_base_address;

    /// @brief The file name of the module to examine for symbols.
    std::string module_name;

    /// @brief The list of addresses to examine in the module.
    addr2ln_options::address_list address_list;
};

////////////////////////////////////////////////////////////////////////////////
// Option Observers
////////////////////////////////////////////////////////////////////////////////

/*static*/ psystem::address_t const *
addr2ln_options::get_module_base_address() noexcept
{
    ASSERT(g_options);
    ASSERT(g_options->m_data);

    return g_options->m_data->module_base_address.get();
}

/*static*/ addr2ln_options::address_list const&
addr2ln_options::get_address_list() noexcept
{
    ASSERT(g_options);
    ASSERT(g_options->m_data);

    return g_options->m_data->address_list;
}

/*static*/ std::string const&
addr2ln_options::get_module_name() noexcept
{
    ASSERT(g_options);
    ASSERT(g_options->m_data);

    return g_options->m_data->module_name;
}

////////////////////////////////////////////////////////////////////////////////
// Informational Functions
////////////////////////////////////////////////////////////////////////////////

/*static*/ void
addr2ln_options::print_usage() noexcept
{
    print_usage((g_options)
                ? g_options->m_data->program_name.c_str()
                : ADDR2LN_APP_NAME);
}

/*static*/ void
addr2ln_options::print_version_info() noexcept
{
    std::cout
        << ADDR2LN_APP_NAME
        << " [Version "
        << ADDR2LN_MAJOR_VERSION << '.' << ADDR2LN_MINOR_VERSION << '.'
        << std::setfill('0') << std::setw(4) << ADDR2LN_BUILD_NUMBER
        << " (built on " << __DATE__ << ")]\n"
           "Copyright (c) 2008-2016 Matt Bisson.  All rights reserved.\n";

    std::cout
        << '\n'
        << PSYSTEM_APP_NAME
        << " [Version "
        << PSYSTEM_MAJOR_VERSION << '.' << PSYSTEM_MINOR_VERSION << '.'
        << std::setfill('0') << std::setw(4) << PSYSTEM_BUILD_NUMBER
        << " (built on " << __DATE__ << ")]\n"
           "Copyright (c) 2008-2016 Matt Bisson.  All rights reserved.\n";
}

/*static*/ void
addr2ln_options::print_usage(const char *const prog_name) noexcept
{
    std::cout << "Prints source code information for an given address.\n\n";

    std::cout << prog_name <<
        " module [" MAKE_OPTION_FLAG("B") " address] address [address...]\n\n";

    PRINT_OPTION(
        "address",
        "An address (hexadecimal) to check for source information.");
    PRINT_OPTION(
        MAKE_OPTION_FLAG("B") " address",
        "Provide a module base-address instead of reading the one encoded in");
    PRINT_INDENTED("the module itself.");
    PRINT_OPTION(
        "module",
        "The file name of an executable or debugging information file.");

    std::cout <<
        "\nSwitches may be presented at any time, however non-switch parameters"
        "\nmust first specify the executable module (only once), and then"
        "\nspecify the addresses to examine.\n";
    std::cout <<
        "\nYou can specify multiple addresses, and they are assumed to be"
        "\nhexadecimal addresses.\n";
}

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

addr2ln_options::addr2ln_options(int argc, char const *argv[])
    : m_data(new addr2ln_options::options_data(argv[0]))
{
    ASSERT(!g_options);
    ASSERT(m_data);

    bool saw_module_name = false;

    for (int i = 1; i < argc; ++i)
    {
        ASSERT(argv[i]);

        switch (*argv[i])
        {
#ifdef _WIN32
        // Windows honors both flag types.
        case '/':
#endif
        case '-':
            // Switches must be exactly one character
            if (argv[i][1] == '\0' || argv[i][2] != '\0')
            {
                throw psystem::exception::ui_exception(
                    __FILE__, __FUNCTION__, __LINE__,
                    "Invalid options - \"%s\"", argv[i]);
            }
            i = try_switch(i, argc, argv); // Can throw help exception...
            break;

        default:
            if (!saw_module_name)
            {
                m_data->module_name = argv[i];
                saw_module_name = true;
            }
            else
            {
                m_data->address_list.insert(fetch_address(argv[i]));
            }
        }
    }

#ifdef _DEBUG
    dump_arguments(*m_data);
#endif

    if (m_data->module_name.empty())
    {
        throw psystem::exception::ui_exception(
            __FILE__, __FUNCTION__, __LINE__, "No executable specified.");
    }

    if (m_data->address_list.empty())
    {
        throw psystem::exception::ui_exception(
            __FILE__, __FUNCTION__, __LINE__, "No address specified.");
    }

    g_options = this;
    ASSERT(g_options);
}

addr2ln_options::~addr2ln_options() noexcept
{
    g_options = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Private Utilities
////////////////////////////////////////////////////////////////////////////////

/*static*/ void
addr2ln_options::dump_arguments(addr2ln_options::options_data const& d) noexcept
{
    std::cout << "ARGS:\n";

    std::cout << "\tMODULE:       \"" << d.module_name << "\"\n";
    std::cout << "\tBASE ADDRESS: ";
    if (d.module_base_address)
    {
        std::cout << ptr(*d.module_base_address);
    }
    else
    {
        std::cout << "(unset)";
    }
    std::cout << '\n';
    std::cout << "\tADDRESS LIST:";
    {
        for (auto const addr : d.address_list)
        {
            std::cout << ' ' << ptr(addr);
        }
        std::cout << '\n';
    }
}

int
addr2ln_options::try_switch(int i, int const argc, char const *argv[])
{
    ASSERT(m_data);

    ASSERT(i < argc);
    ASSERT(('\0' != argv[i][0]) && ('\0' != argv[i][1]));

    char const arg = argv[i][1];
    switch (arg)
    {
    case 'B':
    case 'b':
    {
        if ((i + 1) >= argc)
        {
            throw psystem::exception::ui_exception(
                __FILE__, __FUNCTION__, __LINE__,
                "Switch (\"%c\") requires an additional parameter", arg);
        }

        auto tmp_module_address = std::make_unique<psystem::address_t>(
            fetch_address(argv[i+1]));
        m_data->module_base_address = std::move(tmp_module_address);
        ++i;
        break;
    }

    case 'V':
    case 'v':
        print_version_info();
        throw false;

    case '?':
    case 'H':
    case 'h':
        print_usage(m_data->program_name.c_str());
        throw false;

    default:
        throw psystem::exception::ui_exception(
            __FILE__, __FUNCTION__, __LINE__, "Invalid switch - \"%c\"", arg);
    }

    return i;
}

} // namespace addr2ln
