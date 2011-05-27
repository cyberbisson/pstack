
/** @file exec_file_module.cpp
 ** @brief Define the logical objects needed to examine a thread.
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.3
 ** @version Proclib 1.3
 **/

#include <windows.h>

#include <not_found_exception.hpp>

#include <exception/null_pointer_exception.hpp>
#include <exception/windows_exception.hpp>

#include "exec_file_module.hpp"

using namespace psystem;
using proclib::exec_file_module;

//////////////////////////////////////////////////////////////////////////////
// Public interface:
//////////////////////////////////////////////////////////////////////////////

exec_file_module *proclib::exec_file_module::Create (
    const char         *fileName,
    const memAddress_t  moduleBase /*=INVALID_MODULE_BASE*/)
    throw (std::bad_alloc)
{
    proclib::exec_file_module *ret = new exec_file_module (fileName, moduleBase);

    if (NULL == ret) throw std::bad_alloc ();

    return ret;
}

proclib::symbol_t proclib::exec_file_module::getSymbol (
    const memAddress_t& location) const
{
    /** @todo This should init() */
//    init_or_throw ();

    functionAddressMap_t::const_iterator it =
        m_FunctionMap.lower_bound (location);

    if (m_FunctionMap.end () != it)
    {
        symbol_t symbol;

        symbol.name    = it->second.c_str ();
        symbol.address = it->first;
        return symbol;
    }

    throw proclib::not_found_exception (__FILE__, __FUNCTION__, __LINE__);
}

proclib::symbol_t proclib::exec_file_module::putSymbol (const symbol_t& symbol)
{
    /** Bad exceptions. */
    if ((NULL == symbol.address) || (NULL == symbol.name)) throw "Illegal args";

    init_or_throw ();

    m_FunctionMap[symbol.address] = symbol.name;
    return getSymbol (symbol.address);
}

//////////////////////////////////////////////////////////////////////////////
// Construction / destruction:
//////////////////////////////////////////////////////////////////////////////

proclib::exec_file_module::exec_file_module (
    const char *fileName, const memAddress_t moduleBase)
    throw ()
    : trace_object (INVALID_HANDLE_VALUE),
      m_ModuleBase    (moduleBase),
      m_ModuleFile    ((NULL != fileName) ? fileName : ""),
      m_MsvcGenerated (true)
{

}

//////////////////////////////////////////////////////////////////////////////
// Inherited object interface:
//////////////////////////////////////////////////////////////////////////////

void proclib::exec_file_module::dump_object () throw ()
{
    printf ("EXEC-MODULE (0x%08I64X):\n", this);
    printf ("\tm_ModuleFile:    %s\n",        m_ModuleFile.c_str ());
    printf ("\tm_ModuleBase:    0x%08I64X\n", m_ModuleBase);
    printf ("\tm_MsvcGenerated: %s\n",
            m_MsvcGenerated ? "true" : "false");

    fflush (stdout);
}

void proclib::exec_file_module::init ()
{
    if (m_ModuleFile.empty ())
    {
        /** @todo Better exception... */
        throw "!  This is empty!!";
    }

    if (0 == strcmpi (
            ".exe", m_ModuleFile.substr (m_ModuleFile.size() - 4).c_str()))
    {
        /** @todo Read from the image (if asked) and insert the symbols */
        printf ("\nReading symbols from \"%s\"\n", m_ModuleFile.c_str());

        HANDLE hFile;
        HANDLE hFileMapping;
        memAddress_t fileBase;
    
        if (INVALID_HANDLE_VALUE == (hFile = CreateFile (
                                         m_ModuleFile.c_str(), GENERIC_READ,
                                         FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                         FILE_ATTRIBUTE_NORMAL, 0)))
        {
            THROW_WINDOWS_EXCEPTION_F(
                GetLastError (), "Cannot open executable image \"%s\"",
                m_ModuleFile.c_str ());
        }

        if (NULL == (hFileMapping = CreateFileMapping (
                         hFile, NULL, PAGE_READONLY, 0, 0, NULL)))
        {
            CloseHandle (hFile);

            THROW_WINDOWS_EXCEPTION_F(
                GetLastError (), "Cannot create memory map for image -- \"%s\"",
                m_ModuleFile.c_str ())
        }

        if (NULL == (fileBase = (memAddress_t)MapViewOfFile (
                         hFileMapping, FILE_MAP_READ, 0, 0, 0)))
        {
            CloseHandle (hFileMapping);
            CloseHandle (hFile);

            THROW_WINDOWS_EXCEPTION_F(
                GetLastError (), "Cannot map \"%s\" into memory to find symbols",
                m_ModuleFile.c_str ());
        }

        try
        {
            if (INVALID_MODULE_BASE == m_ModuleBase) findModuleBase (fileBase);
            searchBytesForSymbols (fileBase);
        }
        catch (...)
        {
            UnmapViewOfFile ((void *)fileBase);
            CloseHandle (hFileMapping);
            CloseHandle (hFile);
            throw;
        }

        UnmapViewOfFile ((void *)fileBase);
        CloseHandle (hFileMapping);
        CloseHandle (hFile);
    }
}

void proclib::exec_file_module::shutdown () throw ()
{

}

//////////////////////////////////////////////////////////////////////////////
// Private utility functions:
//////////////////////////////////////////////////////////////////////////////

void proclib::exec_file_module::findModuleBase (const memAddress_t /*fileBase*/)
{
    /** @todo */
}

void proclib::exec_file_module::searchBytesForSymbols (
    const memAddress_t fileBase)
{
    if (NULL == fileBase)
    {
        throw psystem::exception::null_pointer_exception (
            __FILE__, __FUNCTION__, __LINE__);
    }

    const IMAGE_DOS_HEADER& dosHeader = *((IMAGE_DOS_HEADER *)fileBase);

    if (IMAGE_DOS_SIGNATURE == dosHeader.e_magic)
    {
        const IMAGE_NT_HEADERS& ntHeader =
            *((IMAGE_NT_HEADERS *)(fileBase + dosHeader.e_lfanew));

        if ((IMAGE_NT_SIGNATURE != ntHeader.Signature) ||
            (NULL == ntHeader.FileHeader.PointerToSymbolTable) ||
            (IMAGE_NT_OPTIONAL_HDR_MAGIC != ntHeader.OptionalHeader.Magic))
        {
            /** Bad exceptions. */
            throw "Not a Portable Executable (PE) EXE\n";
        }

        /** @note As of 2007.12.05, there is no cygwin/mingw application with
         **       a major version greater than 1. */
        m_MsvcGenerated = (1 != ntHeader.OptionalHeader.MajorImageVersion);

        const byte_t *start =
            (byte_t *)(fileBase + ntHeader.FileHeader.PointerToSymbolTable);
        const byte_t *stringTable =
            start + (ntHeader.FileHeader.NumberOfSymbols * sizeof(IMAGE_SYMBOL));

        if (INVALID_MODULE_BASE == m_ModuleBase)
        {
            throw "internal error";
        }

        const IMAGE_SYMBOL *kSymbolEnd =
            (IMAGE_SYMBOL *)start + ntHeader.FileHeader.NumberOfSymbols;
        for (IMAGE_SYMBOL *sym = (IMAGE_SYMBOL *)start; sym < kSymbolEnd; ++sym)
        {
            // We only care about functions with an address given
            if ((NULL == sym->Value) || (!ISFCN(sym->Type))) continue;

            const memAddress_t kFunctionStart =
                m_ModuleBase + ntHeader.OptionalHeader.BaseOfCode + sym->Value;

            if (sym->N.Name.Short)
            {
                char shortString[9];

                memcpy (shortString, sym->N.ShortName, 8);
                shortString[8] = '\0';

                m_FunctionMap[kFunctionStart] =
                    (!m_MsvcGenerated) ? shortString + 1: shortString;
            }
            else
            {
                m_FunctionMap[kFunctionStart] =
                    (!m_MsvcGenerated) ? 
                    (char *)(stringTable + sym->N.Name.Long) + 1:
                    (char *)(stringTable + sym->N.Name.Long);
            }
        }
    }
    else
    {
        /** Bad exceptions. */
        throw "Not a Portable Executable (PE) EXE\n";
    }
}
