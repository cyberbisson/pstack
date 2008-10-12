
/** @file dbghelp.hpp
 ** @brief Declare the wrapper for Microsoft's Debug Helper DLL.
 ** @author  Matt Bisson
 ** @date    14 July, 2008
 ** @since   Proclib 1.3
 ** @version Proclib 1.3
 **/

#ifndef __DBG_HELP_HPP__
#define __DBG_HELP_HPP__

#include "psystem_ns.hpp"

#include <dbghelp.h>
#include <runtime_library.hpp>

namespace psystem
{

//////////////////////////////////////////////////////////////////////////////
// Function Wrapper Classes (Used By PS-API):
//////////////////////////////////////////////////////////////////////////////

/** @brief Class to wrap the DbgHelp function, EnumerateLoadedModules64() */
class fnEnumerateLoadedModules64 : public psystem::runtime_function<
    BOOL(WINAPI*)(HANDLE, PENUMLOADED_MODULES_CALLBACK64, void *)>
{
public:
    friend class psystem::runtime_library;

    /** @brief Default constructor sets the function pointer (may be NULL). */
    fnEnumerateLoadedModules64 (FARPROC fn = NULL) throw ()
        : runtime_function (fn) { }

    bool operator() (
        HANDLE                          hProcess,
        PENUMLOADED_MODULES_CALLBACK64  callback,
        void                           *userContext) const
        throw (psystem::exception::null_pointer_exception);
};

/** @brief Class to wrap the DbgHelp function, StackWalk64() */
class fnStackWalk64 : public psystem::runtime_function<
    BOOL(WINAPI*)(DWORD, HANDLE, HANDLE, STACKFRAME64 *, void *,
                  PREAD_PROCESS_MEMORY_ROUTINE64,
                  PFUNCTION_TABLE_ACCESS_ROUTINE64,
                  PGET_MODULE_BASE_ROUTINE64,
                  PTRANSLATE_ADDRESS_ROUTINE64)>
{
public:
    friend class psystem::runtime_library;

    /** @brief Default constructor sets the function pointer (may be NULL). */
    fnStackWalk64 (FARPROC fn = NULL) throw ()
        : runtime_function (fn) { }

    bool operator() (
        DWORD                             machineType,
        HANDLE                            hProcess,
        HANDLE                            hThread,
        STACKFRAME64                     *stackFrame,
        void                             *contextRecord,
        PREAD_PROCESS_MEMORY_ROUTINE64    readMemoryRoutine,
        PFUNCTION_TABLE_ACCESS_ROUTINE64  functionTableAccessRoutine,
        PGET_MODULE_BASE_ROUTINE64        getModuleBaseRoutine,
        PTRANSLATE_ADDRESS_ROUTINE64      translateAddress) const
        throw (psystem::exception::null_pointer_exception);
};

/** @brief Class to wrap the DbgHelp function, SymCleanup() */
class fnSymCleanup : public psystem::runtime_function<BOOL(WINAPI*)(HANDLE)>
{
public:
    friend class psystem::runtime_library;

    /** @brief Default constructor sets the function pointer (may be NULL). */
    fnSymCleanup (FARPROC fn = NULL) throw ()
        : runtime_function (fn) { }

    bool operator() (HANDLE hProcess) const
        throw (psystem::exception::null_pointer_exception);
};

/** @brief Class to wrap the DbgHelp function, SymFromAddr() */
class fnSymFromAddr : public psystem::runtime_function<
    BOOL(WINAPI*)(HANDLE, DWORD64, DWORD64 *, SYMBOL_INFO *)>
{
public:
    friend class psystem::runtime_library;

    /** @brief Default constructor sets the function pointer (may be NULL). */
    fnSymFromAddr (FARPROC fn = NULL) throw ()
        : runtime_function (fn) { }

    bool operator() (
        HANDLE       hProcess,
        DWORD64      address,
        DWORD64     *displacement,
        SYMBOL_INFO *symbol) const
        throw (psystem::exception::null_pointer_exception);
};

/** @brief Class to wrap the DbgHelp function, SymFunctionTableAccess64() */
class fnSymFunctionTableAccess64 : public psystem::runtime_function<
    void*(WINAPI*)(HANDLE, DWORD64)>
{
public:
    friend class psystem::runtime_library;

    /** @brief Default constructor sets the function pointer (may be NULL). */
    fnSymFunctionTableAccess64 (FARPROC fn = NULL) throw ()
        : runtime_function (fn) { }

    void *operator() (HANDLE hProcess, DWORD64 addrBase) const
        throw (psystem::exception::null_pointer_exception);
};

/** @brief Class to wrap the DbgHelp function, SymGetLineFromAddr64() */
class fnSymGetLineFromAddr64 : public psystem::runtime_function<
    BOOL(WINAPI*)(HANDLE, DWORD64, DWORD *, IMAGEHLP_LINE64 *)>
{
public:
    friend class psystem::runtime_library;

    /** @brief Default constructor sets the function pointer (may be NULL). */
    fnSymGetLineFromAddr64 (FARPROC fn = NULL) throw ()
        : runtime_function (fn) { }

    bool operator() (
        HANDLE           hProcess,
        DWORD64          addr,
        DWORD           *displacement,
        IMAGEHLP_LINE64 *line) const
        throw (psystem::exception::null_pointer_exception);
};

/** @brief Class to wrap the DbgHelp function, SymGetModuleBase64() */
class fnSymGetModuleBase64 : public psystem::runtime_function<
    DWORD64(WINAPI*)(HANDLE, DWORD64)>
{
public:
    friend class psystem::runtime_library;

    /** @brief Default constructor sets the function pointer (may be NULL). */
    fnSymGetModuleBase64 (FARPROC fn = NULL) throw ()
        : runtime_function (fn) { }

    DWORD64 operator() (HANDLE hProcess, DWORD64 addr) const
        throw (psystem::exception::null_pointer_exception);
};

/** @brief Class to wrap the DbgHelp function, SymGetModuleInfo64() */
class fnSymGetModuleInfo64 : public psystem::runtime_function<
    BOOL(WINAPI*)(HANDLE, DWORD64, IMAGEHLP_MODULE64 *)>
{
public:
    friend class psystem::runtime_library;

    /** @brief Default constructor sets the function pointer (may be NULL). */
    fnSymGetModuleInfo64 (FARPROC fn = NULL) throw ()
        : runtime_function (fn) { }

    bool operator() (
        HANDLE hProcess, DWORD64 addr, IMAGEHLP_MODULE64 *moduleInfo) const
        throw (psystem::exception::null_pointer_exception);
};

/** @brief Class to wrap the DbgHelp function, SymInitialize() */
class fnSymInitialize : public psystem::runtime_function<
    BOOL(WINAPI*)(HANDLE, const char *, BOOL)>
{
public:
    friend class psystem::runtime_library;

    /** @brief Default constructor sets the function pointer (may be NULL). */
    fnSymInitialize (FARPROC fn = NULL) throw ()
        : runtime_function (fn) { }

    bool operator() (
        HANDLE hProcess, const char *userSearchPath, bool invadeProcess) const
        throw (psystem::exception::null_pointer_exception);
};

/** @brief Class to wrap the DbgHelp function, SymLoadModule64() */
class fnSymLoadModule64 : public psystem::runtime_function<
    DWORD64(WINAPI*)(HANDLE, HANDLE, const char *, const char *, DWORD64, DWORD)>
{
public:
    friend class psystem::runtime_library;

    /** @brief Default constructor sets the function pointer (may be NULL). */
    fnSymLoadModule64 (FARPROC fn = NULL) throw ()
        : runtime_function (fn) { }

    DWORD64 operator() (
        HANDLE      hProcess,
        HANDLE      hFile,
        const char *imageName,
        const char *moduleName,
        DWORD64     baseOfDll,
        DWORD       sizeOfDll) const
        throw (psystem::exception::null_pointer_exception);
};

/** @brief Class to wrap the DbgHelp function, SymSetOptions() */
class fnSymSetOptions : public psystem::runtime_function<DWORD(WINAPI*)(DWORD)>
{
public:
    friend class psystem::runtime_library;

    /** @brief Default constructor sets the function pointer (may be NULL). */
    fnSymSetOptions (FARPROC fn = NULL) throw ()
        : runtime_function (fn) { }

    DWORD operator() (DWORD opts) const
        throw (psystem::exception::null_pointer_exception);
};

//////////////////////////////////////////////////////////////////////////////
// DbgHelp class:
//////////////////////////////////////////////////////////////////////////////

/** @brief Encapsulates Microsoft's DbgHelp DLL.
 **
 ** Versions 5 and below of this DLL make for some odd behavior.
 ** @author  Matt Bisson
 ** @date    14 July, 2007
 ** @since   Proclib 1.3
 ** @version Proclib 1.3
 **/
class dbghelp : public psystem::runtime_library
{
public:
    explicit dbghelp ();

protected:
    //////////////////////////////////////////////////////////////////////////
    /// @name Inherited Functions
    //////////////////////////////////////////////////////////////////////////

    virtual void unloadFunctions () throw ();

public:
    //////////////////////////////////////////////////////////////////////////
    /// @name DbgHelp Function Wrappers
    //////////////////////////////////////////////////////////////////////////

    static bool EnumerateLoadedModules64 (
        HANDLE                          hProcess,
        PENUMLOADED_MODULES_CALLBACK64  callback,
        void                           *userContext);

    static bool StackWalk64 (
        DWORD                             machineType,
        HANDLE                            hProcess,
        HANDLE                            hThread,
        STACKFRAME64                     *stackFrame,
        void                             *contextRecord,
        PREAD_PROCESS_MEMORY_ROUTINE64    readMemoryRoutine,
        PFUNCTION_TABLE_ACCESS_ROUTINE64  functionTableAccessRoutine,
        PGET_MODULE_BASE_ROUTINE64        getModuleBaseRoutine,
        PTRANSLATE_ADDRESS_ROUTINE64      translateAddress);

    static bool SymCleanup (HANDLE hProcess);

    static bool SymFromAddr (
        HANDLE       hProcess,
        DWORD64      address,
        DWORD64     *displacement,
        SYMBOL_INFO *symbol);

    static void *SymFunctionTableAccess64 (HANDLE hProcess, DWORD64 addrBase);

    static bool SymGetLineFromAddr64 (
        HANDLE           hProcess,
        DWORD64          addr,
        DWORD           *displacement,
        IMAGEHLP_LINE64 *line);

    static DWORD64 SymGetModuleBase64 (HANDLE hProcess, DWORD64 addr);

    static bool SymGetModuleInfo64 (
        HANDLE hProcess, DWORD64 addr, IMAGEHLP_MODULE64 *moduleInfo);

    static bool SymInitialize (
        HANDLE hProcess, const char *userSearchPath, bool invadeProcess);

    static DWORD64 SymLoadModule64 (
        HANDLE      hProcess,
        HANDLE      hFile,
        const char *imageName,
        const char *moduleName,
        DWORD64     baseOfDll,
        DWORD       sizeOfDll);

    static DWORD SymSetOptions (DWORD opts);

protected:
    //////////////////////////////////////////////////////////////////////////
    /// @name Imported DbgHelp Functions
    //////////////////////////////////////////////////////////////////////////

    /** @brief Cached function for EnumerateLoadedModules64() */
    fnEnumerateLoadedModules64 m_EnumerateLoadedModules64;

    /** @brief Cached function for StackWalk64() */
    fnStackWalk64              m_StackWalk64;

    /** @brief Cached function for SymCleanup() */
    fnSymCleanup               m_SymCleanup;

    /** @brief Cached function for SymFromAddr() */
    fnSymFromAddr              m_SymFromAddr;

    /** @brief Cached function for SymFunctionTableAccess64() */
    fnSymFunctionTableAccess64 m_SymFunctionTableAccess64;

    /** @brief Cached function for SymGetLineFromAddr64() */
    fnSymGetLineFromAddr64     m_SymGetLineFromAddr64;

    /** @brief Cached function for SymGetModuleBase64() */
    fnSymGetModuleBase64       m_SymGetModuleBase64;

    /** @brief Cached function for SymGetModuleInfo64() */
    fnSymGetModuleInfo64       m_SymGetModuleInfo64;

    /** @brief Cached function for SymInitialize() */
    fnSymInitialize            m_SymInitialize;

    /** @brief Cached function for SymLoadModule64() */
    fnSymLoadModule64          m_SymLoadModule64;

    /** @brief Cached function for SymSetOptions() */
    fnSymSetOptions            m_SymSetOptions;
};

}

#endif // __DBG_HELP_HPP__
