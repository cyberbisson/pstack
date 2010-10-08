
/** @file trace_object.hpp
 ** @brief Declare the infrastructure for stack trace objects.
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.0
 ** @version Proclib 1.3
 **/

#ifndef __TRACE_OBJECT_HPP__
#define __TRACE_OBJECT_HPP__

#include <windows.h>
#include <exception>

#include "proclib_ns.hpp"

namespace psystem
{
    namespace exception
    {
        class internal_exception;
        class windows_exception;
    }
}
namespace proclib
{
/** The idea of this object is that all trace objects can be implemented lazily,
 ** as this base object takes care of initializing and destroying.  Any object
 ** that exposes it's constructor or does not call guaranetee a call to init()
 ** should make sure to call init_or_throw() in all public methods.  This will
 ** check that the object is valid and if not, it will try to populate the member
 ** data.
 **
 ** @brief This is a base interface for all objects needed for a stack trace.
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.0
 ** @version Proclib 1.3
 **/
class trace_object
{
public:
    //////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    //////////////////////////////////////////////////////////////////////////

    /** Every subclass should implement this function to aid in debugging or even
     ** to give interesting information to the user.
     ** @brief Output data to stdout.
     **/
    virtual void dump_object () throw () = 0;

    trace_object *destroy          () throw (std::exception);
    trace_object *force_destroy    () throw ();
    void          init_or_throw    () throw (std::exception);
    void          reuse            () throw (std::exception);

protected:
    //////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    //////////////////////////////////////////////////////////////////////////

    explicit trace_object (HANDLE hObject) throw ();
    virtual ~trace_object ()
        throw (psystem::exception::internal_exception);

    //////////////////////////////////////////////////////////////////////////
    /// @name Inheritable Object Interface
    //////////////////////////////////////////////////////////////////////////

    /** @note <i>Do not</i> call init() explicitly.  Doing so may cause the
     **       init_or_throw mechanism to be short-circuited.
     ** @brief Readies the object with anything that can be done lazily.
     **
     ** @throws std::exception Derived classes may throw anything derived from
     **                        std::exception. */
    virtual void init () throw (std::exception) = 0;

    /** @brief This is your chance to clean up.
     **
     ** @throws std::exception Derived classes may throw anything derived from
     **                        std::exception. */
    virtual void shutdown () throw (std::exception) = 0;

    /** @brief Is this object ready to use? */
    inline bool initialized () const throw ()
    {
        return (m_Initialized == INITIALIZED);
    }

    /** @brief Access the (generic) OS handle for this object */
    inline HANDLE getHandle () const throw ()
    {
        return m_hObject;
    }

    /** @brief This provides a means for subclasses to change the OS handle. */
    inline void setHandle (HANDLE inHandle) throw ()
    {
        m_hObject = inHandle;
    }

private:
    //////////////////////////////////////////////////////////////////////////
    /// @name Private Member Data
    //////////////////////////////////////////////////////////////////////////

    /** @brief Defines a simple state-machine for object initialization */
    typedef enum
    {
        /** @brief No successful initialization has occurred. */
        UNINITIALIZED = 0,

        /** @brief Currently running within the context of init(). */
        INITIALIZING,

        /** @brief The object is ready for use. */
        INITIALIZED
    } initState_t;

    /** @brief This OS handle must be set for the object to be considered
     **        "valid." */
    HANDLE m_hObject;

    /** @brief Has this object successfully initialized itself? */
    initState_t m_Initialized;

    /// @}
};

}

#endif // __TRACE_OBJECT_HPP__
