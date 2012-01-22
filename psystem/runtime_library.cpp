
/** @file runtime_library.cpp
 ** @brief Define the interfaces for explicitly managed shared-libraries.
 **
 ** @author  Matt Bisson
 ** @date    14 July, 2008
 ** @since   Proclib 1.3
 ** @version Proclib 1.3
 **/

#include <exception/base_exception.hpp>
#include <exception/dll_load_exception.hpp>
#include <exception/internal_exception.hpp>
#include <exception/null_pointer_exception.hpp>
#include <exception/windows_exception.hpp>

#include "runtime_library.hpp"

//////////////////////////////////////////////////////////////////////////////
// Construction / Destruction:
//////////////////////////////////////////////////////////////////////////////

/** @brief Constructs an object with a "raw" required version number.
 **
 ** @param libname The name of a loadable DLL.  This may not be NULL.
 ** @param version This is a Windows file version number.  This means that the
 **     first 16 bits specify the major version and the last 16 bits specify
 **     the minor version.  If this is 0, there's no restriction on file
 **     versions.
 ** @throw null_pointer_exception If the passed in library name was NULL (or
 **        empty).
 **/
psystem::runtime_library::runtime_library (
    const char *libname, unsigned int version /*=0*/)
    throw (psystem::exception::null_pointer_exception)
    : m_LibName ((NULL != libname) ? libname : ""),
      m_Library (NULL)
{
    m_RequiredVersion.raw = version;

    if (m_LibName.empty ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }
}

/** @brief Constructs an object with a with a minimum version requirement.
 **
 ** @param libname The name of a loadable DLL.  This may not be NULL.
 ** @param major_version The minimum required version of this DLL.
 ** @param minor_version The minimum required (minor) version of this DLL.
 ** @throw null_pointer_exception If the passed in library name was NULL (or
 **        empty).
 **/
psystem::runtime_library::runtime_library (
    const char         *libname,
    unsigned short int  major_version,
    unsigned short int  minor_version)
    throw (psystem::exception::null_pointer_exception)
    : m_LibName ((NULL != libname) ? libname : ""),
      m_Library (NULL)
{
    m_RequiredVersion.version.major = major_version;
    m_RequiredVersion.version.minor = minor_version;

    if (m_LibName.empty ())
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }
}

/** @brief Destruction releases a reference to the library.
 **/
psystem::runtime_library::~runtime_library () throw ()
{
    try
    {
        unloadLibrary ();
    }
    catch (...)
    {
#ifdef _DEBUG        
        fprintf (stderr, "%s: Clobbering exception!\n", __FUNCTION__);
#endif
    }
}

//////////////////////////////////////////////////////////////////////////////
// Public Interface
//////////////////////////////////////////////////////////////////////////////

/** @brief Load the DLL into this application's address space.
 **
 ** This function will first validate the version requirements on the DLL, then
 ** call the system API's to load the image.  If the image has been loaded once
 ** (and not unloaded), this function simply returns.
 ** @throw dll_load_exception If there were errors loading the DLL.
 **/
void psystem::runtime_library::loadLibrary ()
    throw (psystem::exception::dll_load_exception)
{
    if (NULL != m_Library) return;

    try
    {
        // If we don't have a version requirement, don't even bother checking!
        if (0 != m_RequiredVersion.raw)
        {
            checkVersionInfo ();
        }

        if (NULL == (m_Library = LoadLibrary (m_LibName.c_str ())))
        {
            THROW_WINDOWS_EXCEPTION_F(
                GetLastError(), "Cannot load DLL: \"%s\"", m_LibName.c_str ());
        }
    }
    catch (psystem::exception::windows_exception& wex)
    {
        throw psystem::exception::dll_load_exception (
            wex.source_file (), wex.function (), wex.line_number (),
            wex.what ());
    }
    catch (psystem::exception::internal_exception& iex)
    {
        throw psystem::exception::dll_load_exception (
            iex.source_file (), iex.function (), iex.line_number (),
            iex.what ());
    }
}

/** @brief Decrease the reference count on a loaded DLL.
 ** @throw windows_exception If we couldn't unload the DLL for some reason.
 **/
void psystem::runtime_library::unloadLibrary ()
    throw (psystem::exception::windows_exception)
{
    if (NULL == m_Library) return;

    if (FALSE == FreeLibrary (m_Library))
    {
        // Pretend like we freed it anyway...
        m_Library = NULL;
        THROW_WINDOWS_EXCEPTION_F(
            GetLastError(), "Cannot free DLL: \"%s\"", m_LibName.c_str ());
    }

    m_Library = NULL;
}

//////////////////////////////////////////////////////////////////////////////
// Protected Member Functions:
//////////////////////////////////////////////////////////////////////////////

/** @brief Read version information from the disk file and possibly throw.
 ** @throw internal_exception If the DLL file format was unrecognized.
 ** @throw windows_exception If there were problems resulting from Win32
 **     functions loading the DLL.
 **/
void psystem::runtime_library::checkVersionInfo ()
    throw (psystem::exception::internal_exception,
           psystem::exception::windows_exception)
{
    char *vi_buf = NULL;

    try
    {
        DWORD version_info_sz = 0;

        DWORD vi_size = GetFileVersionInfoSize (
            m_LibName.c_str (), &version_info_sz);
        if (0 == vi_size)
        {
            THROW_WINDOWS_EXCEPTION_F(
                GetLastError(),
                "Error getting version information (size) for %s",
                m_LibName.c_str ());
        }

        vi_buf = new char [vi_size];
        if (NULL == vi_buf) throw std::bad_alloc ();

        if (!GetFileVersionInfo (
                m_LibName.c_str (), NULL, vi_size, (void *)vi_buf))
        {
            THROW_WINDOWS_EXCEPTION_F(
                GetLastError(), "Error getting version information for %s",
                m_LibName.c_str ());
        }

        VS_FIXEDFILEINFO *ffi      = NULL;
        size_t            ffi_size = 0;

        if (!VerQueryValue ((void *)vi_buf, "\\", (void **)(&ffi), &ffi_size))
        {
            THROW_WINDOWS_EXCEPTION_F(
                GetLastError(), "Error querying version information for %s",
                m_LibName.c_str ());
        }

        // Sanity check: sig must = 0xFEEF04BD
        if (0xFEEF04BD != ffi->dwSignature)
        {
            throw psystem::exception::internal_exception (
                __FILE__, __FUNCTION__, __LINE__,
                "DLL (%s) had unrecognized file format!",
                m_LibName.c_str ());
        }

#if 0
        dumpVersionInfo (*ffi);
#endif

        if (m_RequiredVersion.raw > ffi->dwFileVersionMS)
        {
            throw psystem::exception::internal_exception (
                __FILE__, __FUNCTION__, __LINE__,
                "Need version %u.%u or higher of %s.",
                m_RequiredVersion.version.major,
                m_RequiredVersion.version.minor,
                m_LibName.c_str ());
        }

        delete [] vi_buf;
    }
    catch (...)
    {
        /** Use scoped object for cleanup */
        delete [] vi_buf;
        throw;
    }
}

/** @brief Dump a given version info structure to the screen.
 ** @param ffi A block of version data (hopefully related to this object).
 **/
void psystem::runtime_library::dumpVersionInfo (const VS_FIXEDFILEINFO& ffi)
    const throw ()
{
    printf ("Version info for \"%s\"\n", m_LibName.c_str ());
    printf ("    Signature:         0x%08X\n", ffi.dwSignature);
    printf ("    Version Struct:    %u.%u\n",
            ffi.dwStrucVersion >> 16,
            ffi.dwStrucVersion & 0xFFFF);
    printf ("    File version:      %u.%u.%u.%u\n",
            ffi.dwFileVersionMS >> 16,
            ffi.dwFileVersionMS & 0xFFFF,
            ffi.dwFileVersionLS >> 16,
            ffi.dwFileVersionLS & 0xFFFF);
    printf ("    Product version:   %u.%u.%u.%u\n",
            ffi.dwProductVersionMS >> 16,
            ffi.dwProductVersionMS & 0xFFFF,
            ffi.dwProductVersionLS >> 16,
            ffi.dwProductVersionLS & 0xFFFF);

    printf ("    File-Flags Mask:   0x%08X\n", ffi.dwFileFlagsMask);

    {
        const DWORD tmpFlags = ffi.dwFileFlags & ffi.dwFileFlagsMask;

        printf ("    File-Flags:        ( ");
        if (VS_FF_DEBUG        & tmpFlags) printf ("DEBUG ");
        if (VS_FF_PRERELEASE   & tmpFlags) printf ("PRERELEASE ");
        if (VS_FF_PATCHED      & tmpFlags) printf ("PATCHED ");
        if (VS_FF_PRIVATEBUILD & tmpFlags) printf ("PRIVATEBUILD ");
        if (VS_FF_INFOINFERRED & tmpFlags) printf ("INFOINFERRED ");
        if (VS_FF_SPECIALBUILD & tmpFlags) printf ("SPECIALBUILD ");
        printf (")\n");
    }

    printf ("    Intended OS:       ");
    switch (ffi.dwFileOS)
    {
    case VOS_DOS:           printf ("MS-DOS"); break;
    case VOS_OS216:         printf ("OS/2 16-bit"); break;
    case VOS_OS232:         printf ("OS/2 32-bit"); break;
    case VOS_NT:            printf ("Windows NT"); break;
    case VOS_WINCE:         printf ("Windows CE"); break;
    case VOS__WINDOWS16:    printf ("Windows 16-bit"); break;
    case VOS__PM16:         printf ("Presentation Mgr 16-bit"); break;
    case VOS__PM32:         printf ("Presentation Mgr 32-bit"); break;
    case VOS__WINDOWS32:    printf ("Windows 32-bit"); break;
    case VOS_DOS_WINDOWS16: printf ("Windows on MS-DOS 16-bit"); break;
    case VOS_DOS_WINDOWS32: printf ("Windows on MS-DOS 32-bit"); break;
    case VOS_OS216_PM16:    printf ("OS/2 on Presentation Mgr 16-bit"); break;
    case VOS_OS232_PM32:    printf ("OS/2 on Presentation Mgr 32-bit"); break;
    case VOS_NT_WINDOWS32:  printf ("Windows NT 32-bit"); break;
    case VOS_UNKNOWN:
    default:
        printf ("Unknown (0x%08X)", ffi.dwFileOS);
    }
    printf ("\n");

    printf ("    File Type:         ");
    switch (ffi.dwFileType)
    {
    case VFT_APP:
        printf ("Application");
        break;
    case VFT_DLL:
        printf ("DLL");
        break;
    case VFT_STATIC_LIB:
        printf ("Static Library");
        break;

        // I don't care enough to fill in the subtypes...
    case VFT_DRV:
        printf ("Device Driver\n");
        printf ("    File Subtype:      0x%08X", ffi.dwFileSubtype);
        break;
    case VFT_FONT:
        printf ("Font\n");
        printf ("    File Subtype:      0x%08X", ffi.dwFileSubtype);
        break;
    case VFT_VXD:
        printf ("Virtual Device\n");
        printf ("    File Subtype:      0x%08X", ffi.dwFileSubtype);
        break;
    case VFT_UNKNOWN:
    default:
        printf ("Unknown (0x%08X)", ffi.dwFileType);
        break;
    }
    printf ("\n");

    printf ("    Date:              0x%08X%08X\n",
            ffi.dwFileDateMS, ffi.dwFileDateLS);
}

/** @brief Internal function for loading a function from a DLL.
 ** @param func_name The exported symbol name to load.
 ** @throws null_pointer_exception If func_name is NULL or the library is not
 **         loaded.
 ** @throws dll_load_exception If there was a problem either loading the DLL
 **         or getting the function's address from that DLL.
 **/
FARPROC psystem::runtime_library::getProcAddress (const char *func_name) const
    throw (psystem::exception::null_pointer_exception,
           psystem::exception::dll_load_exception)
{
    if ((NULL == func_name) || (NULL == m_Library))
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    FARPROC fn = GetProcAddress (m_Library, func_name);
    try
    {
        if (NULL == fn)
        {
            THROW_WINDOWS_EXCEPTION_F(
                GetLastError(), "Cannot find \"%s\" in DLL \"%s\"",
                func_name, m_LibName.c_str ());
        }
    }
    catch (psystem::exception::windows_exception& wex)
    {
        throw psystem::exception::dll_load_exception (
            wex.source_file (), wex.function (), wex.line_number (),
            wex.what ());
    }

    return fn;
}
