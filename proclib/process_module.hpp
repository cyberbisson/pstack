
/** @file process_module.hpp
 ** @brief Declare objects needed to manage mapped modules in a running process.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.0
 ** @version Proclib 1.3
 **/

#ifndef __PROCESS_MODULE_HPP__
#define __PROCESS_MODULE_HPP__

#include <windows.h>
#include <map>
#include <string>

#include "proclib_ns.hpp"

#include "exec_file_module.hpp"

namespace psystem
{
    namespace exception
    {
        class null_pointer_exception;
        class windows_exception;
    }
}
namespace proclib
{
    class process_trace;

/** The important destinction netweem process_module, and it's base,
 ** exec_file_module is that process_module is examining a living executable.
 ** There's a few more things we can do while running.  We have both the parent
 ** process object to manipulate, as well as having an OS-level handle to the
 ** module (to send to OS-native APIs).
 **
 ** @brief Represents one (of possibly many) loaded code modules for a running
 **        process.
 **
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.0
 ** @version Proclib 1.3
 **/
class process_module : public proclib::exec_file_module
{
public:
    //////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    //////////////////////////////////////////////////////////////////////////

    static process_module *Create (
        const proclib::process_trace& process,
        const psystem::memAddress_t   moduleBase,
        DWORD                         length = 0);

    static const std::string& GetProcessFileName (const hProcess_t& hProcess)
        throw (psystem::exception::null_pointer_exception,
               psystem::exception::windows_exception);

    /** @brief Return the run-time module name (this is not necessarily the file
     **        name, but commonly is). */
    inline const std::string& getModuleName () const throw ()
    {
        return m_ModuleName;
    }

    /** @brief Return's a reference to the process that loaded us. */
    inline const proclib::process_trace& getParentProcess () const throw ()
    {
        return m_ParentProcess;
    }

    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////
protected:
    explicit process_module (
        const proclib::process_trace& process,
        const psystem::memAddress_t   moduleBase,
        DWORD                         length);

    //////////////////////////////////////////////////////////////////////////
    /// @name Inherited Object Interface
    //////////////////////////////////////////////////////////////////////////
public:
    virtual void dump_object () throw ();
protected:
    virtual void init        () throw (psystem::exception::windows_exception);

    /** @todo Shutdown is not implemented... */
    virtual void shutdown    () { };

    /** @brief Pass along the OS-native module handle. */
    inline const hModule_t getModuleHandle () const throw ()
    {
        return (hModule_t)getHandle ();
    }

private:
    //////////////////////////////////////////////////////////////////////////
    /// @name Private Member Data
    //////////////////////////////////////////////////////////////////////////

    /** @brief The run-time module name (this is not necessarily the file name,
               but commonly is). */
    std::string          m_ModuleName;

    /** @brief The size of this module after being loaded into memory. */
    DWORD                m_ModuleSize;

    /** @note This object CANNOT die after it's parent process.  If it does, we
     **       will have this dangly pointer hanging around.
     ** @brief Reference back to our module's parent.
     **/
    const proclib::process_trace& m_ParentProcess;

    /// @}
    /** @brief Copying this object is not allowed. */
    process_module& operator= (const process_module&) throw ()
    {
        return *this;
    }
};

}

#endif // __PROCESS_MODULE_HPP__
