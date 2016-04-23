// ===-- src/pstack/main.hpp ------------------------------ -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief The main entry point for the PStack executable.
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

#include <psystem/framework/memory.hpp>
#include <psystem/framework/iomanip.hpp>
#include <psystem/dll/debug_help_dll.hpp>

#include <psystem/dbghelp_symbol_engine.hpp>
#include <psystem/module_info.hpp>
#include <psystem/module_address_map.hpp>
#include <psystem/process_info.hpp>
#include <psystem/symbol.hpp>
#include <psystem/thread_info.hpp>

#include <psystem/exception/base_exception.hpp>
#include <psystem/exception/system_exception.hpp>
#include <psystem/exception/ui_exception.hpp>

#include <pstack/debug_event_dumper.hpp>
#include <pstack/pstack_event_handler.hpp>
#include <pstack/pstack_options.hpp>

#include <proclib/debug_engine.hpp>
#include <proclib/demangle/demangle.hpp>
#include <proclib/process_debugger.hpp>

#include <algorithm>
#include <iostream>

using psystem::format::hex;
using psystem::format::ptr;

using pstack::pstack_options;

////////////////////////////////////////////////////////////////////////////////
// Static Utilities
////////////////////////////////////////////////////////////////////////////////

/// @brief Functions that are local to this compilation unit.
namespace {

/**
 * @brief Prints a single stack from a stack trace to the console.
 *
 * @tparam SymbolEngine
 *     This method can be configured to use different "engines" for resolving
 *     function names.  psystem::dbghelp_symbol_engine is one such example.
 *
 * @param[in] symbol_eng Used to map addresses to function symbol names.
 * @param[in] modules    Catalog of loaded modules to search for information.
 * @param[in] frame      The stack frame that we are examining.
 */
template <typename SymbolEngine>
void
output_stack_frame(
    SymbolEngine& symbol_eng,
    psystem::module_address_map const& modules,
    psystem::stack_frame const& frame)
{
    // Output module name
    auto const mod_it = modules.lower_bound(frame.get_pc());
    std::cout
        << std::setw(15) << std::left
        << ((modules.end() != mod_it) ? mod_it->second.get_name() : "(Unknown)")
        << std::right << ' ';

    // Output the PC
    std::cout << ptr(frame.get_pc());

    // Output the frame pointer
    if (pstack_options::show_frames())
    {
        std::cout << '\t' << ptr(frame.get_fp());
    }

    // Output a symbol, if it exists
    auto sym = symbol_eng.find_symbol(frame.get_pc());
    if (sym)
    {
        std::cout << " - ";
        if (true /** @todo msvc_generated */)
        {
            std::cout << sym->get_name();
        }
        else
        {
            std::string const demangled_name =
                proclib::demangle::demangle_gcc(sym->get_name().c_str());

            std::cout <<
                (!demangled_name.empty()) ? demangled_name : sym->get_name();
        }

        if (sym->get_code_offset())
        {
            std::cout << " [+" << hex(sym->get_code_offset()) << ']';
        }
    }

    /// @todo Source-code line information unimplemented.
#if 0
    if (have_source_line)
    {
        std::cout << " - " << file << ':' << line_number;
    }
#endif

    std::cout << '\n';
}

/**
 * @brief Prints one thread's call stack to the screen.
 *
 * @tparam SymbolEngine
 *     This method can be configured to use different "engines" for resolving
 *     function names.  psystem::dbghelp_symbol_engine is one such example.
 *
 * @param[in] symbol_eng Used to map addresses to function symbol names.
 * @param[in] proc       The process that we are examining.
 * @param[in] thrd       The thread that we are examining.
 */
template <typename SymbolEngine>
void
output_stack_trace(
    SymbolEngine& symbol_eng,
    psystem::process_info *const proc,
    psystem::thread_info *const thrd)
{
    std::cout << "\nCall stack (thread: " << thrd->get_thread_id() << "):\n";
    std::cout << "Module\t\tAddress"
              << (pstack_options::show_frames() ? "\t\t\tFrame" : "")
              << '\n';

    auto const& modules = proc->m_modules;
    auto const& trace = thrd->get_stack_trace(*proc, nullptr, nullptr);
    for (auto const& frame : trace)
    {
        output_stack_frame(symbol_eng, modules, frame);
    }
}

} // namespace <anon>

////////////////////////////////////////////////////////////////////////////////
// PStack Basics
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Attach to a process and output stack traces for all threads.
 *
 * This is really the crux of the PStack application, as it attaches to a
 * process, creates the utilities to analyze debugging symbols, and actually
 * outputs the resulting stack traces.
 *
 * @param[in] engine
 *     The debugging "engine" used to attach to a process and read information.
 * @param[in] pid
 *     The system process ID (PID) of the process that will be examined.
 *
 * @todo Document what exceptions this throws (there are many).
 * @todo
 *     It would be nice if a problem with on PID didn't stop execution.
 *     Rearrange the exception handling a bit.
 */
void
process_pid(proclib::debug_engine& engine, psystem::process_id_t const pid)
{
    // Attatch the debugger to a process by PID
    proclib::process_debugger& d = engine.attach_to_process(pid);
#ifdef _DEBUG
    d.add_event_listener(std::make_shared<pstack::debug_event_dumper>());
#endif

    auto listener = std::make_shared<pstack::pstack_event_handler>();
    d.add_event_listener(listener);

    // Pump out debug events -- the PSTACK listener will build up the data model
    while (!listener->is_debugger_ready())
    {
        engine.wait_for_event();
    }

    /// @todo Debug-only log message here
#ifdef _DEBUG
    std::cout << "Done seeing debugger events... analyzing data.\n";
#endif

    psystem::process_info *const proc = listener->get_process_info();
    ASSERT(proc);

    /// @todo Debug-only log message here
#ifdef _DEBUG
    for (auto const& module_it : proc->m_modules)
    {
        std::cout << "Found a module: \"" << module_it.second.get_name()
                  << "\" - " << ptr(module_it.second.get_base_of_image())
                  << '\n';
    }
#endif

    psystem::dbghelp_symbol_engine symbol_engine(proc->get_process_handle());

    if (!pstack::pstack_options::search_all_threads())
    {
        output_stack_trace(
            symbol_engine,
            proc,
            proc->m_threads[listener->get_active_thread_id()].get());
    }
    else
    {
        using psystem::process_info;
        std::for_each(
            proc->m_threads.begin(), proc->m_threads.end(),
            [&symbol_engine, proc](
                process_info::thread_info_map::value_type& thread_data) {
                output_stack_trace(
                    symbol_engine,
                    proc,
                    thread_data.second.get());
            });
    }
}

/**
 * @brief The main entry point for the PStack application.
 *
 * See pstack::pstack_options for details on supported command-line options.
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
main(int const argc, char const *argv[])
{
    // Options' lifetime coexists with main process flow
    std::unique_ptr<pstack_options> options;

    try
    {
        // Only main has CTOR access... make_unique not usable here.
        options.reset(new pstack_options(argc, argv));
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

    try
    {
        psystem::initialize_psystem();

        proclib::debug_engine engine;
        engine.enable_debug_privilege();

        // Basically, I want to conditionally add code, rather than add a
        // conditional expression to run each iteration through the loop.
        //
        // This would properly be done with auto type deduction, and the ternary
        // operator, but MSVC++ cannot seem to do this, despite the lambdas
        // being captureless and the same function signature.  We must do an
        // if-then-else instead.
        std::function<void(psystem::process_id_t)> process_pid_func;
        if (pstack_options::process_id_list().size() <= 1)
        {
            process_pid_func = [&engine](psystem::process_id_t const pid) {
                process_pid(engine, pid);
            };
        }
        else
        {
            process_pid_func = [&engine](psystem::process_id_t const pid) {
                std::cout << "\n---------- PROCESS " << pid << " ----------\n";
                process_pid(engine, pid);
            };
        }

        /// @todo /I option (scan file for symbols) not implemented.
        /// @todo /P option (administrative privs) not implemented.
        std::for_each(
            pstack_options::process_id_list().begin(),
            pstack_options::process_id_list().end(),
            process_pid_func);

        return 0;
    }
    catch (psystem::exception::system_exception const& sex)
    {
        std::cerr << pstack_options::program_name() << ": " << sex << '\n';
        return sex.error_code();
    }
    catch (psystem::exception::base_exception const& bex)
    {
        std::cerr << pstack_options::program_name() << ": " << bex << '\n';
        return -2;
    }
    catch (std::exception const& ex)
    {
        std::cerr
            << pstack_options::program_name() << ": " << ex.what() << '\n';
        return -3;
    }
    catch (...)
    {
        return -4;
    }
}
