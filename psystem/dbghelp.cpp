
/** @file dbghelp.cpp
 ** @brief Define the wrapper class for Microsoft's Debug Helper DLL.
 ** @author  Matt Bisson
 ** @date    14 July, 2008
 ** @since   Proclib 1.3
 ** @version Proclib 1.3
 **/

#include <exception/dll_load_exception.hpp>
#include <exception/null_pointer_exception.hpp>
#include <exception/windows_exception.hpp>

#include "dbghelp.hpp"

/** @brief Static singleton instance of the DbgHelp.dll helper module. */
static psystem::dbghelp d;

//////////////////////////////////////////////////////////////////////////////
// Template Specialization For loadFunction():
//////////////////////////////////////////////////////////////////////////////

MAKE_LOAD_FUNCTION(EnumerateLoadedModules64);

/** @brief Passthrough to the Windows API function. */
bool psystem::fnEnumerateLoadedModules64::operator() (
    HANDLE                          hProcess,
    PENUMLOADED_MODULES_CALLBACK64  callback,
    void                           *userContext) const
    throw (psystem::exception::null_pointer_exception)
{
    if (!isLoaded ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return (0 != (getNativeFunction ()) (hProcess, callback, userContext));
}

MAKE_LOAD_FUNCTION(StackWalk64);

/** @brief Passthrough to the Windows API function. */
bool psystem::fnStackWalk64::operator() (
    DWORD                             machineType,
    HANDLE                            hProcess,
    HANDLE                            hThread,
    STACKFRAME64                     *stackFrame,
    void                             *contextRecord,
    PREAD_PROCESS_MEMORY_ROUTINE64    readMemoryRoutine,
    PFUNCTION_TABLE_ACCESS_ROUTINE64  functionTableAccessRoutine,
    PGET_MODULE_BASE_ROUTINE64        getModuleBaseRoutine,
    PTRANSLATE_ADDRESS_ROUTINE64      translateAddress) const
    throw (psystem::exception::null_pointer_exception)
{
    if (!isLoaded ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return (0 != (getNativeFunction ()) (
                machineType, hProcess, hThread, stackFrame, contextRecord,
                readMemoryRoutine, functionTableAccessRoutine,
                getModuleBaseRoutine, translateAddress));
}

MAKE_LOAD_FUNCTION(SymCleanup);

/** @brief Passthrough to the Windows API function. */
bool psystem::fnSymCleanup::operator() (HANDLE hProcess) const
    throw (psystem::exception::null_pointer_exception)
{
    if (!isLoaded ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return (0 != (getNativeFunction ()) (hProcess));
}

MAKE_LOAD_FUNCTION(SymFromAddr);

/** @brief Passthrough to the Windows API function. */
bool psystem::fnSymFromAddr::operator() (
    HANDLE hProcess, DWORD64 address, DWORD64 *displacement, SYMBOL_INFO *symbol)
    const throw (psystem::exception::null_pointer_exception)
{
    if (!isLoaded ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return (0 != (getNativeFunction ()) (
                hProcess, address, displacement, symbol));
}

MAKE_LOAD_FUNCTION(SymFunctionTableAccess64);

/** @brief Passthrough to the Windows API function. */
void *psystem::fnSymFunctionTableAccess64::operator() (
    HANDLE hProcess, DWORD64 addrBase) const
    throw (psystem::exception::null_pointer_exception)
{
    if (!isLoaded ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return (getNativeFunction ()) (hProcess, addrBase);
}

MAKE_LOAD_FUNCTION(SymGetLineFromAddr64);

/** @brief Passthrough to the Windows API function. */
bool psystem::fnSymGetLineFromAddr64::operator() (
    HANDLE           hProcess,
    DWORD64          addr,
    DWORD           *displacement,
    IMAGEHLP_LINE64 *line) const
    throw (psystem::exception::null_pointer_exception)
{
    if (!isLoaded ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return (0 != (getNativeFunction ()) (hProcess, addr, displacement, line));
}

MAKE_LOAD_FUNCTION(SymGetModuleBase64);

/** @brief Passthrough to the Windows API function. */
DWORD64 psystem::fnSymGetModuleBase64::operator() (HANDLE hProcess, DWORD64 addr)
    const throw (psystem::exception::null_pointer_exception)
{
    if (!isLoaded ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return (getNativeFunction ()) (hProcess, addr);
}

MAKE_LOAD_FUNCTION(SymGetModuleInfo64);

/** @brief Passthrough to the Windows API function. */
bool psystem::fnSymGetModuleInfo64::operator() (
    HANDLE hProcess, DWORD64 addr, IMAGEHLP_MODULE64 *moduleInfo) const
    throw (psystem::exception::null_pointer_exception)
{
    if (!isLoaded ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return (0 != (getNativeFunction ()) (hProcess, addr, moduleInfo));
}

MAKE_LOAD_FUNCTION(SymInitialize);

/** @brief Passthrough to the Windows API function. */
bool psystem::fnSymInitialize::operator() (
    HANDLE hProcess, const char *userSearchPath, bool invadeProcess) const
    throw (psystem::exception::null_pointer_exception)
{
    if (!isLoaded ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return (0 != (getNativeFunction ()) (
                hProcess, userSearchPath, invadeProcess));
}

MAKE_LOAD_FUNCTION(SymLoadModule64);

/** @brief Passthrough to the Windows API function. */
DWORD64 psystem::fnSymLoadModule64::operator() (
    HANDLE      hProcess,
    HANDLE      hFile,
    const char *imageName,
    const char *moduleName,
    DWORD64     baseOfDll,
    DWORD       sizeOfDll) const
    throw (psystem::exception::null_pointer_exception)
{
    if (!isLoaded ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return (getNativeFunction ()) (
        hProcess, hFile, imageName, moduleName, baseOfDll, sizeOfDll);
}

MAKE_LOAD_FUNCTION(SymSetOptions);

/** @brief Passthrough to the Windows API function. */
DWORD psystem::fnSymSetOptions::operator() (DWORD opts) const
    throw (psystem::exception::null_pointer_exception)
{
    if (!isLoaded ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return (getNativeFunction ()) (opts);
}
#if 0
MAKE_LOAD_FUNCTION();

/** @brief Passthrough to the Windows API function. */
 psystem::fn::operator() (
    ) const
    throw (psystem::exception::null_pointer_exception)
{
    if (!isLoaded ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return (getNativeFunction ()) ();
}
#endif
//////////////////////////////////////////////////////////////////////////////
// Construction / Destruction:
//////////////////////////////////////////////////////////////////////////////

psystem::dbghelp::dbghelp ()
    : runtime_library ("dbghelp.dll", 6, 4),
      m_EnumerateLoadedModules64 (NULL),
      m_StackWalk64              (NULL),
      m_SymCleanup               (NULL),
      m_SymFromAddr              (NULL),
      m_SymFunctionTableAccess64 (NULL),
      m_SymGetLineFromAddr64     (NULL),
      m_SymGetModuleBase64       (NULL),
      m_SymGetModuleInfo64       (NULL),
      m_SymInitialize            (NULL),
      m_SymLoadModule64          (NULL),
      m_SymSetOptions            (NULL)
{

}

//////////////////////////////////////////////////////////////////////////////
// Inherited Functions:
//////////////////////////////////////////////////////////////////////////////

void psystem::dbghelp::unloadFunctions () throw ()
{
    m_EnumerateLoadedModules64.clear  ();
    m_StackWalk64.clear               ();
    m_SymCleanup.clear                ();
    m_SymFromAddr.clear               ();
    m_SymFunctionTableAccess64.clear  ();
    m_SymGetLineFromAddr64.clear      ();
    m_SymGetModuleBase64.clear        ();
    m_SymGetModuleInfo64.clear        ();
    m_SymInitialize.clear             ();
    m_SymLoadModule64.clear           ();
    m_SymSetOptions.clear             ();
}

//////////////////////////////////////////////////////////////////////////////
// DbgHelp Function Wrappers:
//////////////////////////////////////////////////////////////////////////////

bool psystem::dbghelp::EnumerateLoadedModules64 (
    HANDLE                          hProcess,
    PENUMLOADED_MODULES_CALLBACK64  callback,
    void                           *userContext)
{
    if (!d.m_EnumerateLoadedModules64.isLoaded ())
    {
        d.m_EnumerateLoadedModules64 =
            d.loadFunction<fnEnumerateLoadedModules64> ();
    }

    return d.m_EnumerateLoadedModules64 (hProcess, callback, userContext);
}

bool psystem::dbghelp::StackWalk64 (
    DWORD                             machineType,
    HANDLE                            hProcess,
    HANDLE                            hThread,
    STACKFRAME64                     *stackFrame,
    void                             *contextRecord,
    PREAD_PROCESS_MEMORY_ROUTINE64    readMemoryRoutine,
    PFUNCTION_TABLE_ACCESS_ROUTINE64  functionTableAccessRoutine,
    PGET_MODULE_BASE_ROUTINE64        getModuleBaseRoutine,
    PTRANSLATE_ADDRESS_ROUTINE64      translateAddress)
{
    if (!d.m_StackWalk64.isLoaded ())
    {
        d.m_StackWalk64 = d.loadFunction<fnStackWalk64> ();
    }

    return d.m_StackWalk64 (
        machineType,
        hProcess,
        hThread,
        stackFrame,
        contextRecord,
        readMemoryRoutine,
        functionTableAccessRoutine,
        getModuleBaseRoutine,
        translateAddress);
}

bool psystem::dbghelp::SymCleanup (HANDLE hProcess)
{
    if (!d.m_SymCleanup.isLoaded ())
    {
        d.m_SymCleanup = d.loadFunction<fnSymCleanup> ();
    }

    return d.m_SymCleanup (hProcess);
}

bool psystem::dbghelp::SymFromAddr (
    HANDLE       hProcess,
    DWORD64      address,
    DWORD64     *displacement,
    SYMBOL_INFO *symbol)
{
    if (!d.m_SymFromAddr.isLoaded ())
    {
        d.m_SymFromAddr = d.loadFunction<fnSymFromAddr> ();
    }

    return d.m_SymFromAddr (hProcess, address, displacement, symbol);
}

void *psystem::dbghelp::SymFunctionTableAccess64 (
    HANDLE hProcess, DWORD64 addrBase)
{
    if (!d.m_SymFunctionTableAccess64.isLoaded ())
    {
        d.m_SymFunctionTableAccess64 =
            d.loadFunction<fnSymFunctionTableAccess64> ();
    }

    return d.m_SymFunctionTableAccess64 (hProcess, addrBase);
}

bool psystem::dbghelp::SymGetLineFromAddr64 (
    HANDLE           hProcess,
    DWORD64          addr,
    DWORD           *displacement,
    IMAGEHLP_LINE64 *line)
{
    if (!d.m_SymGetLineFromAddr64.isLoaded ())
    {
        d.m_SymGetLineFromAddr64 = d.loadFunction<fnSymGetLineFromAddr64> ();
    }

    return d.m_SymGetLineFromAddr64 (hProcess, addr, displacement, line);
}

DWORD64 psystem::dbghelp::SymGetModuleBase64 (HANDLE hProcess, DWORD64 addr)
{
    if (!d.m_SymGetModuleBase64.isLoaded ())
    {
        d.m_SymGetModuleBase64 = d.loadFunction<fnSymGetModuleBase64> ();
    }

    return d.m_SymGetModuleBase64 (hProcess, addr);
}

bool psystem::dbghelp::SymGetModuleInfo64 (
    HANDLE hProcess, DWORD64 addr, IMAGEHLP_MODULE64 *moduleInfo)
{
    if (!d.m_SymGetModuleInfo64.isLoaded ())
    {
        d.m_SymGetModuleInfo64 = d.loadFunction<fnSymGetModuleInfo64> ();
    }

    return d.m_SymGetModuleInfo64 (hProcess, addr, moduleInfo);
}

bool psystem::dbghelp::SymInitialize (
    HANDLE hProcess, const char *userSearchPath, bool invadeProcess)
{
    if (!d.m_SymInitialize.isLoaded ())
    {
        d.m_SymInitialize = d.loadFunction<fnSymInitialize> ();
    }

    return  d.m_SymInitialize (hProcess, userSearchPath, invadeProcess);
}

DWORD64 psystem::dbghelp::SymLoadModule64 (
    HANDLE      hProcess,
    HANDLE      hFile,
    const char *imageName,
    const char *moduleName,
    DWORD64     baseOfDll,
    DWORD       sizeOfDll)
{
    if (!d.m_SymLoadModule64.isLoaded ())
    {
        d.m_SymLoadModule64 = d.loadFunction<fnSymLoadModule64> ();
    }

    return d.m_SymLoadModule64 (
        hProcess, hFile, imageName, moduleName, baseOfDll, sizeOfDll);
}

DWORD psystem::dbghelp::SymSetOptions (DWORD opts)
{
    if (!d.m_SymSetOptions.isLoaded ())
    {
        d.m_SymSetOptions = d.loadFunction<fnSymSetOptions> ();
    }

    return d.m_SymSetOptions (opts);
}
