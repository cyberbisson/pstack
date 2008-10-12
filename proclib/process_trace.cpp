
/** @file process_trace.cpp
 ** @brief Define interfaces for tracing a single process.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.0
 ** @version Proclib 1.3
 **/

#include <windows.h>

#include <Tlhelp32.h>

#include <dbghelp.hpp>
#include <debug_module.hpp>
#include <process_module.hpp>
#include <thread_trace.hpp>
#include <trace_object.hpp>

#include <exception/windows_exception.hpp>
#include <exception/null_pointer_exception.hpp>

#include "process_trace.hpp"

using namespace psystem;

//////////////////////////////////////////////////////////////////////////////
// "process_trace::proclib_debugger" interface:
//////////////////////////////////////////////////////////////////////////////

/** @brief Save the debugger's handle for this process and return true. */
bool proclib::process_trace::proclib_debugger::OnCreateProcess (
    processId_t, threadId_t, const CREATE_PROCESS_DEBUG_INFO& data)
    throw ()
{
    m_Parent.setHandle (data.hProcess);
    return true;
}

//////////////////////////////////////////////////////////////////////////////
// Public interface:
//////////////////////////////////////////////////////////////////////////////

/** @brief Allocate a process object and attach it with the debugger.
 **
 ** @param processId The system process ID to examine.
 ** @param allThreads If true, this will find all threads of the running process
 **        and examine them.  If this is false, it examines only the process
 **        that is active <b>at the time of object creation</b>.
 ** @return A newly allocated process_trace object.
 ** @throws bad_alloc If memory could not be allocated for the new object.
 ** @throws windows_exception If an error was thrown while upgrading privileges
 **         (if requested), or an error was thrown attaching to the process.
 **/
proclib::process_trace *proclib::process_trace::Create (
    processId_t processId, bool allThreads /*=true*/)
    throw (psystem::exception::unimplemented_exception,
           psystem::exception::windows_exception,
           std::bad_alloc)
{
    proclib::process_trace *ret = new process_trace (processId, allThreads);

    if (NULL == ret) throw std::bad_alloc ();

    ret->attach ();
    return ret;
}

void proclib::process_trace::dump_object ()
    throw ()
{
    /** @todo Dump object! */
}

/** @brief Find a symbol, given a module.
 **
 ** First, we try to return the symbol by looking only in that module.  This is
 ** a read-only operation.  If this fails, we try to look up the symbol from the
 ** running executable's debug information.  This may also fail in the same
 ** manner as a normal lookup fails.  If this succeeds, we put this symbol in
 ** the given module's symbol list for future searches.
 **
 ** @param module The running module to search (and possibly modify).
 ** @param location The memory location to match to a symbol.
 ** @return A symbol name and the offset from that symbol's starting point.
 **/
proclib::symbol_t proclib::process_trace::findSymbol (
    const proclib::process_module& module, const memAddress_t& location)
{
    try
    {
        return module.getSymbol (location);
    }
    catch (const char *)
    {
        memAddress_t codeOffset  = NULL;
        const size_t kSymbolSize =
            sizeof(SYMBOL_INFO) + ((MAX_SYM_NAME + 1) * sizeof (TCHAR));
        BYTE         symBuffer[kSymbolSize];
        SYMBOL_INFO *symbolInfo  = (SYMBOL_INFO *)symBuffer;

        memset (symbolInfo, 0, kSymbolSize);
        symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbolInfo->MaxNameLen   = MAX_SYM_NAME;

        if (!psystem::dbghelp::SymFromAddr (
                getProcessHandle (), location, &codeOffset, symbolInfo))
        {
            /** @todo Bullshit exception... */
            throw "";
        }

        symbol_t symbol;
        symbol.name    = symbolInfo->Name;
        symbol.address = location - codeOffset;

        /** @todo Sanity check that the address is at least reasonable for this
         **       module. */

        /** @todo non-const? */
        return ((process_module&)module).putSymbol (symbol);
    }
}

/** @brief Looks for a symbol in all modules.
 **
 ** @param location The memory location to match to a symbol.
 ** @return A symbol name and the offset from that symbol's starting point.
 **/
proclib::symbol_t proclib::process_trace::findSymbol (
    const memAddress_t& location)
{
    const proclib::process_module& module = findModule (location);
    return findSymbol (module, location);
}

/** @brief Given a memory address, find the module that <i>should</i> own the
 **        memory.
 **
 ** Often, the size of a module's address space is not known, so this function
 ** may believe that an address belongs to a module, when it's simply beyond
 ** the highest module's base address.
 **
 ** @param location A memory address that we hope to link to a module.
 ** @return A reference to the associated module's object.
 **/
const proclib::process_module& proclib::process_trace::findModule (
    const memAddress_t& location) const
{
    const memAddress_t base =
        psystem::dbghelp::SymGetModuleBase64 (getProcessHandle (), location);

    /** @todo Bullshit exception... */
    if (NULL == base) throw "NOTFOUND";

    moduleList_t::const_iterator found = m_Modules.find (base);
    /** @todo Bullshit exception... */
    if ((m_Modules.end () == found) || (NULL == found->second)) throw "NOTFOUND";

    return *(found->second);
}

/** @brief Retrieve the module name at the given memory address.
 **
 ** @param location An address expected to be somewhere inside a loaded module.
 ** @return The name of the module or "<Unknown>".
 ** @todo Shouldn't this function at least *try* to use process_module?
 **/
const char *proclib::process_trace::getModuleName (
    const memAddress_t& location) const
    throw ()
{
    const char *modname = NULL;

    try
    {
        const memAddress_t base = psystem::dbghelp::SymGetModuleBase64 (
            getProcessHandle (), location);

        if (NULL == base)
        {
            THROW_WINDOWS_EXCEPTION_F (
                GetLastError(),
                "Cannot get module base of VA 0x%08I64X", location);
        }

        moduleList_t::const_iterator found = m_Modules.find (base);
        if (m_Modules.end () != found)
        {
            modname = found->second->getModuleName().c_str ();
        }
        else
        {
            /** @todo Exception? **/
        }
    }
    catch (psystem::exception::windows_exception&)
    {
        // Do I care??
    }

    return (modname && *modname) ? "<Unknown>" : modname;
}

//////////////////////////////////////////////////////////////////////////////
// Protected methods:
//////////////////////////////////////////////////////////////////////////////

/** @brief The constructor changes access rights of the caller's process, as well
 **        as constructing the object.
 **
 ** @param processId The system process ID to examine.
 ** @param allThreads If true, this will find all threads of the running process
 **        and examine them.  If this is false, it examines only the process
 **        that is active <b>at the time of object creation</b>.
 ** @see proclib::debug_module::EnableDebugPrivilege for what exceptions are
 **      thrown and why.
 **/
proclib::process_trace::process_trace (processId_t processId, bool allThreads)
    throw (psystem::exception::unimplemented_exception,
           psystem::exception::windows_exception)
    : trace_object (INVALID_HANDLE_VALUE),
      m_AllThreads    (allThreads),
      m_ProcessId     (processId),
      m_DebugListener (*this)  /** @todo Compiler complains about this */
{
    try
    {
        /** @todo Isn't this a configurable parameter??? */
        debug_module::EnableDebugPrivilege ();
    }
    catch (psystem::exception::base_exception&)
    {
        /** @todo Do we want this to escape process_trace?  Any changes here
         **       affect this method's decl. and Create's. */
        fprintf (stderr, "Cannot get admin privileges.\n");
    }
}

/** @brief Attach this object's debugger to the associated process.
 **
 ** @throws windows_exception If there is an error connecting to the process.
 **/
void proclib::process_trace::attach ()
    throw (psystem::exception::windows_exception)
{
    debug_module& debugger = debug_module::GetDebugger (m_ProcessId);

    debugger.addListener (m_DebugListener);
#ifdef _DEBUG
    debugger.addListener (m_EventDumper);
#endif
    debugger.pumpAvailableEvent ();
}

void proclib::process_trace::init ()
    throw (psystem::exception::windows_exception)
{
    m_MachineType = initMachineType ();
    initSymbols ();

    HANDLE hToolHelp;

    hToolHelp = CreateToolhelp32Snapshot (TH32CS_SNAPTHREAD, m_ProcessId);
    if (INVALID_HANDLE_VALUE == hToolHelp)
    {
        THROW_WINDOWS_EXCEPTION_F(
            GetLastError(), "Error getting thread snapshot for process %d",
            m_ProcessId);
    }

    DWORD         hr = ERROR_SUCCESS;
    THREADENTRY32 thread;

    memset (&thread, 0, sizeof (thread));
    thread.dwSize = sizeof (THREADENTRY32);

    if (!Thread32First (hToolHelp, &thread))
    {
        hr = GetLastError ();

        if (ERROR_NO_MORE_FILES != hr)
        {
            CloseHandle (hToolHelp);

            THROW_WINDOWS_EXCEPTION_F(
                GetLastError(), "Error getting threads for process %d",
                m_ProcessId);
        }

        if (thread.th32OwnerProcessID == m_ProcessId)
        {
            m_Threads.push_back (thread_trace::Create (*this, thread.th32ThreadID));
            if (!m_AllThreads)
            {
                CloseHandle (hToolHelp); return;
            }
        }
    }
    else
    {
        if (thread.th32OwnerProcessID == m_ProcessId)
        {
            m_Threads.push_back (thread_trace::Create (*this, thread.th32ThreadID));
            if (!m_AllThreads)
            {
                CloseHandle (hToolHelp); return;
            }
        }
    }

    while (ERROR_NO_MORE_FILES != hr)
    {
        if (!Thread32Next (hToolHelp, &thread))
        {
            hr = GetLastError ();

            if (ERROR_NO_MORE_FILES != hr)
            {
                CloseHandle (hToolHelp);

                THROW_WINDOWS_EXCEPTION_F(
                    GetLastError(), "Error getting threads for process %d",
                    m_ProcessId);
            }

            if (thread.th32OwnerProcessID == m_ProcessId)
            {
                m_Threads.push_back (thread_trace::Create (*this, thread.th32ThreadID));
                if (!m_AllThreads) break;
            }
        }
        else
        {
            if (thread.th32OwnerProcessID == m_ProcessId)
            {
                m_Threads.push_back (thread_trace::Create (*this, thread.th32ThreadID));
                if (!m_AllThreads) break;
            }
        }
    }

    /** @todo Close handle on exceptions! */
    CloseHandle (hToolHelp);
}

void proclib::process_trace::shutdown ()
{
    // for each thread
    for (threadList_t::iterator to_dest = m_Threads.begin ();
         to_dest != m_Threads.end ();
         ++to_dest)
    {
        (*to_dest)->destroy ();
    }

    // for each module
    for (moduleList_t::iterator to_dest = m_Modules.begin ();
         to_dest != m_Modules.end ();
         ++to_dest)
    {
        to_dest->second->destroy ();
    }

    if (!psystem::dbghelp::SymCleanup (getProcessHandle ()))
    {
        /** @todo Check for return codes! */
    }
}

//////////////////////////////////////////////////////////////////////////////
// Private methods:
//////////////////////////////////////////////////////////////////////////////

/** @brief Returns the code to identify the process' (intended) architecture.
 **/
DWORD proclib::process_trace::initMachineType () const
    throw ()
{
    // USE ImageNtHeader Function?
    /** @todo Have to get the machine type from the image itself. */
    return IMAGE_FILE_MACHINE_I386;
}

/** @brief Set up the debugger to grab symbols from a running executable.
 **/
void proclib::process_trace::initSymbols () const
    throw ()
{

    /** @todo Check for return codes! */
    psystem::dbghelp::SymSetOptions (
        SYMOPT_DEBUG | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_LOAD_ANYTHING |
        SYMOPT_LOAD_LINES | SYMOPT_NO_PROMPTS | SYMOPT_INCLUDE_32BIT_MODULES);

    /** @todo Check for return codes! */
    try
    {
        const std::string& moduleName =
            proclib::process_module::GetProcessFileName (getProcessHandle ());
        psystem::dbghelp::SymInitialize (
            getProcessHandle (), moduleName.c_str (), false);
    }
    catch (psystem::exception::windows_exception&)
    {
        psystem::dbghelp::SymInitialize (getProcessHandle (), NULL, false);
    }
    catch (psystem::exception::null_pointer_exception&)
    {
        // SEE TODO...
    }

    /// @todo Check return code
    psystem::dbghelp::EnumerateLoadedModules64 (
        getProcessHandle (),
        (PENUMLOADED_MODULES_CALLBACK64)&EnumerateLoadedModules,
        (void *)this);
}

/** @brief Callback used by EnumerateLoadedModules64() from the Windows API.
 **/
bool CALLBACK proclib::process_trace::EnumerateLoadedModules (
    const TCHAR       *,
    memAddress_t       moduleBase,
    unsigned long int  moduleSize,
    void              *userdata)
    throw ()
{
    proclib::process_trace *c_this = (proclib::process_trace *)userdata;

    /** @todo What to do here!? */
    if (NULL == c_this) return true;

    /** @note We MUST catch errors in this function, as the API will just
     **       abort its enumeration of the modules.  We'll try to pretend
     **       like nothing is wrong and keep enumerating. */
    try
    {
        c_this->m_Modules[moduleBase] =
            process_module::Create (
                *c_this, moduleBase, moduleSize);
    }
#ifdef _DEBUG
    catch (psystem::exception::base_exception& wex)
    {
        fprintf (stderr, "Exception caught at %s:%d (function %s): %s\n",
                 wex.source_file(), wex.line_number (),
                 wex.function(),    wex.what ());
    }
    catch (std::exception& sex)
    {
        fprintf (stderr, "Exception caught: %s\n",
                 sex.what ());
    }
#endif
    catch (...)
    {
#ifdef _DEBUG
        fprintf(stderr, "Unknown error enumerating modules.\n");
#endif
    }

    return true;
}
