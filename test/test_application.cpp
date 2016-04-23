// ===-- test/test_application.cpp ------------------------ -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief The entry-point for a test application for PStack.
 *
 * @author  Matt Bisson
 * @date    29, January 2015
 * @since   PStack 2.0
 * @version PStack 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#ifdef __GNUC__
#   include <unistd.h>
#   ifdef __CYGWIN__
#       include <sys/cygwin.h>
#   endif
#else
#   include <process.h>
using pid_t = int;
#if defined(_MSC_VER) && (_MSV_VER <= 1800)
#   define _ALLOW_KEYWORD_MACROS
#   define constexpr const
#   define noexcept throw()
#endif
#endif

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <csignal>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

////////////////////////////////////////////////////////////////////////////////
/// @name Test Infrastructure:
////////////////////////////////////////////////////////////////////////////////

namespace {

/// @brief Mutex used to block threads via block_thread().
std::mutex g_block_thread_mutex;

/// @brief Condition on which all threads will wait during block_thread().
std::condition_variable g_block_thread_waiter;

/// @brief Total number of threads to spawn (not including main()).
constexpr int g_max_threads = 4;

} // namespace <anon>

/**
 * @brief Get the process ID known to the kernel.
 *
 * Windows and *NIX use different API to retrieve the PID.  Cygwin uses the *NIX
 * API, but it is not the process ID known to Windows, and it must be converted.
 *
 * @return The process ID known to the OS.
 */
static pid_t get_native_pid()
{
#ifdef __GNUC__
#   ifdef __CYGWIN__
    return cygwin_internal(CW_CYGWIN_PID_TO_WINPID, getpid());
#   else
    return getpid();
#   endif
#else
    return _getpid();
#endif
}

static void sigterm_handler(int)
{
    std::cout << "Releasing threads.\n";
    g_block_thread_waiter.notify_all();
}

static void block_thread()
{
    std::unique_lock<std::mutex> lock(g_block_thread_mutex);

    std::cout << std::this_thread::get_id() << ": blocking thread.\n";
    g_block_thread_waiter.wait(lock);
    std::cout << std::this_thread::get_id() << ": unblocking thread.\n";
}

////////////////////////////////////////////////////////////////////////////////
/// @name Test Code
////////////////////////////////////////////////////////////////////////////////

namespace test_ns {

class test_class
{
public:
    virtual void virtual_function() const
    {
        block_thread();
    }

    static void static_function(std::string const&)
    {
        auto tc = std::make_shared<test_class>();

        auto functor = std::bind(&test_class::virtual_function, tc);
        functor();
    }
};

} // namespace test_ns

static void thread_main()
{
    std::cout << std::this_thread::get_id() << ": entering thread_main().\n";
    test_ns::test_class::static_function("");
    std::cout << std::this_thread::get_id() << ": exiting thread_main().\n";
}

int main()
{
    std::cout << "Installing SIGINT handler.\n";
    signal(SIGINT, sigterm_handler);

    std::list<std::thread> thread_list;

    for (int i = 0; i < g_max_threads; ++i)
    {
        thread_list.emplace_back(thread_main);
    }

    /// @todo Too lazy to properly wait for all threads to block -- using sleep
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "\nProcess running " << thread_list.size()
              << " threads with PID: " << get_native_pid() << "\n\n";

    std::for_each(
        thread_list.begin(), thread_list.end(), [](std::thread& t) {
            auto const id = t.get_id();
            t.join();

            // Reusing g_block_thread_mutex to hold the console...
            std::unique_lock<std::mutex> lock(g_block_thread_mutex);
            std::cout << "Joined thread '" << id << "'.\n";
        });

    std::cout << "All threads reclaimed.\n";
    return 0;
}
