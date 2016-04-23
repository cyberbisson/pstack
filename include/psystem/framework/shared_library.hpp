// ===-- include/psystem/framwork/shared_library.hpp ------ -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Define a base type to perform on-demand loading of shared libraries.
 *
 * @author  Matt Bisson
 * @date    19 November, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#pragma once
#ifndef PSYSTEM_FRAMEWORK_SHARED_LIBRARY_HPP
#define PSYSTEM_FRAMEWORK_SHARED_LIBRARY_HPP

#include "platform.hpp"

#include <algorithm>
#include <type_traits>
#include <vector>

#include "not_copyable.hpp"
#include "shared_handle.hpp"
#include "type_traits.hpp"

namespace psystem {

/**
 * @brief Load shared libraries into the current process for execution.
 *
 * This class loads shared libraries (SO) and dynamic link libraries (DLL) into
 * the current process so that automatic binding does not occur.  This is
 * interesting because we may wish to check the version of a library before
 * using it (a strange error occurs when symbols are not found), or perhaps we
 * may fall back to another bit of functionality if the libraries to support the
 * preferred aproach are not present on the system.
 *
 * This class should be a base class, and specific DLLs and SOs should be
 * encapsulated by their own derived class.
 *
 * @note Copy and move construction of this class is certainly feasible, but not
 * needed, so for now this functionality does not exist here.
 *
 * @author  Matt Bisson
 * @date    19 November, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
class shared_library
    : public psystem::not_copyable
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Type Declarations
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Type for shared library versions.
     *
     * When viewed as an integer value, the 16 most significant bits represent a
     * major version, while the lowest 16 represent a minor version.  For
     * example, a version of 4.15 would manifest as @c 0x00040015.
     */
    using library_version = uint32_t;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
protected:
    /**
     * @brief Construct a shared library instance (without loading the module).
     *
     * @param[in] library_name
     *     The name of the module to load.  This must follow whatever the
     *     general rules for the system are (e.g., @c LoadLibrary for Windows).
     * @param[in] num_functions
     *     The absolute number of functions to import from the shared library.
     *     This creates the size of the initial cache of function pointers.
     */
    explicit shared_library(
        std::string library_name, size_t num_functions) noexcept;
public:
    /**
     * @brief Clean up the shared library.
     *
     * This will decrease the reference count on the shared library, possibly
     * causing the system to unload it from the process space.  All function
     * pointers acquired from this instance may become invalidated.
     */
    ~shared_library() noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Access the version for the library that this instance wraps.
     *
     * @return The associated library version information relevant to this
     *         instance.
     *
     * @throws psystem::exception::system_exception
     *     if the library specified for this instance is not valid, or if there
     *     was a problem gathering the version information.
     */
    library_version get_version() const;

    /**
     * @brief Determines if the shared library is mapped into the process
     *        space.
     *
     * @return
     *     @c true if the shared library is mapped into the current process
     *     space, and the handle is contained within this instance.  This
     *     returns @c false, otherwise.
     */
    bool is_loaded() const noexcept;

    /**
     * @brief Loads the requested library into the current process space.
     *
     * No function pointers will be acquired as a result of this call.
     *
     * @pre @c is_loaded() == @c false
     * @post @c is_loaded() == @c true
     *
     * @throws psystem::exception::system_exception
     *     if the library could not be loaded.
     */
    void load();

    /**
     * @brief Unload this library and clear the function pointers.
     * @post @c is_loaded() == @c false
     */
    void unload() noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Inherited Interface
    ////////////////////////////////////////////////////////////////////////////
protected:
    /**
     * @brief Acquire a function pointer (by name) from a loaded library.
     *
     * @pre @c is_loaded() == @c true
     *
     * @param[in] export_name The name of the exported symbol.
     * @return The pointer to the function acquired from the library.
     *
     * @throws psystem::exception::system_exception
     *     if the specified symbol could not be found in the loaded shared
     *     library.
     */
    FARPROC get_function(char const *export_name) const;

    /**
     * @brief Fully populate the @c m_functions data with addresses.
     *
     * This function completely replaces the contents of the @c
     * m_functions member data.  You must pass a simply array of
     * C-style strings, which will then be used to load the function
     * addresses.
     *
     * @pre @c is_loaded() == @c true
     *
     * @tparam N
     *     The number of elements in the @p export_names parameter.
     *     This must exactly match the number of elements in @c
     *     m_functions.

     * @param[in] export_names
     *     The names taken from the shared library's export table that
     *     will be used to gather addresses from the loaded module.
     */
    template <size_t N>
    void load_all_functions(char const *export_names[N])
    {
        ASSERT(is_loaded());
        ASSERT(N == m_functions.size());

        std::transform(
            export_names, export_names + N, // Input = export_names
            m_functions.begin(),            // Output = m_functions
            [this] (char const *export_name) {
                return get_function(export_name);
            });
    }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
protected:
    /// @brief The cache of function pointers gathered from a loaded library.
    std::vector<FARPROC> m_functions;

    /// @brief The system handle to the loaded library.  It may be @c nullptr.
    psystem::shared_handle<HMODULE, nullptr> m_library_handle;

    /// @brief The (passed-in) name of the library that this class represents.
    std::string m_library_name;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Internal Class Declarations
    ////////////////////////////////////////////////////////////////////////////
private:
    /**
     * @brief Common base class for all specializations of loaded_function.
     *
     * @tparam T The function signature that will be used to store the wrapped
     *           function.
     *
     * @author  Matt Bisson
     * @date    19 November, 2015
     * @since   PSystem 2.0
     * @version PSystem 2.0
     */
    template <typename T>
    struct loaded_function_base
    {
        static_assert(
            std::is_function<T>::value,
            "Template argument must be a function pointer.");

        /**
         * @brief Construct a loaded_function from a function pointer.
         *
         * @param[in] proc The proceedure's address.  Type safety is out the
         *                 window; we must trust you.  This may be @c nullptr.
         */
        loaded_function_base(FARPROC proc)
            : m_function(reinterpret_cast<T*>(proc))
        {
        }

        /**
         * @brief Re-assign a function pointer to this functor instance.
         *
         * Not all users of loaded_function will know the memory address at
         * construction time (think "lazy" initialization).  This method allows
         * them to assign the pointer at a later time.
         *
         * @param[in] proc The proceedure's address.  Type safety is out the
         *                 window; we must trust you.  This may be @c nullptr.
         */
        void retarget(FARPROC proc) noexcept
        {
            m_function = reinterpret_cast<T*>(proc);
        }

        /**
         * @brief Access the function pointer contained here directly.
         * @return This may return @c nullptr.
         */
        T *get_ptr() const noexcept
        {
            return m_function;
        }

    protected:
        /// @brief The function pointer that may be invoked.
        T *m_function;
    };

protected:
    /**
     * @brief Encapsulate a loaded function's address into a callable functor.
     *
     * This functor automagically determines the return type of the function
     * passed to it, and returns the data accordingly.  Also, it uses "perfect
     * forwarding" to hand all arguments from the caller to the underlying
     * loaded function.
     *
     * @note This work can easily be done with @c std::function, but this
     * implementation takes much less space, and @c function may even introduce
     * a vtable, which is far too heavy.
     *
     * @tparam T
     *     The function signature that will be used to store the wrapped
     *     function.
     * @tparam is_void_ref
     *     Used to determine if the loaded_function instance should provide a
     *     function with a @c void return type.  Unless you wrap a non-void
     *     function, and explicitly want to force it into a @c void return type,
     *     do not specify this.  The metafunction, @c has_void_return_type will
     *     determine the correct return automatically.
     *
     * @author  Matt Bisson
     * @date    19 November, 2015
     * @since   PSystem 2.0
     * @version PSystem 2.0
     */
    template <typename T, typename is_void_ref = has_void_return_type<T>>
    struct loaded_function
        : public loaded_function_base<T>
    {
        /**
         * @brief Construct a loaded_function from a function pointer.
         *
         * @param[in] proc The proceedure's address.  Type safety is out the
         *                 window; we must trust you.  This may be @c nullptr.
         *
         * @todo I would rather inherit the CTOR with "using base::base", but
         *       MSVC apparently does not support this.
         */
        loaded_function(FARPROC proc) : loaded_function_base<T>(proc) {}

        /**
         * @brief Invoke the library function.
         *
         * @tparam Args
         *     The argument pack should match the declared function signature.
         * @param[in] args
         *     Zero or more arguments to be forwarded to the underlying
         *     function.  Check the actual function signature and documentation
         *     for details.
         * @return
         *     Whatever the underlying function returns.
         * @throws
         *     Whatever the underlying function throws.
         *
         * @todo Determine the @c noexcept value when MSVC supports it.
         */
        template <typename... Args>
        auto operator()(Args&&... args)
            // this->m_function is protected, so using it for the return type of
            // a public function will be trouble.  Instead, we can just use T,
            // and make an assumption that this->m_function is ultimately a T
            // type.
            -> decltype(std::declval<T>()(std::forward<Args>(args)...))
        {
            ASSERT(this->m_function);
            return (*this->m_function)(std::forward<Args>(args)...);
        }
    };

    /**
     * @brief Provide a specialization for loaded_function to handle a @c void
     *        return type.
     *
     * @tparam T
     *     The function signature that will be used to store the wrapped
     *     function.
     *
     * @author  Matt Bisson
     * @date    19 November, 2015
     * @since   PSystem 2.0
     * @version PSystem 2.0
     */
    template <typename T>
    struct loaded_function<T, std::true_type>
        : public loaded_function_base<T>
    {
        /**
         * @brief Construct a loaded_function from a function pointer.
         *
         * @param[in] proc The proceedure's address.  Type safety is out the
         *                 window; we must trust you.  This may be @c nullptr.
         *
         * @todo I would rather inherit the CTOR with "using base::base", but
         *       MSVC apparently does not support this.
         */
        loaded_function(FARPROC proc) : loaded_function_base<T>(proc) {}

        /**
         * @brief Invoke the library function.
         *
         * @tparam Args
         *     The argument pack should match the declared function signature.
         * @param[in] args
         *     Zero or more arguments to be forwarded to the underlying
         *     function.  Check the actual function signature and documentation
         *     for details.
         * @throws
         *     Whatever the underlying function throws.
         *
         * @todo Determine the @c noexcept value when MSVC supports it.
         */
        template <typename... Args>
        void operator()(Args&&... args)
        {
            ASSERT(this->m_function);
            this->m_function(std::forward<Args>(args)...);
        }
    };
};

} // namespace psystem

#endif // PSYSTEM_FRAMEWORK_SHARED_LIBRARY_HPP
