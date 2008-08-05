
/** @file trace_object.cpp
 ** @brief Define the infrastructure for stack trace objects.
 ** @author  Matt Bisson
 ** @date    25 November, 2007
 ** @since   Proclib 1.0
 ** @version Proclib 1.3
 **/

#include <windows.h>
#include <exception>

#include <exception/internal_exception.hpp>

#include "trace_object.hpp"

//////////////////////////////////////////////////////////////////////////////
// Public interface:
//////////////////////////////////////////////////////////////////////////////

/** @brief Shutdown and delete memory for trace_object.
 **
 ** Call this rather than delete.  This ensures that the virtual functions for
 ** tearing down the object are called.
 ** @return This always returns NULL (for convenience of destroying pointers).
 ** @throws std::exception This throws whatever the derived shutdown() method
 **         ultimately throws.
 **/
proclib::trace_object *proclib::trace_object::destroy ()
    throw (std::exception)
{
    if (initialized ())
    {
        m_Initialized = UNINITIALIZED;
        shutdown ();
    }

    delete this;
    return NULL;
}

/** @brief Call the init() method if the object has not successully initialized.
 **
 ** This object may throw or not, depending on how base classes implement their
 ** init() method.  We'll pass any exceptions along, starting with the base
 ** exception.
 **
 ** It is safe to call init_or_throw() within the context of init().
 **/
void proclib::trace_object::init_or_throw ()
    throw (std::exception)
{
    if (UNINITIALIZED == m_Initialized)
    {
        m_Initialized = INITIALIZING;
        init ();
        m_Initialized = INITIALIZED;
    }
}

/** @brief Shutdown the object without deleting the memory.
 **
 ** This function shuts down the object and makes sure that any calls into
 ** init_or_throw() or init() will reinitialize it.
 ** @throws std::exception This throws whatever the derived shutdown() method
 **         ultimately throws.
 **/
void proclib::trace_object::reuse ()
    throw (std::exception)
{
    if (initialized ())
    {
        m_Initialized = UNINITIALIZED;
        shutdown ();
    }
}

//////////////////////////////////////////////////////////////////////////////
// Construction / destruction:
//////////////////////////////////////////////////////////////////////////////

/** @brief Set up the trace_object.
 **
 ** @param hObject This can be invalid during construction, but should not be
 **                when the object is used.
 **/
proclib::trace_object::trace_object (HANDLE hObject)
    throw ()
    : m_hObject (hObject),
      m_Initialized (UNINITIALIZED)
{
    // Gigitty.
}

/** @brief Destroy the trace_object.
 **
 ** This does not call the destroy() method.  If destroy() has not been called
 ** before this point, the destructor will throw an exception.  This is because
 ** the vtable will have been destroyed by the time we get here, so calling
 ** destroy() will have unknown results.
 ** @throw internal_exception If destroy() was not called before destruction.
 **/
proclib::trace_object::~trace_object ()
    throw (psystem::exception::internal_exception)
{
    if (initialized ())
    {
        // Well, I'd print out the name of the object that we're blowing away,
        // but all but trace_object's vtable will be destroyed by now...
        throw psystem::exception::internal_exception (
            __FILE__, __FUNCTION__, __LINE__, "Destroyed object improperly");
    }
}
