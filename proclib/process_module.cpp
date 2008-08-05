
/** @file process_module.cpp
 ** @brief Define objects needed to manage mapped modules in a running process.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.0
 ** @version Proclib 1.3
 **/

#include <windows.h>
#include <psapi.hpp>

#include <map>

#include <dbghelp.hpp>
#include <process_trace.hpp>

#include <exception/null_pointer_exception.hpp>
#include <exception/windows_exception.hpp>

#include "process_module.hpp"

using namespace psystem;

/** @todo remove this hack!! */
bool g_ImageSymbolsHack = false;

/** @brief Used to cache the file name that belongs to a process.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.0
 ** @version Proclib 1.3
 ** @todo This shouldn't be used any longer.
 ** @deprecated Boo!
 **/
static struct cached_procfile
{
    /** The code counts on the process handle startin with NULL.  This is
     ** how we denote that no file name has been cached within.
     **/
    explicit cached_procfile () throw ()
        : process_handle (NULL)
    {
        // Gigitty goo
    }

    std::string         file_name;
    proclib::hProcess_t process_handle;  /** @todo Make ALL handles references */
} s_ProcessFileName;

/** @todo Create an IOptions interface so this code can be used in multiple apps?
 **/

/** @todo What if HMODULE is a different size than HANDLE.  Perhaps handles
 **       should be wrapped in a class.*/

//////////////////////////////////////////////////////////////////////////////
// Public Interface
//////////////////////////////////////////////////////////////////////////////

proclib::process_module *proclib::process_module::Create (
    const proclib::process_trace& process, const memAddress_t moduleBase,
    DWORD length /*=0*/)
{
    proclib::process_module *ret = new process_module (
        process, moduleBase, length);

    if (NULL == ret) throw std::bad_alloc ();

    return ret;
}

/** @todo Do we even use this function any more?? */
const std::string& proclib::process_module::GetProcessFileName (
    const hProcess_t& hProcess)
    throw (psystem::exception::null_pointer_exception,
           psystem::exception::windows_exception)
{
    // Fuck off with your NULL handles
    if ((INVALID_HANDLE_VALUE == hProcess) || (NULL == hProcess))
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    // We've got it cached
    if (hProcess == s_ProcessFileName.process_handle)
    {
        return s_ProcessFileName.file_name;
    }

    /** @todo CHECK ALL ALLOCATIONS FOR SUCCESS!!! */
    char *moduleName = new char [MAX_PATH];
    if (NULL == moduleName)
    {
        throw std::bad_alloc ();
    }

    if (!psystem::psapi::GetModuleFileNameExA (
            hProcess, NULL, /* <- We want the process' module specifically */
            moduleName, MAX_PATH * sizeof (*moduleName)))
    {
        delete [] moduleName;
        THROW_WINDOWS_EXCEPTION(GetLastError (), "Cannot get module file name");
    }

    s_ProcessFileName.process_handle = hProcess;
    s_ProcessFileName.file_name      = moduleName;

    delete [] moduleName;
    return s_ProcessFileName.file_name;
}

//////////////////////////////////////////////////////////////////////////////
// Construction / Destruction
//////////////////////////////////////////////////////////////////////////////

proclib::process_module::process_module (
    const proclib::process_trace& process,
    const memAddress_t            moduleBase,
    DWORD                         /* length*/)
    : exec_file_module (NULL, moduleBase),
      m_ModuleSize    (0),
      m_ParentProcess (process)
{
    if ((NULL == getModuleBase ()) || (INVALID_MODULE_BASE == getModuleBase ()))
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    /** @todo Make this lazy!! */
    init_or_throw ();
}

//////////////////////////////////////////////////////////////////////////////
// Inherited Object Interface
//////////////////////////////////////////////////////////////////////////////

void proclib::process_module::dump_object () throw ()
{
    printf ("MODULE (0x%08I64X):\n", this);
    printf ("\tm_ModuleName:    %s\n",        m_ModuleName.c_str ());
    printf ("\tm_ModuleFile:    %s\n",        getModuleFile().c_str ());
    printf ("\tm_ModuleBase:    0x%08I64X\n", getModuleBase ());
    printf ("\tm_ModuleSize:    %d\n",        m_ModuleSize);
    printf ("\tm_MsvcGenerated: %s\n",        msvcModule() ? "true" : "false");
    printf ("\thProcess:        0x%08I64X\n",
            m_ParentProcess.getProcessHandle());

    IMAGEHLP_MODULE64 mod;
    memset (&mod, 0, sizeof (mod));
    mod.SizeOfStruct = sizeof (IMAGEHLP_MODULE64);

    /** @todo Hello, my interface sucks.  For one thing, I need an accessor for
     **       m_ParentProcess. */

    if (psystem::dbghelp::SymGetModuleInfo64 (
            m_ParentProcess.getProcessHandle(),
            getModuleBase (),
            &mod))
    {
        printf ("\t-- MODULE INFO --\n", mod.ModuleName);
        printf ("\tImage address:   0x%08I64X - size: %d bytes\n", mod.BaseOfImage,
                mod.ImageSize);
        printf ("\tSymbol type:     ");
        switch (mod.SymType)
        {
        case SymCoff:
            printf ("COFF Symbols");
            break;
        case SymCv:
            printf ("CodeView Symbols");
            break;
        case SymDeferred:
            printf ("Symbol Loading Deferred");
            break;
        case SymDia:
            printf ("DIA Symbols");
            break;
        case SymExport:
            printf ("DLL Export Symbols");
            break;
        case SymNone:
            printf ("No Symbols Loaded");
            break;
        case SymPdb:
            printf ("PDB Symbols");
            break;
        case SymSym:
            printf (".SYM File");
            break;
        case SymVirtual:
            printf ("Virtual");
            break;
        }
        printf ("\n");

        printf ("\tSymbol count:    %d\n", mod.NumSyms);
        printf ("\tType Info:       %s\n", mod.TypeInfo ? "true" : "false");
        printf ("\tGlobal Symbols:  %s\n", mod.GlobalSymbols ? "true" : "false");
        printf ("\tLine Numbers:    %s\n", mod.LineNumbers ? "true" : "false");
        printf ("\tImage name:      \"%s\"\n", mod.ImageName);
        printf ("\tModule file:     \"%s\"\n", mod.LoadedImageName);
        printf ("\tPDB file:        \"%s\"\n", mod.LoadedPdbName);
    }
    else
    {
        printf (
            "Can't dump process_module 0x%08I64X -- %s\n",
            this, psystem::exception::windows_exception::GetWindowsError (
                GetLastError ()).c_str());
    }

    fflush (stdout);
}

void proclib::process_module::init ()
    throw (psystem::exception::windows_exception)
{
    /** @todo CHECK ERRORS! */

    MEMORY_BASIC_INFORMATION mbi;

    memset (&mbi, 0, sizeof (mbi));

    if (!VirtualQueryEx (m_ParentProcess.getProcessHandle (), (void *)getModuleBase (), &mbi, sizeof (mbi)))
    {
        THROW_WINDOWS_EXCEPTION_F(
            GetLastError (), "Cannot find module with address 0x%08X",
            (int)getModuleBase ());
    }

////////////////////////////////
    {
        char szModName[MAX_PATH];

        setHandle ((hModule_t)mbi.AllocationBase);

        if (0 != psystem::psapi::GetModuleFileNameExA (
                m_ParentProcess.getProcessHandle (), getModuleHandle (), szModName, sizeof (szModName)))
        {
            setModuleFile (szModName);
        }
        else
        {
//            THROW_WINDOWS_EXCEPTION(GetLastError (), "Cannot get module file");
        }

        if (0 != psystem::psapi::GetModuleBaseNameA (
                m_ParentProcess.getProcessHandle (), getModuleHandle (), szModName, sizeof (szModName)))
        {
            m_ModuleName = szModName;
        }
        else
        {
//            THROW_WINDOWS_EXCEPTION(GetLastError (), "Cannot get module name");
        }
    }
////////////////////////////////

    if (0 == psystem::dbghelp::SymLoadModule64 (
            m_ParentProcess.getProcessHandle (),
            NULL, // hFile
            getModuleFile().c_str(),
            m_ModuleName.c_str(), // Module name
            getModuleBase (),
            m_ModuleSize))
    {
        // If GetLastError = 0, then we've already loaded the module - no biggie
        if (0 != GetLastError ())
        {
            THROW_WINDOWS_EXCEPTION_F(
                GetLastError(), "Cannot load library \"%s\"",
                m_ModuleName.c_str());
        }
    }

    try
    {
        if (g_ImageSymbolsHack) exec_file_module::init ();
    }
    catch (...)
    {
        /** @todo Ignoring all exceptions from not being able to parse the exec
         **       file.  Is that right? */
    }
}
