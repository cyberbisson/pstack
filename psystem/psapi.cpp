
/** @file psapi.cpp
 ** @brief Define the wrapper for Microsoft's PS API DLL.
 ** @author  Matt Bisson
 ** @date    14 July, 2008
 ** @since   Proclib 1.3
 ** @version Proclib 1.3
 **/

#include <exception/dll_load_exception.hpp>
#include <exception/null_pointer_exception.hpp>
#include <exception/windows_exception.hpp>

#include "psapi.hpp"

static psystem::psapi p;

//////////////////////////////////////////////////////////////////////////////
// Template Specialization For loadFunction():
//////////////////////////////////////////////////////////////////////////////

MAKE_LOAD_FUNCTION(GetModuleBaseNameA);

/** @brief Passthrough to the Windows API function. */
DWORD psystem::fnGetModuleBaseNameA::operator() (
    HANDLE hProcess, HMODULE hModule, char *outBase, DWORD size) const
    throw (psystem::exception::null_pointer_exception)
{
    if (!isLoaded ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return (getNativeFunction ()) (hProcess, hModule, outBase, size);
}

MAKE_LOAD_FUNCTION(GetModuleBaseNameW);

/** @brief Passthrough to the Windows API function. */
DWORD psystem::fnGetModuleBaseNameW::operator() (
    HANDLE hProcess, HMODULE hModule, wchar_t *outBase, DWORD size) const
    throw (psystem::exception::null_pointer_exception)
{
    if (!isLoaded ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return (getNativeFunction ()) (hProcess, hModule, outBase, size);
}

MAKE_LOAD_FUNCTION(GetModuleFileNameExA);

/** @brief Passthrough to the Windows API function. */
DWORD psystem::fnGetModuleFileNameExA::operator() (
    HANDLE hProcess, HMODULE hModule, char *outFile, DWORD size) const
    throw (psystem::exception::null_pointer_exception)
{
    if (!isLoaded ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return (getNativeFunction ()) (hProcess, hModule, outFile, size);
}

MAKE_LOAD_FUNCTION(GetModuleFileNameExW);

/** @brief Passthrough to the Windows API function. */
DWORD psystem::fnGetModuleFileNameExW::operator() (
    HANDLE hProcess, HMODULE hModule, wchar_t *outFile, DWORD size) const
    throw (psystem::exception::null_pointer_exception)
{
    if (!isLoaded ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    return (getNativeFunction ()) (hProcess, hModule, outFile, size);
}

//////////////////////////////////////////////////////////////////////////////
// Construction / Destruction:
//////////////////////////////////////////////////////////////////////////////

psystem::psapi::psapi ()
    : runtime_library ("psapi.dll", 5, 1),
      m_GetModuleBaseNameA   (NULL),
      m_GetModuleBaseNameW   (NULL),
      m_GetModuleFileNameExA (NULL),
      m_GetModuleFileNameExW (NULL)
{

}

//////////////////////////////////////////////////////////////////////////////
// Inherited Functions:
//////////////////////////////////////////////////////////////////////////////

void psystem::psapi::unloadFunctions () throw ()
{
    m_GetModuleBaseNameA.clear   ();
    m_GetModuleBaseNameW.clear   ();
    m_GetModuleFileNameExA.clear ();
    m_GetModuleFileNameExW.clear ();
}

//////////////////////////////////////////////////////////////////////////////
// PS-API Function Wrappers:
//////////////////////////////////////////////////////////////////////////////

DWORD psystem::psapi::GetModuleBaseNameA (
    HANDLE hProcess, HMODULE hModule, char *outBase, DWORD size)
    throw (psystem::exception::dll_load_exception)
{
    if (!p.m_GetModuleBaseNameA.isLoaded ())
    {
        p.m_GetModuleBaseNameA = p.loadFunction<fnGetModuleBaseNameA> ();
    }

    return p.m_GetModuleBaseNameA (hProcess, hModule, outBase, size);
}

DWORD psystem::psapi::GetModuleBaseNameW (
    HANDLE hProcess, HMODULE hModule, wchar_t *outBase, DWORD size)
{
    if (!p.m_GetModuleBaseNameW.isLoaded ())
    {
        p.m_GetModuleBaseNameW = p.loadFunction<fnGetModuleBaseNameW> ();
    }

    return p.m_GetModuleBaseNameW (hProcess, hModule, outBase, size);
}

DWORD psystem::psapi::GetModuleFileNameExA (
    HANDLE hProcess, HMODULE hModule, char *outFile, DWORD size)
{
    if (!p.m_GetModuleFileNameExA.isLoaded ())
    {
        p.m_GetModuleFileNameExA = p.loadFunction<fnGetModuleFileNameExA> ();
    }

    return p.m_GetModuleFileNameExA (hProcess, hModule, outFile, size);
}

DWORD psystem::psapi::GetModuleFileNameExW (
    HANDLE hProcess, HMODULE hModule, wchar_t *outFile, DWORD size)
{
    if (!p.m_GetModuleFileNameExW.isLoaded ())
    {
        p.m_GetModuleFileNameExW = p.loadFunction<fnGetModuleFileNameExW> ();
    }

    return p.m_GetModuleFileNameExW (hProcess, hModule, outFile, size);
}
