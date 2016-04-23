// ===-- include/psystem/framework/unique_handle.hpp ------ -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare an RAII container for system API handles
 *        (psystem::unique_handle).
 *
 * @note
 *     There are no comparison operators for @c unique_handle to simple @c handle
 *     types for a few reasons.  First, it is difficult challenging (not
 *     impossible) to decide on a type for the unmanaged handle because handles
 *     are generally pointers or even integers that have been given another name
 *     via typedef.  They may also be forward declarations of a type as well.
 *     The second reason is that many handle types will conflict with the
 *     @c bool operator---the expression @c "unique_handle(hndl) == hndl" may
 *     first convert the @c unique_handle to a bool, then use a simple equality
 *     operator that compares POD integers, or it call the @c operator== that
 *     takes @c unique_handle and the handle type.  There is ambiguity.  We will
 *     therefore not defined these operators, and only compare @c unique_handle
 *     to @c unique_handle.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#pragma once
#ifndef PSYSTEM_FRAMEWORK_UNIQUE_HANDLE_HPP
#define PSYSTEM_FRAMEWORK_UNIQUE_HANDLE_HPP

#include "platform.hpp"

#include "default_close_handle.hpp"
#include "managed_handle_proxy.hpp"
#include "stack_allocated.hpp"

namespace psystem {

// -----------------------------------------------------------------------------
// Internal class template for unique_handle's deleterless API
// -----------------------------------------------------------------------------

/**
 * @brief Base unique_handle functionality common to all template
 *        specializations.
 *
 * Construction of this class is private, and only the declared friend,
 * unique_handle can create an instance of this class.  This class should be
 * considered @em part of unique_handle --- the part that provides the
 * interface that has nothing to do with the "deleter".  Template specializations
 * for unique_handle focus on optimizing the memory size by removing the
 * deleter field from the structure if it is a stateless function invocation
 * (e.g., @c CloseHandle from the Win32 API).
 *
 * @author  Matt Bisson
 * @date    24 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @todo Irritatingly, @c constexpr is not possible on Windows, which makes the
 *       @c invalid_handle_value member not possible.  It would be great to
 *       change the instances of @c kInvalidHandle back to
 *       @c invalid_handle_value when MSVC brings itself up to three years ago.
 *
 * @todo Maybe we can keep the handle on a block that can be specialized to
 *       have/not have a deleter, rather than specializing pretty much the
 *       entire unique_handle class.
 */
template <typename T, typename D, T kInvalidHandle>
class unique_handle_impl : public psystem::stack_allocated
{
    /// @brief Limit use of this class ONLY to unique_handle; they are two halves
    ///        of the same interface.
    template <typename T2, typename, T2 kInvalidHandle2>
    friend class unique_handle;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Type Declarations
    ////////////////////////////////////////////////////////////////////////////
public:
    /// @brief The data-type of the clean-up facility.
    using deleter_type = D;

    /// @brief The data-type of the handle being managed.
    using handle_type = T;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Constants
    ////////////////////////////////////////////////////////////////////////////
public:
    /// @brief The handle value that marks an "invalid" handle.
#ifndef _MSC_VER
    static constexpr T invalid_handle_value = kInvalidHandle;
#endif

    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
private:
    /**
     * @brief Construct this base container object with a handle.
     *
     * @param[in] handle
     *     The handle value to hold on behalf of derived classes.  This may be
     *     @c invalid_handle_value.
     */
    explicit unique_handle_impl(handle_type handle) noexcept
        : m_handle(handle) { }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Copy Construction
    ////////////////////////////////////////////////////////////////////////////
public:
    /// @brief Disable copying because it will likely be deleted twice.
    unique_handle_impl(unique_handle_impl const&) = delete;

    /// @brief Disable copy-assignment to prevent double-frees.
    unique_handle_impl& operator=(unique_handle_impl&) = delete;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Access the handle contained in this object.
     * @return The handle (it may or may not be valid).
     */
    handle_type get() const noexcept
    {
        return m_handle;
    }

    /**
     * @brief Release the handle from being managed by this class.
     *
     * @return
     *     The handle (formerly) contained in this object (it may or may not be
     *     valid).
     *
     * @post The handle will no longer be "closed" by this object.
     * @post Subsequent handle access will result in @c invalid_handle_value.
     */
    handle_type release() noexcept
    {
        handle_type handle = m_handle;
        m_handle = kInvalidHandle;
        return handle;
    }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Operator Overloads
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Determines the validity of this instance.
     *
     * This is comperable to a @c nullptr check for pointer types.  The
     * conversion is a "contextual" conversion, so it will work implicitly with
     * boolean operators and @c if statements, but cannot implicitly convert to
     * a boolean container.
     *
     * @return @c true if this instance contains a valid handle, @c false,
     *     otherwise.
     */
    explicit operator bool() const noexcept
    {
        return m_handle != kInvalidHandle;
    }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief The handle that will be managed by this class.
    handle_type m_handle;
};

// -----------------------------------------------------------------------------
// Public-facing unique_handle class template
// -----------------------------------------------------------------------------

/**
 * @brief Uniquely manage a single "handle".
 *
 * A handle in this context is any opaque item of data that is used
 * (non-opaquely) by an API.  Typically handles are received from some API
 * invocation, and will need to be "closed" by a separate API to free any
 * resources associated with that handle.
 *
 * When we say that the handle is "uniquely" managed by this instance, we simply
 * mean that no other references to that handle exist (or if they exist, they
 * are guaranteed not to outlive this container), and that it is this container's
 * sole responsibility to free all resources for that handle when the container's
 * lifetime ends.
 *
 * While it is possible to use @c std::unique_ptr with handles, it becomes
 * somewhat ugly (the handle type must be @c void*, the handle must use
 * @c nullptr as its "invalid value (Windows' @c INVALID_HANDLE_VALUE actually
 * maps to @c 0xFFFFFFFF), and so on).  Frankly, it's more fun to see what we
 * can do by creating our own implementation, so here we are.
 *
 * @note
 *     Part of this API exists within unique_handle_impl, but Doxygen isn't
 *     supplying this connection when explicit subgroupings are used.  See this
 *     class template for basic accessors to the handle.
 *
 * @note
 *     By taking the default parameters for this class template, the deleter
 *     parameter is assumed to be immutable, and the interface strips the ability
 *     to specify a deleter during construction or "reset."  This assumption
 *     allows us to optimize the class such that <em>its memory footprint is
 *     exactly the same as the handle it contains.</em>  In other words, using a
 *     unique_handle instance in this way is no more costly than using the
 *     handle directly, except that it provides a lot more functionality and
 *     exception safety.  To visit the documentation for the optimized interface,
 *     see psystem::unique_handle<T,default_close_handle<T>,kInvalidHandle>.
 *
 * @warning
 *     This interface mimics @c std::unique_ptr, but it is not identical.  Be
 *     aware of interface and behavioral differences by carefully reading
 *     documentation for individual methods.
 *
 * @tparam T The type of the handle that is managed by this class.
 * @tparam D
 *     The type of the "deleter" that must clean up the resources consumed by the
 *     handle that is managed by this class.  The clean-up routine must match
 *     the function signature, @c "void(handle_type)", and it must not throw.
 * @tparam kInvalidHandle
 *     For a handle to be used by this class, it must have the concept of an
 *     "invalid" handle value.  This concept is analogous to @c nullptr for
 *     pointer types.  The value passed to this template paraameter represents
 *     the invalid value marker for the handle type managed by this class.
 *
 * @author  Matt Bisson
 * @date    5 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
template <
    typename T = HANDLE,
    typename D = default_close_handle<T>,
    T kInvalidHandle = INVALID_HANDLE_VALUE>
class unique_handle
    : public unique_handle_impl<T, D, kInvalidHandle>
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Type Declarations
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief A convenient name for the base-class type.
    using base = unique_handle_impl<T, D, kInvalidHandle>;
public:
    using typename base::deleter_type;
    using typename base::handle_type;

    /// @brief A safe container for altering the handle by the address-of
    ///        operator.
    using safe_address_container =
        psystem::internal::managed_handle_proxy<
            handle_type,
            unique_handle<handle_type, deleter_type, kInvalidHandle>
        >;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Constants
    ////////////////////////////////////////////////////////////////////////////
public:
#ifndef _MSC_VER
    using base::invalid_handle_value;
#endif

    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct an instance to manage a handle.
     *
     * Construction always succeeds, as exceptions are not allowed.  If no
     * parameter is specified, this constructs an instance that does not
     * (initially) own an handle.
     *
     * @warning
     *     @c deleter_type must be default-constructable, and its constructor
     *     must not throw.
     *
     * @param[in] hndl
     *     The handle (or invalid handle value) that this instance will manage.
     */
    explicit unique_handle(handle_type hndl = kInvalidHandle) noexcept
        : base(hndl),
          m_deleter()
    { }

    /**
     * @brief Construct an instance to manage a handle with a custom "deleter."
     *
     * Construction always succeeds, as exceptions are not allowed.
     *
     * @warning
     *     @c deleter_type must be copy-constructable or move-constructable,
     *     depending on how this constructor is invoked (see @p deleter
     *     parameter documentation), and that constructor may not throw.
     *
     * @param[in] hndl
     *     The handle (or invalid handle value) that this instance will manage.
     * @param[in] deleter
     *     The instance of an object used to clean up the handle that it managed
     *     by this instance.  This is a "universal reference" (as Scott Meyers
     *     termed it), which means:
     *     @li If @p deleter is passed by value, this parameter is a @c const&,
     *         and will be @em copied into this instance.
     *     @li If @p deleter is passed by r-value, this parameter is an r-value
     *         reference, and will be @em moved into this instance.
     *     @li If @p deleter is passed by reference, this parameter is that same
     *         reference type, and will be @em copied into this instance.
     */
    template <typename D2>
    unique_handle(handle_type hndl, D2&& deleter) noexcept
        : base(hndl),
          m_deleter(std::forward<D2>(deleter))
    { }

    /**
     * @brief Transfer ownership of a handle from another unique_handle.
     *
     * @warning
     *     This differs from the @c std::unique_ptr in that it does not allow
     *     the transfer of @em related handle types.  Often, handles are
     *     convertable types (@c void* to @c void* or @c long to @c long) that
     *     are merely obscured via a typedef.  Transferring ownership of
     *     different handle types would also require changing the type of the
     *     @c deleter_type and the value of @c invalid_handle_value.
     *
     * @warning
     *     @c deleter_type must be move-constructable, and it must not throw.
     *
     * @param[in,out] other The unique_handle from which we're moving data.
     */
    unique_handle(unique_handle&& other) noexcept
        : base(other.release()),
          m_deleter(std::move(other.m_deleter))
    { }

    /**
     * @brief Destroy this container, freeing the handle managed within.
     *
     * If the handle is @c invalid_handle_value, this method does not call the
     * deleter.  The deleter contained within this instance will also be
     * destroyed.
     *
     * @warning
     *     Neither the deleter, nor the @c deleter_type::operator() may throw.
     */
    ~unique_handle() noexcept
    {
        if (kInvalidHandle != this->m_handle)
        {
            m_deleter(this->m_handle);
#ifdef _DEBUG
            this->m_handle = kInvalidHandle;
#endif
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Operator Overloads
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Transfer ownership of a handle into this instance.
     *
     * @warning
     *     This differs from the @c std::unique_ptr in that it does not allow
     *     the transfer of @em related handle types.  Often, handles are
     *     convertable types (@c void* to @c void* or @c long to @c long) that
     *     are merely obscured via a typedef.  Transferring ownership of
     *     different handle types would also require changing the type of the
     *     @c deleter_type and the value of @c invalid_handle_value.
     *
     * @param[in,out] o The unique_handle from which we're moving data.
     * @return A reference to @c this, after the assignment.
     */
    unique_handle& operator=(unique_handle&& o) noexcept
    {
        // Since we've overloaded the operator& for this class, our incoming
        // variable is basically unreference-able... until C++11.  See
        // std::addressof()!
        if (std::addressof(o) == this) return *this;

        reset(o.release());

        m_deleter = std::forward<deleter_type>(o.m_deleter);
        return *this;
    }

    /**
     * @brief Allow safe alteration of the handle by routines that return data
     *        by C-style output parameters.
     *
     * @return
     *     A temporary object designed to track any changes to the handle when
     *     accessed by address.  If the handle changes while the "safe"
     *     container tracks it, the prior handle will be released safely.
     */
    safe_address_container operator&() noexcept
    {
        return safe_address_container(*this);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Access the deleter instance within this class.
     *
     * @return
     *     The object that will be used to delete the handle managed in this
     *     class.  This reference is valid for the lifetime of the unique_handle
     *     instance.
     */
    deleter_type& get_deleter() noexcept
    {
        return m_deleter;
    }

    /**
     * @brief Access the @c const deleter instance within this class.
     *
     * @return
     *     The object that will be used to delete the handle managed in this
     *     class.  This reference is valid for the lifetime of the unique_handle
     *     instance.  The data within the returned instance may change as a
     *     side-effect of operator=() or reset().
     */
    deleter_type const& get_deleter() const noexcept
    {
        return m_deleter;
    }

    /**
     * @brief Assign a new handle to be managed by this instance.
     *
     * If there was a valid handle managed previously by this unique_handle
     * instance, its resources will be cleaned up prior to assigning the new
     * handle.  If the new handle equals (@c ==) the old handle, this method
     * does nothing.
     *
     * If no parameters are specified, this resets the unique_handle to the
     * "invalid" state.
     *
     * @param[in] hndl The new handle to manage.  It need not be "valid."
     */
    void reset(handle_type hndl = kInvalidHandle) noexcept
    {
        if (hndl == this->m_handle) return;

        if (kInvalidHandle != this->m_handle)
        {
            m_deleter(this->m_handle);
        }
        this->m_handle = hndl;
    }

    /**
     * @brief Swap ownership of two unique_handle instances.
     *
     * @warning
     *     @c deleter_type must be move-assignable, and that assignment must not
     *     throw.
     *
     * @param[in,out] o
     *     The unique_handle instance that will swap handle data and deleters
     *     with this instance.
     */
    void swap(unique_handle& o) noexcept
    {
        /// @todo Better "best practices" to practice for swap?

        // This would be a pointless swap.
        if (UNLIKELY(&o == this)) return;

        // Make temp copies
        handle_type  tmp_handle  = o.m_handle;
        deleter_type tmp_deleter = std::move(o.m_deleter);

        // Make o valid
        o.m_handle  = this->m_handle;
        o.m_deleter = std::move(m_deleter);

        // Make *this valid
        this->m_handle = tmp_handle;
        m_deleter      = std::move(tmp_deleter);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief The instance that will be used to clean up the handle's resources.
    deleter_type m_deleter;
};

// -----------------------------------------------------------------------------
// Partial specialization to eliminate storage for deleter_type.
// -----------------------------------------------------------------------------

/**
 * @brief Uniquely manage a single "handle" with (optimized for the default
 *        "deleter").
 *
 * See the discussion in the unspecialized psystem::unique_handle for the full
 * rationale behind this class template.  Essentially, this class exists to
 * optimize away the runtime storage for a deleter that we know how to use.
 * Naturally, this causes the API to remove deleter-specific items like
 * unique_handle::get_deleter().
 *
 * @note
 *     Part of this API exists within unique_handle_impl, but Doxygen isn't
 *     supplying this connection when explicit subgroupings are used.  See this
 *     class template for basic accessors to the handle.
 *
 * @author  Matt Bisson
 * @date    24 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
template <typename T, T kInvalidHandle>
class unique_handle<T, default_close_handle<T>, kInvalidHandle>
    : public unique_handle_impl<T, default_close_handle<T>, kInvalidHandle>
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Type Declarations
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief A convenient name for the base-class type.
    using base = unique_handle_impl<T, default_close_handle<T>, kInvalidHandle>;
public:
    using typename base::deleter_type;
    using typename base::handle_type;

    /// @brief A safe container for altering the handle by the address-of
    ///        operator.
    using safe_address_container =
        psystem::internal::managed_handle_proxy<
            handle_type,
            unique_handle<handle_type, deleter_type, kInvalidHandle>
        >;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Constants
    ////////////////////////////////////////////////////////////////////////////
public:
#ifndef _MSC_VER
    using base::invalid_handle_value;
#endif

    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct an instance to manage a handle.
     *
     * Construction always succeeds, as exceptions are not allowed.  If no
     * parameter is specified, this constructs an instance that does not
     * (initially) own an handle.
     *
     * @param[in] hndl
     *     The handle (or invalid handle value) that this instance will manage.
     */
    explicit unique_handle(handle_type hndl = kInvalidHandle) noexcept
        : base(hndl)
    { }

    /**
     * @brief Transfer ownership of a handle from another unique_handle.
     *
     * @warning
     *     This differs from the @c std::unique_ptr in that it does not allow
     *     the transfer of @em related handle types.  Often, handles are
     *     convertable types (@c void* to @c void* or @c long to @c long) that
     *     are merely obscured via a typedef.  Transferring ownership of
     *     different handle types would also require changing the type of the
     *     @c deleter_type and the value of @c invalid_handle_value.
     *
     * @param[in,out] other The unique_handle from which we're moving data.
     */
    unique_handle(unique_handle&& other) noexcept
        : base(other.release())
    { }

    /**
     * @brief Destroy this container, freeing the handle managed within.
     *
     * If the handle is @c invalid_handle_value, this method does not call the
     * deleter.  The deleter contained within this instance will also be
     * destroyed.
     */
    ~unique_handle() noexcept
    {
        if (kInvalidHandle != this->m_handle)
        {
            default_close_handle<T>::close_handle(this->m_handle);
#ifdef _DEBUG
            this->m_handle = kInvalidHandle;
#endif
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Operator Overloads
    ////////////////////////////////////////////////////////////////////////////
public:
    /// @copydoc psystem::unique_handle::operator=()
    unique_handle& operator=(unique_handle&& o) noexcept
    {
        reset(o.release());
        return *this;
    }

    /// @copydoc psystem::unique_handle::operator&()
    safe_address_container operator&() noexcept
    {
        return safe_address_container(*this);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /// @copydoc psystem::unique_handle::reset()
    void reset(handle_type hndl = kInvalidHandle) noexcept
    {
        if (hndl == this->m_handle) return;

        if (kInvalidHandle != this->m_handle)
        {
            default_close_handle<T>::close_handle(this->m_handle);
        }
        this->m_handle = hndl;
    }

    /**
     * @brief Swap ownership of two unique_handle instances.
     *
     * @param[in,out] o
     *     The unique_handle instance that will swap handle data with this
     *     instance.
     */
    void swap(unique_handle& o) noexcept
    {
        // This would be a pointless swap.
        if (UNLIKELY(&o == this)) return;

        handle_type tmp_handle  = o.m_handle;
        o.m_handle = this->m_handle;
        this->m_handle   = tmp_handle;
    }
};

} // namespace psystem

// -----------------------------------------------------------------------------
// Template specializations injected into the std namespace
// -----------------------------------------------------------------------------

namespace std {

/**
 * @brief Delegate @c std::swap calls for unique_handle to the member function.
 *
 * This specialization of @c std::swap allows unique_handle data swaps to occur
 * using either the free function or the member function.
 *
 * @tparam T              The handle type.
 * @tparam D              The deleter type.
 * @tparam kInvalidHandle The value of an "invalid" handle.
 *
 * @param[in,out] o1 The first instance to swap.
 * @param[in,out] o2 The second instance to swap.
 */
template <typename T, typename D, T kInvalidHandle>
void
swap(
    psystem::unique_handle<T, D, kInvalidHandle>& o1,
    psystem::unique_handle<T, D, kInvalidHandle>& o2)
    noexcept
{
    o1.swap(o2);
}

/**
 * @brief Forward the @c hash functor along to the underlying handle type for
 *        psystem::unique_handle.
 *
 * This is primarily useful for containers of handles.
 *
 * @tparam T              The handle type.
 * @tparam D              The deleter type.
 * @tparam kInvalidHandle The value of an "invalid" handle.
 */
template <typename T, typename D, T kInvalidHandle>
struct hash< psystem::unique_handle<T, D, kInvalidHandle> >
{
    /// @brief Convenient name for shared_handle.
    using unique_handle_type = psystem::unique_handle<T, D, kInvalidHandle>;

    /// @brief Invoke the hash functor.
    /// @param[in] value The handle to obtain a hash for.
    /// @return The hashed value of the handle.
    size_t operator()(unique_handle_type const& value) const
    {
        return std::hash<typename unique_handle_type::handle_type>()(
            value.get());
    }
};

} // namespace std

// -----------------------------------------------------------------------------
// Free functions related to unique_handle
// -----------------------------------------------------------------------------

/**
 * @brief Test for equality of two handles.
 *
 * This ignores any deleter instance in the unique_handle.
 *
 * @tparam T              The handle type.
 * @tparam D              The deleter type.
 * @tparam kInvalidHandle The value of an "invalid" handle.
 *
 * @param[in] h1 The first unique_handle to compare.
 * @param[in] h2 The second unique_handle to compare.
 *
 * @return The value of the comparison.
 */
template <typename T, typename D, T kInvalidHandle>
inline bool operator==(
    psystem::unique_handle<T, D, kInvalidHandle> const& h1,
    psystem::unique_handle<T, D, kInvalidHandle> const& h2) noexcept
{
    return (h1.get() == h2.get());
}

/**
 * @brief Test for inequality of two handles.
 *
 * This ignores any deleter instance in the unique_handle.
 *
 * Inequality is defined in terms of operator=().
 *
 * @tparam T              The handle type.
 * @tparam D              The deleter type.
 * @tparam kInvalidHandle The value of an "invalid" handle.
 *
 * @param[in] h1 The first unique_handle to compare.
 * @param[in] h2 The second unique_handle to compare.
 *
 * @return The value of the comparison.
 */
template <typename T, typename D, T kInvalidHandle>
inline bool operator!=(
    psystem::unique_handle<T, D, kInvalidHandle> const& h1,
    psystem::unique_handle<T, D, kInvalidHandle> const& h2) noexcept
{
    return !(h1 == h2);
}

/**
 * @brief Test if the first handle is less than the second.
 *
 * This ignores any deleter instance in the unique_handle.
 *
 * This comparison is defined in terms of @c std::less.
 *
 * @tparam T              The handle type.
 * @tparam D              The deleter type.
 * @tparam kInvalidHandle The value of an "invalid" handle.
 *
 * @param[in] h1 The first unique_handle to compare.
 * @param[in] h2 The second unique_handle to compare.
 *
 * @return The value of the comparison.
 */
template <typename T, typename D, T kInvalidHandle>
inline bool operator<(
    psystem::unique_handle<T, D, kInvalidHandle> const& h1,
    psystem::unique_handle<T, D, kInvalidHandle> const& h2) noexcept
{
    return std::less<
        typename psystem::unique_handle<T, D, kInvalidHandle>::handle_type>(
            h1.get(), h2.get());
}

/**
 * @brief Test if the first handle is less than or equal to the second.
 *
 * This ignores any deleter instance in the unique_handle.
 *
 * This comparison is defined in terms of operator<()
 *
 * @tparam T              The handle type.
 * @tparam D              The deleter type.
 * @tparam kInvalidHandle The value of an "invalid" handle.
 *
 * @param[in] h1 The first unique_handle to compare.
 * @param[in] h2 The second unique_handle to compare.
 *
 * @return The value of the comparison.
 */
template <typename T, typename D, T kInvalidHandle>
inline bool operator<=(
    psystem::unique_handle<T, D, kInvalidHandle> const& h1,
    psystem::unique_handle<T, D, kInvalidHandle> const& h2) noexcept
{
    return !(h2 < h1);
}

/**
 * @brief Test if the first handle is greater than the second.
 *
 * This ignores any deleter instance in the unique_handle.
 *
 * This comparison is defined in terms of operator<()
 *
 * @tparam T              The handle type.
 * @tparam D              The deleter type.
 * @tparam kInvalidHandle The value of an "invalid" handle.
 *
 * @param[in] h1 The first unique_handle to compare.
 * @param[in] h2 The second unique_handle to compare.
 *
 * @return The value of the comparison.
 */
template <typename T, typename D, T kInvalidHandle>
inline bool operator>(
    psystem::unique_handle<T, D, kInvalidHandle> const& h1,
    psystem::unique_handle<T, D, kInvalidHandle> const& h2) noexcept
{
    return (h2 < h1);
}

/**
 * @brief Test if the first handle is greater than or equal to the second.
 *
 * This ignores any deleter instance in the unique_handle.
 *
 * This comparison is defined in terms of operator<()
 *
 * @tparam T              The handle type.
 * @tparam D              The deleter type.
 * @tparam kInvalidHandle The value of an "invalid" handle.
 *
 * @param[in] h1 The first unique_handle to compare.
 * @param[in] h2 The second unique_handle to compare.
 *
 * @return The value of the comparison.
 */
template <typename T, typename D, T kInvalidHandle>
inline bool operator>=(
    psystem::unique_handle<T, D, kInvalidHandle> const& h1,
    psystem::unique_handle<T, D, kInvalidHandle> const& h2) noexcept
{
    return !(h1 < h2);
}

#endif // PSYSTEM_FRAMEWORK_UNIQUE_HANDLE_HPP
