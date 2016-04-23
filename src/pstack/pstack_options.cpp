// ===-- src/pstack/pstack_options.hpp -------------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define the command-line options interface for the PStack application.
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

#include "pstack/pstack_options.hpp"

#include <climits>
#include <iostream>

#include <psystem/framework/iomanip.hpp>
#include <psystem/exception/ui_exception.hpp>

using psystem::format::swtch;

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
pstack::pstack_options *g_options = nullptr;

} // namespace <anon>

namespace pstack {

/**
 * @brief The full set of options information available to
 *        pstack::pstack_options.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PStack 2.0
 * @version PStack 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */
struct pstack_options::options_data final
{
    /**
     * @brief Construct the options data with default values.
     * @param[in] prog_name The name by which the applications was invoked.
     */
    explicit options_data(char const *prog_name) noexcept
        : program_name(prog_name) /// @todo Convert to TCHAR*
    { ASSERT(prog_name); }

    /// @brief Should PStack get symbols directly or with Windows API?
    bool get_image_symbols = true;

    /// @brief Should PStack traverse all threads for stack traces?
    bool search_all_threads = true;

    /// @brief Should PStack show frame pointers for all stack frames?
    bool show_frames = false;

    /// @brief Should PStack output extra debugging information?
    bool show_debug = false; ///< @todo UNUSED

    /// @brief Should PStack show error messages?
    bool show_error = true;

    /// @brief Should PStack show informational messages?
    bool show_info = false;

    /// @brief The name of the executable that is running.
    psystem::tstring program_name;

    /// @brief The list of process IDs requested by the user.
    pstack_options::pid_list process_id_list;
};

////////////////////////////////////////////////////////////////////////////////
// Option Observers
////////////////////////////////////////////////////////////////////////////////

/*static*/ bool
pstack_options::get_image_symbols() noexcept
{
    ASSERT(g_options);
    ASSERT(g_options->m_data);

    return g_options->m_data->get_image_symbols;
}

/*static*/ pstack_options::pid_list const&
pstack_options::process_id_list() noexcept
{
    ASSERT(g_options);
    ASSERT(g_options->m_data);

    return g_options->m_data->process_id_list;
}

/*static*/ psystem::tstring const&
pstack_options::program_name() noexcept
{
    ASSERT(g_options);
    ASSERT(g_options->m_data);

    return g_options->m_data->program_name;
}

/*static*/ bool
pstack_options::search_all_threads() noexcept
{
    ASSERT(g_options);
    ASSERT(g_options->m_data);

    return g_options->m_data->search_all_threads;
}

/*static*/ bool
pstack_options::show_frames() noexcept
{
    ASSERT(g_options);
    ASSERT(g_options->m_data);

    return g_options->m_data->show_frames;
}

/*static*/ bool
pstack_options::show_debug() noexcept
{
    ASSERT(g_options);
    ASSERT(g_options->m_data);

    return g_options->m_data->show_debug;
}

/*static*/ bool
pstack_options::show_error() noexcept
{
    ASSERT(g_options);
    ASSERT(g_options->m_data);

    return g_options->m_data->show_error;
}

/*static*/ bool
pstack_options::show_info() noexcept
{
    ASSERT(g_options);
    ASSERT(g_options->m_data);

    return g_options->m_data->show_info;
}

////////////////////////////////////////////////////////////////////////////////
// Informational Functions
////////////////////////////////////////////////////////////////////////////////

/*static*/ void
pstack_options::print_usage() noexcept
{
    print_usage((g_options)
                ? g_options->m_data->program_name.c_str()
                : PSTACK_APP_NAME);
}

/*static*/ void
pstack_options::print_version_info() noexcept
{
    std::cout
        << PSTACK_APP_NAME
        << " [Version "
        << PSTACK_MAJOR_VERSION << '.' << PSTACK_MINOR_VERSION << '.'
        << std::setfill('0') << std::setw(4) << PSTACK_BUILD_NUMBER
        << " (built on " << __DATE__ << ")]\n"
           "Copyright (c) 2007-2016 Matt Bisson.  All rights reserved.\n";

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
pstack_options::print_usage(const char *const prog_name) noexcept
{
    std::cout << "Prints a running application's stack information.\n\n";

    std::cout
        << prog_name << " ["
        MAKE_OPTION_FLAG("A") " | "
        MAKE_OPTION_FLAG("O") "] ["
        MAKE_OPTION_FLAG("F") "] ["
        MAKE_OPTION_FLAG("I") "] ["
        MAKE_OPTION_FLAG("P") "] process [process ...]\n\n";

    PRINT_OPTION(
        "process",
        "Numerical ID of the process to scan.");
    PRINT_OPTION_FLAG(
        "A",
        "Show stack traces for all threads in the process (this");
    PRINT_INDENTED(
        "is the default).  This will conflict with the "
        MAKE_OPTION_FLAG("O") " option.");

    PRINT_OPTION_FLAG(
        "O",
        "Display a stack trace only for the active thread.  This");
    PRINT_INDENTED("will conflict with the " MAKE_OPTION_FLAG("A") " option.");

    PRINT_OPTION_FLAG(
        "F",
        "Display frame pointers with the stack trace.");

    PRINT_OPTION_FLAG(
        "I",
        "If no symbols are found, scan the executable image.");

    PRINT_OPTION_FLAG(
        "P",
        "Use administrative privileges if possible.");

    PRINT_OPTION_FLAG(
        "V",
        "Display version info and exit.");
    PRINT_OPTION_FLAG(
        "?",
        "Display this help info and exit.");

    std::cout <<
        "\nAll options apply to all process IDs given regardless of order."
        "\nThe last option specified of conflicting options is used.\n";
    std::cout <<
        "\nYou can specify multiple process IDs, and they can be given in"
        "\ndecimal, octal (by prefixing the number with \'0\'), or"
        " hexadecimal\n(by prefixing the number with \"0x\").\n";
}

////////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
////////////////////////////////////////////////////////////////////////////////

pstack_options::pstack_options(int const argc, char const *argv[])
    : m_data(new pstack_options::options_data(argv[0]))
{
    ASSERT(!g_options);
    ASSERT(m_data);

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
            try_switch(argv[i][1]); // Can throw help exception...
            break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            try_pid(argv[i]);
            break;

        default:
            throw psystem::exception::ui_exception(
                __FILE__, __FUNCTION__, __LINE__, "Invalid argument - \"%s\"",
                argv[i]);
        }
    }

#ifdef _DEBUG
    dump_arguments(*m_data);
#endif

    if (m_data->process_id_list.empty())
    {
        throw psystem::exception::ui_exception(
            __FILE__, __FUNCTION__, __LINE__, "No processes specified.");
    }

    g_options = this;
    ASSERT(g_options);
}

pstack_options::~pstack_options() noexcept
{
    g_options = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Private Utilities
////////////////////////////////////////////////////////////////////////////////

/*static*/ void
pstack_options::dump_arguments(pstack_options::options_data const& d) noexcept
{
    std::cout << "ARGS:\n";
    std::cout << "\tDEBUG:    " << swtch(d.show_debug)  << '\n';
    std::cout << "\tINFO:     " << swtch(d.show_info)   << '\n';
    std::cout << "\tERROR:    " << swtch(d.show_error)  << '\n';
    std::cout << "\tFRAMES:   " << swtch(d.show_frames) << '\n';
    std::cout << "\tTHREADS:  show " << ((d.search_all_threads) ? "all" : "one")
              << '\n';
    std::cout << "\tSYMBOLS:  find in " << 
           ((d.get_image_symbols) ? "memory" : "memory/file") << '\n';
    std::cout << '\n';

    std::cout << "\tPID LIST:";
    {
        for (auto const pid : d.process_id_list)
        {
            std::cout << ' ' << pid;
        }
        std::cout << '\n';
    }
}

void
pstack_options::try_pid(char const *arg)
{
    ASSERT(arg);

    char *end;
    psystem::process_id_t new_pid = strtoul(arg, &end, 0);

    if (!end || *end != '\0' || arg == end)
    {
        throw psystem::exception::ui_exception(
            __FILE__, __FUNCTION__, __LINE__, "Invalid argument - \"%s\"", arg);
    }

    if (ULONG_MAX == new_pid)
    {
        throw psystem::exception::ui_exception(
            __FILE__, __FUNCTION__, __LINE__, "Process ID is too large - \"%s\"",
            arg);
    }

    ASSERT(m_data);
    m_data->process_id_list.insert(new_pid);
}

void
pstack_options::try_switch(char const arg)
{
    ASSERT(m_data);

    switch (arg)
    {
    case 'A':
    case 'a':
        m_data->search_all_threads = true;
        return;

    case 'F':
    case 'f':
        m_data->show_frames = true;
        return;

    case 'I':
    case 'i':
        m_data->get_image_symbols = true;
        return;

    case 'O':
    case 'o':
        m_data->search_all_threads = false;
        return;

    case 'P':
    case 'p':
        /// @todo Use administrative privs if possible
        return;

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
}

} // namespace pstack
