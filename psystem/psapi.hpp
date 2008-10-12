
/** @file psapi.hpp
 ** @brief Declare the wrapper for Microsoft's PS API DLL.
 ** @author  Matt Bisson
 ** @date    14 July, 2008
 ** @since   Proclib 1.3
 ** @version Proclib 1.3
 **/

#ifndef __PSAPI_HPP__
#define __PSAPI_HPP__

#include "psystem_ns.hpp"

#include <runtime_library.hpp>

namespace psystem
{

//////////////////////////////////////////////////////////////////////////////
// Function Wrapper Classes (Used By PS-API):
//////////////////////////////////////////////////////////////////////////////

/** @brief Class to wrap the MCBS PS-API function, GetModuleBaseName() */
class fnGetModuleBaseNameA : public psystem::runtime_function<
    DWORD(WINAPI*) (HANDLE, HMODULE, LPSTR, DWORD)>
{
public:
    friend class psystem::runtime_library;

    /** @brief Default constructor sets the function pointer (may be NULL). */
    fnGetModuleBaseNameA (FARPROC fn = NULL) throw ()
        : runtime_function (fn) { }

    DWORD operator() (
        HANDLE hProcess, HMODULE hModule, char *outBase, DWORD size) const
        throw (psystem::exception::null_pointer_exception);
};

/** @brief Class to wrap the UNICODE PS-API function, GetModuleBaseName() */
class fnGetModuleBaseNameW : public psystem::runtime_function<
    DWORD(WINAPI*) (HANDLE, HMODULE, LPWSTR, DWORD)>
{
public:
    friend class psystem::runtime_library;

    /** @brief Default constructor sets the function pointer (may be NULL). */
    fnGetModuleBaseNameW (FARPROC fn = NULL) throw ()
        : runtime_function (fn) { }

    DWORD operator() (
        HANDLE hProcess, HMODULE hModule, wchar_t *outBase, DWORD size) const
        throw (psystem::exception::null_pointer_exception);
};

/** @brief Class to wrap the MCBS PS-API function, GetModuleFileNameEx() */
class fnGetModuleFileNameExA : public psystem::runtime_function<
    DWORD(WINAPI*) (HANDLE, HMODULE, LPSTR, DWORD)>
{
public:
    friend class psystem::runtime_library;

    /** @brief Default constructor sets the function pointer (may be NULL). */
    fnGetModuleFileNameExA (FARPROC fn = NULL) throw ()
        : runtime_function (fn) { }

    DWORD operator() (
        HANDLE hProcess, HMODULE hModule, char *outFile, DWORD size) const
        throw (psystem::exception::null_pointer_exception);
};

/** @brief Class to wrap the UNICODE PS-API function, GetModuleFileNameEx() */
class fnGetModuleFileNameExW : public psystem::runtime_function<
    DWORD(WINAPI*) (HANDLE, HMODULE, LPWSTR, DWORD)>
{
public:
    friend class psystem::runtime_library;

    /** @brief Default constructor sets the function pointer (may be NULL). */
    fnGetModuleFileNameExW (FARPROC fn = NULL) throw ()
        : runtime_function (fn) { }

    DWORD operator() (
        HANDLE hProcess, HMODULE hModule, wchar_t *outFile, DWORD size) const
        throw (psystem::exception::null_pointer_exception);
};

//////////////////////////////////////////////////////////////////////////////
// PS-API class:
//////////////////////////////////////////////////////////////////////////////

/** @brief Encapsulates Microsoft's PS-API DLL.
 ** @author  Matt Bisson
 ** @date    14 July, 2007
 ** @since   Proclib 1.3
 ** @version Proclib 1.3
 **/
class psapi : public psystem::runtime_library
{
public:
    explicit psapi ();

protected:
    //////////////////////////////////////////////////////////////////////////
    /// @name Inherited Functions
    //////////////////////////////////////////////////////////////////////////

    virtual void unloadFunctions () throw ();

public:
    //////////////////////////////////////////////////////////////////////////
    /// @name PS-API Function Wrappers
    //////////////////////////////////////////////////////////////////////////

    static DWORD GetModuleBaseNameA   (
        HANDLE hProcess, HMODULE hModule, char    *outBase, DWORD size);
    static DWORD GetModuleBaseNameW   (
        HANDLE hProcess, HMODULE hModule, wchar_t *outBase, DWORD size);

    static DWORD GetModuleFileNameExA (
        HANDLE hProcess, HMODULE hModule, char    *outFile, DWORD size);
    static DWORD GetModuleFileNameExW (
        HANDLE hProcess, HMODULE hModule, wchar_t *outFile, DWORD size);

protected:
    //////////////////////////////////////////////////////////////////////////
    /// @name Imported PS-API Functions
    //////////////////////////////////////////////////////////////////////////

    /** @brief Cached function for GetModuleBaseNameA() */
    fnGetModuleBaseNameA   m_GetModuleBaseNameA;

    /** @brief Cached function for GetModuleBaseNameW() */
    fnGetModuleBaseNameW   m_GetModuleBaseNameW;

    /** @brief Cached function for GetModuleFileNameExA() */
    fnGetModuleFileNameExA m_GetModuleFileNameExA;

    /** @brief Cached function for GetModuleFileNameExW() */
    fnGetModuleFileNameExW m_GetModuleFileNameExW;
};

}

#endif // __PSAPI_HPP__
