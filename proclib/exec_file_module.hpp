
/** @file exec_file_module.hpp
 ** @brief Declare the logical objects needed to examine a Windows executable
 **        file.
 ** @author  Matt Bisson
 ** @date    15 May, 2008
 ** @since   Proclib 1.3
 ** @version Proclib 1.3
 **/

#ifndef __EXEC_FILE_MODULE_HPP__
#define __EXEC_FILE_MODULE_HPP__

#include <map>
#include <string>

#include "proclib_ns.hpp"
#include "trace_object.hpp"

/** @brief An address at which no module can exist. */
#define INVALID_MODULE_BASE             ((psystem::memAddress_t)(~0LL))

namespace proclib
{

/** This object can act as a lookup for functions, given their address.
 **
 ** @brief Capable of cracking a Windows executable open and reading it's
 **        symbol table.
 ** @author  Matt Bisson
 ** @date    15 May, 2008
 ** @since   Proclib 1.3
 ** @version Proclib 1.3
 **/
class exec_file_module : public proclib::trace_object
{
public:
    //////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    //////////////////////////////////////////////////////////////////////////

    static exec_file_module *Create (
        const char                  *fileName,
        const psystem::memAddress_t  moduleBase = INVALID_MODULE_BASE)
        throw (std::bad_alloc);

    symbol_t getSymbol (const psystem::memAddress_t& location) const;
    symbol_t putSymbol (const symbol_t& symbol);

    /** @brief Return the architecture against which this executable was
     **     built. */
    inline architecture_t getArchitecture () const throw ()
    {
        /** @todo This returns a hard-coded x86 response. */
        return ARCH_X86;
    }

    /** @brief Return the expected base memory address for this module's code. */
    inline const psystem::memAddress_t& getModuleBase () const throw ()
    {
        return m_ModuleBase;
    }

    /** @brief Return the file name for this module (may be empty) */
    inline const std::string& getModuleFile () const throw ()
    {
        return m_ModuleFile;
    }

    /** @brief Returns true if and only if this module was created by
     **        Microsoft's C compiler. */
    inline bool msvcModule () const throw ()
    {
        return m_MsvcGenerated;
    }

protected:
    //////////////////////////////////////////////////////////////////////////
    /// @name Protected Data Types
    //////////////////////////////////////////////////////////////////////////

    /** @brief Links a memory address to a function name (sorted descending). */
    typedef std::map <psystem::memAddress_t, std::string,
                      std::greater<psystem::memAddress_t> > functionAddressMap_t;

    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////

    explicit exec_file_module (
        const char *fileName, const psystem::memAddress_t moduleBase) throw ();

    //////////////////////////////////////////////////////////////////////////
    /// @name Inherited Object Interface
    //////////////////////////////////////////////////////////////////////////

public:
    virtual void dump_object () throw ();
protected:
    virtual void init        ();
    virtual void shutdown    () throw ();

    //////////////////////////////////////////////////////////////////////////
    /// @name Protected Interface
    //////////////////////////////////////////////////////////////////////////

    /** @brief Set the module file name.  NULL results in an empty string. */
    inline void setModuleFile (const char *inFile) throw ()
    {
        m_ModuleFile = (NULL == inFile) ? "" : inFile;
    }

private:
    //////////////////////////////////////////////////////////////////////////
    /// @name Private Member Data
    //////////////////////////////////////////////////////////////////////////

    /** @brief Cache away the architecture type for this process. */
    architecture_t              m_Architecture;

    /** @brief A searchable list of function names contained in this module. */
    functionAddressMap_t        m_FunctionMap;

    /** @brief The expected base address of this module's code segments. */
    const psystem::memAddress_t m_ModuleBase;

    /** @brief The name of the disk file for this module. */
    std::string                 m_ModuleFile;

    /** @brief Only true if the Microsoft C compiler created this module. */
    bool                        m_MsvcGenerated;

    //////////////////////////////////////////////////////////////////////////
    /// @name Private Utility Functions
    //////////////////////////////////////////////////////////////////////////

    void findModuleBase        (const psystem::memAddress_t fileBase);
    void searchBytesForSymbols (const psystem::memAddress_t fileBase);

    /** @brief Copying this object is not allowed. */
    exec_file_module& operator= (const exec_file_module&) throw ()
    {
        return *this;
    }
};

}

#endif // __EXEC_FILE_MODULE_HPP__
