// ===-- include/pstack/pstack_ns.hpp --------------------- -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief The basic header file for the PStack namespace.
 *
 * All modules that declare ::pstack namespace items must include this header
 * explicitly.  When external modules reference this header, they are declaring
 * a dependency on the namespace / module.
 *
 * @author  Matt Bisson
 * @date    9 July, 2008 -- 5 August, 2014
 * @since   PStack 1.3
 * @version PStack 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 *
 * @todo It might be nice to be able to selectively use the 32-bit or 64-bit
 *       process examination API.  Currently, we're held to 64-bit only.
 */

/**
 * @dir
 * @brief This directory contains the public interface declarations for the
 *        ::pstack namespace.
 *
 * @author  Matt Bisson
 * @date    9 July, 2008 -- 5 August 2014
 * @since   PStack 1.3
 * @version PStack 2.0
 */

/**
 * @mainpage Stack Printout (PStack) Reference
 *
 * The main project contained in this code-base is the PStack application.  This
 * is an application that is capable of connecting to a running process, and
 * displaying stack traces for all threads.  It decodes both MSVC's and G++'s
 * name mangling styles.
 *
 * The genesis of this application comes from a previous position of mine in
 * which no single set of tools was not able to give reliable stack traces for
 * the running process.  Essentially, the @c mingw C++ toolchain's name-mangling
 * was not consistent with Microsoft's debugging libraries, and produced
 * obfuscated stack traces.  In addition, the test machines under Windows did
 * not necessarily have a debugger present (as opposed to Linux with it's
 * ubiquitous installation of GDB).  PStack is a light-weight solution to both
 * of these problems.
 *
 * Download the latest source
 * [here](https://cyberbisson.com/download/pstack-2.0.0.tgz).
 *
 * @author  Matt Bisson
 * @date    25 November, 2007 -- 15 September, 2014
 * @since   PStack 1.0
 * @version PStack 2.0
 *
 * # Points of Interest #
 *
 * Before delving into the details of this application, note that the project is
 * a playground where I can toy with software engineering tidbits while still
 * developing something real.  As such, I will direct your attention to some
 * interesting points in the code for those interested in C++ development, and
 * so on:
 *
 * @li First, as you've probably already noticed, the entire project is
 *     documented in great detail with
 *     [Doxygen](http://www.stack.nl/~dimitri/doxygen/).
 * @li PStack loads the DbgHelp DLL manually, which allows it to check the
 *     version of this DLL for compatibility instead of simply misbehaving (a
 *     common reaction to older DbgHelp versions).
 *     See: psystem::debug_help_dll.
 * @li There is a small bit of template meta-programming in type_traits.hpp to
 *     examine (at compile time) if a function has a @c void return type
 *     (psystem::has_void_return_type).  This meta-function then determines what
 *     function-call operator psystem::shared_library uses for manually loaded
 *     DLL imports.
 * @li The psystem::symbol_file_module class does some interesting manual
 *     examination of Windows DLL and EXE files (in COFF format).  It even
 *     exposes its own iterators so that DLL exports and debugging symbols can
 *     be examined in standard C++ algorithms.
 * @li PSystem has its own RAII containers for Windows handles to match the C++
 *     standard pointer-type containers (e.g., @c std::unique_ptr).  These are
 *     psystem::shared_handle and psystem::unique_handle.  Also of interest is
 *     that the @c unique_handle is implemented in such a way that is takes no
 *     memory footprint (beyond that of the handle it contains) when
 *     @c CloseHandle is acceptable for clean-up.  Since Windows handles are
 *     often passed into Win32 functions for modification, these types also
 *     define an @c operator& that keeps the handle safe even during that
 *     modification.
 * @li The psystem::format namespace defines some enhancements to @c iostream
 *     to augment the way in which certain commonly used data is formatted for
 *     output.
 *
 * Of course, the actual work that PStack does is interesting as well, so we
 * will discuss that next.
 *
 * # Modules #
 *
 * Since its inception, the PStack application has been broken down into
 * multiple modules.  The separation of the application from base functionality
 * allows us to create other applications with ease that are unrelated to stack
 * printing.
 *
 * ## PSystem ##
 *
 * The PSystem library is an abstraction layer that provides a platform-agnostic
 * data-model, on which a debugger may be implemented.  Within this context, the
 * library defines objects that encapsulate system processes, threads, stack
 * frames, and so on within a logical run-time structure.  It also contains
 * utilities for reading executable files directly, and for mapping memory
 * addresses to symbols and source code information.
 *
 * Conveniently, PSystem also presents basic utilities, such as exception types
 * and framework classes (e.g., psystem::shared_handle, or
 * psystem::format::mask()).
 *
 * ## ProcLib ##
 *
 * ProcLib is a simple library built on top of PSystem's infrastructure that
 * exposes debugging functionality.  Applications should look here for classes
 * that connect to running processes, and access that process's memory, set
 * break-points, and so on.
 *
 * A typical application based on this library does the following:
 * @li Create a proclib::debug_engine instance.
 * @li Attach the instance to a process.
 * @li Register a listener for debugging events with
 *     proclib::process_debugger::add_event_listener().
 * @li Based on events, gather psystem::process_info, psystem::thread_info,
 *     psystem::module_info, and so on.
 *
 * ## PStack ##
 *
 * PStack is a useful tool if you don't have proper debugging tools on a machine
 * (for example, on a machine that you try to keep as clean as possible for QA
 * purposes).  The app consists of one executable and has dependencies on only a
 * few (Microsoft) DLL's.
 *
 * Beyond this, the tool seems to get better stack trace information that most
 * debuggers (including the MS debugger).  It can be told to upgrade its OS
 * access rights (to debug processes running as a service, for example), and can
 * search the executable on disk for debugging symbols that aren't present in
 * memory.  This allows it to be more useful on GCC-generated executables than
 * MSVC (which doesn't understand them at all).
 *
 * The application runs fairly quickly, and with a program running 30 threads,
 * and an executable size of 150mb, it can return in well under a second with
 * data (on 2007 technology, that is).
 *
 * ## Addr2Ln ##
 *
 * This is a fairly simple application that exercises a decent amount of the
 * Psystem functionality.  Given an executable module file and an address, this
 * application will attempt to look up the function name (and line number) that
 * corresponds to that offset.  In other words, given a program counter address,
 * one can look up where in the code the program is executing.
 */

#pragma once
#ifndef PSTACK_PSTACK_NS_HPP
#define PSTACK_PSTACK_NS_HPP

#include <psystem/psystem_ns.hpp>

////////////////////////////////////////////////////////////////////////////////
/// @name Application Version Macros
////////////////////////////////////////////////////////////////////////////////

/// @brief Informational name of this application.
#define PSTACK_APP_NAME         "PStack (Process Stack Printer)"

/// @brief Major version number (representing major functionality changes).
#ifndef PSTACK_MAJOR_VERSION
#   define PSTACK_MAJOR_VERSION 2
#endif

/// @brief Minor version (representing incremental bug-fixing and minor feature
///        changes).
#ifndef PSTACK_MINOR_VERSION
#   define PSTACK_MINOR_VERSION 0
#endif

/// @brief Corresponds to a quick patch level applied to a build.
#ifndef PSTACK_PATCH_LEVEL
#   define PSTACK_PATCH_LEVEL   0
#endif

/// @brief Corresponds to a nightly build number.
///
/// If this is 0, this is a "special" build.
#ifndef PSTACK_BUILD_NUMBER
#   define PSTACK_BUILD_NUMBER  0
#endif

/// @brief This is used to make a note in the version info in the executable.
#if (PSTACK_BUILD_NUMBER == 0)
#   define _PRIVATE_BUILD       1
#endif

/// @}

/**
 * @brief Contains modules that make up the PStack stack-printing application.
 *
 * @author  Matt Bisson
 * @date    9 July, 2008 -- 5 August, 2014
 * @since   PStack 1.3
 * @version PStack 2.0
 */
namespace pstack {

} // namespace pstack

#endif // PSTACK_PSTACK_NS_HPP
