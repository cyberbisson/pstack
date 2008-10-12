
/** @file runtime_library.hpp
 ** @brief Declare the interface for explicitly managed shared-libraries.
 **
 ** @author  Matt Bisson
 ** @date    14 July, 2008
 ** @since   Proclib 1.3
 ** @version Proclib 1.3
 **/

#ifndef __RUNTIME_LIBRARY_HPP__
#define __RUNTIME_LIBRARY_HPP__

#include "psystem_ns.hpp"
#include <string>

namespace psystem
{
    namespace exception
    {
        class dll_load_exception;
        class internal_exception;
        class null_pointer_exception;
        class windows_exception;
    }

/** @brief Encapsulates a function loaded dynamically from a shared library.
 **
 ** The intended use for this class is to create an overloaded operator() that
 ** matches the method signature of the underlying function, then use the object
 ** as you would normally use the loaded function.  As each function loaded from
 ** the DLL will have a specific datatype associated with it, this will make it
 ** possible to specialize the template function,
 ** runtime_library::loadFunction ()
 ** @author  Matt Bisson
 ** @date    17 July, 2008
 ** @since   Proclib 1.3
 ** @version Proclib 1.3
 **/
template<class F>
class runtime_function
{
    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////

protected:
    /** @brief Create an empty function object. */
    explicit runtime_function (FARPROC function) throw ()
        : m_Function ((F)function)
    {
        // I am so awesome, it's ridiculous.
    }

public:
    /** @brief Let's destroy! */
    virtual ~runtime_function () throw ()
    {
        // Weeeeeeeeee!
    }

public:
    //////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    //////////////////////////////////////////////////////////////////////////

    /** @brief Describes the datatype for the function being wrapped. */
    typedef F function_signature;

    /** @brief Alters the isLoaded() state to false. */
    inline void clear () throw ()
    {
        m_Function = NULL;
    }

    /** @brief Return a callable function pointer from the DLL.
     ** @return It is possible for this function to return NULL.
     **/
    inline F getNativeFunction () const throw ()
    {
        return m_Function;
    }

    /** @brief Returns true of the function has been set. */
    inline bool isLoaded () const throw ()
    {
        return (NULL != m_Function);
    }

protected:
    //////////////////////////////////////////////////////////////////////////
    /// @name Derived-class Interface
    //////////////////////////////////////////////////////////////////////////

    /** @brief Simple mutator for the underlying DLL function. */
    inline void setNativeFunction (FARPROC function) throw ()
    {
        m_Function = (F)function;
    }

private:
    //////////////////////////////////////////////////////////////////////////
    /// @name Private Member Data
    //////////////////////////////////////////////////////////////////////////

    /** @brief The callable function lives in here.  It may be NULL, though. */
    F m_Function;
};

/** @brief A simple representation of file versions.
 **
 ** This is designed to be assigned to, and compared in one operation, while
 ** still making for readable code.
 ** @note Watch for endianness here!!!
 **/
typedef union
{
    /** @brief A comparable quad-byte value for versions. */
    unsigned int raw;

    struct __version
    {
#ifdef _BIG_ENDIAN
        unsigned short int major;
        unsigned short int minor;
#else
        /** @brief Major version number. */
        unsigned short int minor;

        /** @brief Minor version number. */
        unsigned short int major;
#endif
    } version;    /**< @brief A more logical representation of versions. */
} exeVersion_t;

/** @brief Basic interface for loading and using DLLs.
 **
 ** @note loadFunction with no parameters is not implemented because C++ is odd.
 ** @author  Matt Bisson
 ** @date    14 July, 2008
 ** @since   Proclib 1.3
 ** @version Proclib 1.3
 **/
class runtime_library
{
public:
    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////

    explicit runtime_library (const char *libname, unsigned int version = 0)
        throw (psystem::exception::null_pointer_exception);
    explicit runtime_library (
        const char         *libname,
        unsigned short int  major_version,
        unsigned short int  minor_version)
        throw (psystem::exception::null_pointer_exception);
    virtual ~runtime_library () throw ();

    //////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    //////////////////////////////////////////////////////////////////////////

    /** @brief Automatically load the correct function based on type.
     ** @return This returns a function object of type T.  T is expected to be
     **         derived from psystem::runtime_function.
     ** @throws dll_load_exception If there was a problem either loading the DLL
     **         or getting the function's address from that DLL.
     **/
    template<class T> T loadFunction ()
        throw (psystem::exception::dll_load_exception);

    /** @brief Find the virtual address of a function (by name) and return it.
     ** @param func_name The exported symbol name to load.
     ** @return This returns a function of type T.  It should never return NULL.
     ** @throws null_pointer_exception If func_name is NULL.
     ** @throws dll_load_exception If there was a problem either loading the DLL
     **         or getting the function's address from that DLL.
     ** @deprecated This second mechanism for loading functions should not be
     **     used, as it requires both T and func_name to be given.  T must be a
     **     function type, etc, etc., so it's very prone to error.
     **/
    template<class T> T loadFunction (const char *func_name)
        throw (psystem::exception::null_pointer_exception,
               psystem::exception::dll_load_exception)
    {
        loadLibrary ();

        return (T)getProcAddress (func_name);
    }

    void loadLibrary   () throw (psystem::exception::dll_load_exception);
    void unloadLibrary () throw (psystem::exception::windows_exception);

protected:
    //////////////////////////////////////////////////////////////////////////
    /// @name Protected Member Functions
    //////////////////////////////////////////////////////////////////////////

    void checkVersionInfo ()
        throw (psystem::exception::internal_exception,
               psystem::exception::windows_exception);
    void dumpVersionInfo  (const VS_FIXEDFILEINFO& ffi) const throw ();

    FARPROC getProcAddress (const char *func_name) const
        throw (psystem::exception::null_pointer_exception,
               psystem::exception::dll_load_exception);

    /** @brief Callback to wipe function pointers on library unload.
     **
     ** Those implementing runtime_library must remember to clear out all their
     ** functions if the library gets unloaded.  This is their chance!
     **/
    virtual void unloadFunctions () = 0;

private:
    //////////////////////////////////////////////////////////////////////////
    /// @name Private Member Data
    //////////////////////////////////////////////////////////////////////////

    /** @brief Filename for the library (used to load). */
    std::string m_LibName;

    /** If this variable is NULL, the library is not loaded, and any calls to
     ** loadLibrary() will try to load the library again.  If it is not NULL,
     ** loadLibrary() has no effect.  To reload the library, first unload it.
     ** @brief Handle to the loaded module for use in API functions.
     **/
    HMODULE m_Library;

    /** @brief Minimum version of this library that is OK for the caller. */
    exeVersion_t m_RequiredVersion;
};

//////////////////////////////////////////////////////////////////////////////
/// @name Convenience Macros
//////////////////////////////////////////////////////////////////////////////

/** @brief Make a template specialization for
 **        psystem::runtime_library::loadFunction()
 **
 ** This function actually builds the body of a template specialization, given
 ** a specific function type.  It assumes that the template should use a class
 ** that can be constructed with a single FARPROC argument.  This class is
 ** expected to belong in the psystem namespace, and have a class name in the
 ** form of fn##F.  If this doesn't work for you, you're on your own!
 ** @param F The name of the Win32 function that lives in the DLL.
 ** @relates psystem::runtime_library
 **/
#define MAKE_LOAD_FUNCTION(F) \
    template<>                                                          \
    psystem::fn##F psystem::runtime_library::loadFunction ()            \
        throw (psystem::exception::dll_load_exception)                  \
    {                                                                   \
        loadLibrary ();                                                 \
                                                                        \
        try                                                             \
        {                                                               \
            psystem::fn##F newFn =                                      \
                GetProcAddress (m_Library, #F);                         \
            if (!newFn.isLoaded ())                                     \
            {                                                           \
                THROW_WINDOWS_EXCEPTION_F(                              \
                    GetLastError(), "Cannot find \"%s\" in DLL \"%s\"", \
                    #F, m_LibName.c_str ());                            \
            }                                                           \
                                                                        \
            return newFn;                                               \
        }                                                               \
        catch (psystem::exception::windows_exception& wex)              \
        {                                                               \
            throw psystem::exception::dll_load_exception (              \
                wex.source_file (), wex.function (), wex.line_number (), \
                wex.what ());                                           \
        }                                                               \
    }

}

#endif // __RUNTIME_LIBRARY_HPP__
