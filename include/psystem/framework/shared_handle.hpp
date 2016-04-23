// ===-- include/psystem/framework/shared_handle.hpp ------ -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare an RAII container for sharing system API handles
 *        (psystem::shared_handle).
 *
 * @note
 *     There are no comparison operators for @c shared_handle to simple @c handle
 *     types for a few reasons.  First, it is difficult challenging (not
 *     impossible) to decide on a type for the unmanaged handle because handles
 *     are generally pointers or even integers that have been given another name
 *     via typedef.  They may also be forward declarations of a type as well.
 *     The second reason is that many handle types will conflict with the
 *     @c bool operator---the expression @c "shared_handle(hndl) == hndl" may
 *     first convert the @c shared_handle to a bool, then use a simple equality
 *     operator that compares POD integers, or it call the @c operator== that
 *     takes @c shared_handle and the handle type.  There is ambiguity.  We will
 *     therefore not defined these operators, and only compare @c shared_handle
 *     to @c shared_handle.
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
#ifndef PSYSTEM_FRAMEWORK_SHARED_HANDLE_HPP
#define PSYSTEM_FRAMEWORK_SHARED_HANDLE_HPP

#include "platform.hpp"

#include <atomic>

#include "default_close_handle.hpp"
#include "stack_allocated.hpp"
#include "unique_handle.hpp" // Needed?

namespace psystem {

/**
 * @brief Share management of a single "handle" between multiple owners.
 *
 * A handle in this context is any opaque item of data that is used
 * (non-opaquely) by an API.  Typically handles are received from some API
 * invocation, and will need to be "closed" by a separate API to free any
 * resources associated with that handle.
 *
 * This class shares access and management of a handle between shared_handle
 * instances by allocating a separate "control block."  This way, the control
 * block data can be observed by all shared_handle instances, and when a single
 * instance is all that remains (see unique()), the handle can be closed on the
 * deletion of the instance.  The control block is also shared between "weak"
 * references, so when no weak or strong reference containers exist, the control
 * block must be deleted.  Custom deletion for the handle exists within the
 * control block as well --- when the control block is destroyed, so will the
 * deleter provided to it.
 *
 * While it is possible to use @c std::shared_ptr with handles, it becomes
 * somewhat ugly (the handle type must be @c void*, the handle must use
 * @c nullptr as its "invalid value (Windows' @c INVALID_HANDLE_VALUE actually
 * maps to @c 0xFFFFFFFF), and so on).  Frankly, it's more fun to see what we
 * can do by creating our own implementation, so here we are.
 *
 * @internal @invariant
 *     If @c m_handle == @c invalid_handle_value, then
 *     @c m_control_block == @c nullptr.
 *
 * @warning
 *     This interface mimics @c std::shared_ptr, but it is not identical.  Be
 *     aware of interface and behavioral differences by carefully reading
 *     documentation for individual methods.
 *
 * @tparam T The type of the handle that is managed by this class.
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
 *
 * @todo Irritatingly, @c constexpr is not possible on Windows, which makes the
 *       @c invalid_handle_value member not possible.  It would be great to
 *       change the instances of @c kInvalidHandle back to
 *       @c invalid_handle_value when MSVC brings itself up to three years ago.
 */
template <typename T = HANDLE, T kInvalidHandle = INVALID_HANDLE_VALUE>
class shared_handle : public psystem::stack_allocated
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Internal Classes
    ////////////////////////////////////////////////////////////////////////////
private:
    /**
     * @brief The basic control block for a shared_handle.
     *
     * This is a "basic" control block because it does not support a custom
     * deleter.
     */
    struct shared_handle_data
    {
        /// @brief Initialize a usage count of 1 and a weak-usage count of 1.
        shared_handle_data() noexcept
            : m_usage_count(1),
              m_weak_count(1)
        { }

        /// @brief Provide a virtual destructor for polymorphic deletion.
        virtual ~shared_handle_data() noexcept { }

        /**
         * @brief Remove resources associated with a handle.
         *
         * The intention of this method is for cleaning the handle that is
         * managed by a shared_handle instance.  This is a virtual method
         * because the derived class may use a different deleter.
         *
         * @param[in] hndl The handle to close.
         *
         * @note
         *     If I didn't have to support the deleter being provided in
         *     construction (to match the @c std::shared_ptr interface), I
         *     wouldn't need the control block to have a template param (or a
         *     @c vtable).
         */
        virtual void delete_handle(T hndl) noexcept
        {
            default_close_handle<T>::close_handle(hndl);
        }

        /**
         * @brief The count of shared_handle instances that are managing this
         *        handle.
         */
        std::atomic<size_t> m_usage_count;

        /**
         * @brief The "weak" reference count (manages the control block).
         *
         * This is the count of weak_handle instances --- plus 1 if there are
         * active shared_handle instances.
         */
        std::atomic<size_t> m_weak_count;
    };

    /**
     * @brief A control block for a shared_handle with a user-specified deleter.
     *
     * @tparam D The deleter type.  It should be callable with the function
     *           signature, @c "void(T handle)".
     */
    template <typename D>
    struct shared_handle_data_with_delete
        : public shared_handle_data
    {
        /// @brief The facility used to clean handle resources.
        using deleter_type = D;

        /**
         * @brief Construct this instance with a custom deleter.
         * @param[in] deleter The deleter to copy into this instance.
         * @warning The deleter must be copy-constructable and must not throw.
         */
        explicit shared_handle_data_with_delete(deleter_type const& deleter)
            noexcept
            : shared_handle_data(),
              m_deleter(deleter)
        { }

        /**
         * @brief Construct this instance by taking ownership of a deleter.
         * @param[in] deleter The deleter to own.
         * @warning The deleter must be move-constructable and must not throw.
         */
        explicit shared_handle_data_with_delete(deleter_type&& deleter) noexcept
            : shared_handle_data(),
              m_deleter(std::move(deleter))
        { }

        /**
         * @brief Invoke the provided deleter to clean up handle resources.
         * @param[in] hndl The handle to close.
         */
        virtual void delete_handle(T hndl) noexcept override
        {
            m_deleter(hndl);
        }

        /// @brief A user-provided handle deletion utility.
        deleter_type m_deleter;
    };

    ////////////////////////////////////////////////////////////////////////////
    /// @name Type Declarations
    ////////////////////////////////////////////////////////////////////////////
public:
    /// @brief The data-type of the handle being managed.
    using handle_type = T;

    /// @brief A safe container for altering the handle by the address-of
    ///        operator.
    using safe_address_container =
        psystem::internal::managed_handle_proxy<
            handle_type, shared_handle<handle_type, kInvalidHandle> >;

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
    explicit shared_handle(handle_type hndl = kInvalidHandle) noexcept
        : m_handle(hndl),
          m_control_block(
              (kInvalidHandle != hndl)
              ? new shared_handle_data
              : nullptr)
    { }

    /**
     * @brief Construct an instance to manage a handle with a custom "deleter."
     *
     * Construction always succeeds, as exceptions are not allowed.
     *
     * @warning
     *     @p D must be copy-constructable or move-constructable, depending on
     *     how this constructor is invoked (see @p deleter parameter
     *     documentation), and that constructor may not throw.
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
    template <typename D>
    shared_handle(handle_type hndl, D&& deleter) noexcept
        : m_handle(hndl),
          m_control_block(
              (kInvalidHandle != hndl)
              ? new shared_handle_data_with_delete<D>(std::forward<D>(deleter))
              : nullptr)
    { }

    /**
     * @brief Share ownership of a handle with another shared_handle.
     *
     * @warning
     *     This differs from the @c std::shared_ptr in that it does not allow
     *     the transfer of @em related handle types.  Often, handles are
     *     convertable types (@c void* to @c void* or @c long to @c long) that
     *     are merely obscured via a typedef.  Transferring ownership of
     *     different handle types would also require changing the type of the
     *     deleter's type and the value of @c invalid_handle_value.
     *
     * @param[in] other The shared_handle with which we're sharing data.
     */
    shared_handle(shared_handle const& other) noexcept
        : m_handle(other.m_handle),
          m_control_block(other.m_control_block)
    {
        if (kInvalidHandle != m_handle) increment();
    }

    /**
     * @brief Transfer ownership of a handle from another shared_handle.
     *
     * @warning
     *     This differs from the @c std::shared_ptr in that it does not allow
     *     the transfer of @em related handle types.  Often, handles are
     *     convertable types (@c void* to @c void* or @c long to @c long) that
     *     are merely obscured via a typedef.  Transferring ownership of
     *     different handle types would also require changing the type of the
     *     deleter's type and the value of @c invalid_handle_value.
     *
     * @param[in,out] other The shared_handle from which we're moving data.
     */
    shared_handle(shared_handle&& other)
        : m_handle(other.m_handle),
          m_control_block(other.m_control_block)
    {
        other.m_handle        = kInvalidHandle;
        other.m_control_block = nullptr;
    }

    /**
     * @brief Transfer ownership of a handle from an unique_handle.
     *
     * This method moves the handle managed by a unique_handle along with the
     * deleter that is assigned to the instance.
     *
     * @warning
     *     This differs from the @c std::shared_ptr in that it does not allow
     *     the transfer of @em related handle types.  Often, handles are
     *     convertable types (@c void* to @c void* or @c long to @c long) that
     *     are merely obscured via a typedef.  Transferring ownership of
     *     different handle types would also require changing the type of the
     *     deleter's type and the value of @c invalid_handle_value.
     * @warning
     *     @p D must be move-constructable, and it must not throw.
     *
     * @tparam D
     *     The type of the "deleter" that must clean up the resources consumed
     *     by the handle that is managed by this class.  The clean-up routine
     *     must match the function signature, @c "void(handle_type)", and it
     *     must not throw.
     *
     * @param[in,out] other The unique_handle from which we're moving data.
     */
    template <typename D>
    explicit shared_handle(psystem::unique_handle<T, D, kInvalidHandle>&& other)
        noexcept
        : m_handle(other.release()),
          m_control_block(
              (kInvalidHandle != m_handle)
              ? new shared_handle_data_with_delete<D>(
                  std::move(other.get_deleter()))
              : nullptr)
    { }

    /**
     * @brief Transfer ownership of a handle from a default unique_handle type.
     *
     * This method moves the handle managed by a unique_handle.
     *
     * @warning
     *     This differs from the @c std::shared_ptr in that it does not allow
     *     the transfer of @em related handle types.  Often, handles are
     *     convertable types (@c void* to @c void* or @c long to @c long) that
     *     are merely obscured via a typedef.  Transferring ownership of
     *     different handle types would also require changing the type of the
     *     deleter's type and the value of @c invalid_handle_value.
     *
     * @param[in,out] other The unique_handle from which we're moving data.
     */
    explicit shared_handle(
        psystem::unique_handle<
            T, default_close_handle<T>, kInvalidHandle>&& other) noexcept
        : m_handle(other.release()),
          m_control_block(
              (kInvalidHandle != m_handle)
              ? new shared_handle_data
              : nullptr)
    { }

    /**
     * @brief Release ownership of the handle by destroying this instance.
     *
     * If this handle reports @c true from unique(), the handle will be closed,
     * and the data associated with it will be cleaned (by the deleter, if
     * specified).
     *
     * If the handle is @c invalid_handle_value, this method does not call the
     * deleter.  The deleter contained within this instance will also be
     * destroyed.
     *
     * @warning
     *     Neither the deleter, nor the @c D::operator() may throw.
     */
    ~shared_handle() noexcept
    {
        if (kInvalidHandle != m_handle) decrement();
    }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Operator Overloads
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Share ownership of a handle managed by a shared_handle instance.
     *
     * @warning
     *     This differs from the @c std::shared_ptr in that it does not allow
     *     the transfer of @em related handle types.  Often, handles are
     *     convertable types (@c void* to @c void* or @c long to @c long) that
     *     are merely obscured via a typedef.  Transferring ownership of
     *     different handle types would also require changing the type of the
     *     deleter's type and the value of @c invalid_handle_value.
     *
     * @note
     *     This operation may not throw, but it provides a "strong" exception
     *     guarantee in case that requirement changes.  That is, if there is an
     *     exception while moving the unique_handle, the current shared_handle
     *     instance is unmodified.  Exception requirements can be relaxed when
     *     MSVC supports the @c noexcept keyword, and this property can be
     *     determined at using the @c noexcept operator.
     *
     * @param[in,out] o The shared_handle with which we're sharing data.
     * @return A reference to @c this, after the assignment.
     */
    shared_handle& operator=(shared_handle const& o) noexcept
    {
        if (&o == this) return *this;

        shared_handle tmp = o; // Strong exception guarantee

        std::swap(tmp.m_handle,        m_handle);
        std::swap(tmp.m_control_block, m_control_block);

        return *this;
    }

    /**
     * @brief Transfer ownership of a handle into this insance.
     *
     * @warning
     *     This differs from the @c std::shared_ptr in that it does not allow
     *     the transfer of @em related handle types.  Often, handles are
     *     convertable types (@c void* to @c void* or @c long to @c long) that
     *     are merely obscured via a typedef.  Transferring ownership of
     *     different handle types would also require changing the type of the
     *     deleter's type and the value of @c invalid_handle_value.
     *
     * @note
     *     This operation may not throw, but it provides a "strong" exception
     *     guarantee in case that requirement changes.  That is, if there is an
     *     exception while moving the unique_handle, the current shared_handle
     *     instance is unmodified.  Exception requirements can be relaxed when
     *     MSVC supports the @c noexcept keyword, and this property can be
     *     determined at using the @c noexcept operator.
     *
     * @param[in,out] o The shared_handle from which we're moving data.
     * @return A reference to @c this, after the assignment.
     */
    shared_handle& operator=(shared_handle&& o) noexcept
    {
        if (&o == this) return *this;

        shared_handle tmp = std::move(o); // Strong exception guarantee

        std::swap(tmp.m_handle,        m_handle);
        std::swap(tmp.m_control_block, m_control_block);

        return *this;
    }

    /**
     * @brief Move an unique_handle instance into a shared_handle.
     *
     * This method moves the handle managed by a unique_handle along with the
     * deleter that is assigned to the instance.
     *
     * @warning
     *     This differs from the @c std::shared_ptr in that it does not allow
     *     the transfer of @em related handle types.  Often, handles are
     *     convertable types (@c void* to @c void* or @c long to @c long) that
     *     are merely obscured via a typedef.  Transferring ownership of
     *     different handle types would also require changing the type of the
     *     deleter's type and the value of @c invalid_handle_value.
     *
     * @note
     *     This operation may not throw, but it provides a "strong" exception
     *     guarantee in case that requirement changes.  That is, if there is an
     *     exception while moving the unique_handle, the current shared_handle
     *     instance is unmodified.  Exception requirements can be relaxed when
     *     MSVC supports the @c noexcept keyword, and this property can be
     *     determined at using the @c noexcept operator.
     *
     * @tparam D
     *     The type of the "deleter" that must clean up the resources consumed
     *     by the handle that is managed by this class.  The clean-up routine
     *     must match the function signature, @c "void(handle_type)", and it
     *     must not throw.
     *
     * @param[in,out] o The unique_handle from which we're moving data.
     * @return A reference to @c this, after the assignment.
     */
    template <typename D>
    shared_handle& operator=(
        psystem::unique_handle<T, D, kInvalidHandle>&& o) noexcept
    {
        if (&o == this) return *this;

        shared_handle tmp = std::move(o); // Strong exception guarantee

        std::swap(tmp.m_handle,        m_handle);
        std::swap(tmp.m_control_block, m_control_block);

        return *this;
    }

    /**
     * @brief Transfer ownership of a handle from a default unique_handle type.
     *
     * This method moves the handle managed by a unique_handle.
     *
     * @warning
     *     This differs from the @c std::shared_ptr in that it does not allow
     *     the transfer of @em related handle types.  Often, handles are
     *     convertable types (@c void* to @c void* or @c long to @c long) that
     *     are merely obscured via a typedef.  Transferring ownership of
     *     different handle types would also require changing the type of the
     *     deleter's type and the value of @c invalid_handle_value.
     *
     * @note
     *     This operation may not throw, but it provides a "strong" exception
     *     guarantee in case that requirement changes.  That is, if there is an
     *     exception while moving the unique_handle, the current shared_handle
     *     instance is unmodified.  Exception requirements can be relaxed when
     *     MSVC supports the @c noexcept keyword, and this property can be
     *     determined at using the @c noexcept operator.
     *
     * @param[in,out] o The unique_handle from which we're moving data.
     * @return A reference to @c this, after the assignment.
     */
    shared_handle& operator=(
        psystem::unique_handle<T, default_close_handle<T>, kInvalidHandle>&& o)
        noexcept
    {
        if (&o == this) return *this;

        shared_handle tmp = std::move(o); // Strong exception guarantee

        std::swap(tmp.m_handle,        m_handle);
        std::swap(tmp.m_control_block, m_control_block);

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
        return (kInvalidHandle != m_handle);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Public Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Access the handle contained in this object.
     *
     * @return The handle (it may or may not be valid).  A valid handle returned
     *         by this method will remain so until the last shared_handle
     *         releases it.
     */
    handle_type get() const noexcept
    {
        return m_handle;
    }

    /**
     * @brief Assign a new handle to be managed by this instance.
     *
     * If there was a valid handle managed previously by this shared_handle
     * instance, its resources may be cleaned up prior to assigning the new
     * handle.  If the new handle equals (@c ==) the old handle, this method
     * does nothing.
     *
     * If no parameters are specified, this resets the shared_handle to the
     * "invalid" state.
     *
     * @param[in] hndl The new handle to manage.  It need not be "valid."
     */
    void reset(handle_type hndl = kInvalidHandle) noexcept
    {
        if (kInvalidHandle != m_handle) decrement();
        m_handle = hndl;
    }

    /**
     * @brief Assign a new handle to be managed by this instance with a custom
     *        deleter.
     *
     * If there was a valid handle managed previously by this shared_handle
     * instance, its resources may be cleaned up prior to assigning the new
     * handle.  If the new handle equals (@c ==) the old handle, this method
     * does nothing.
     *
     * If no parameters are specified, this resets the shared_handle to the
     * "invalid" state.
     *
     * @tparam D
     *     The type of the "deleter" that must clean up the resources consumed
     *     by the handle that is managed by this class.  The clean-up routine
     *     must match the function signature, @c "void(handle_type)", and it
     *     must not throw.
     *
     * @param[in] hndl
     *     The new handle to manage.  It need not be "valid."
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
    template <typename D>
    void reset(handle_type hndl, D&& deleter) noexcept
    {
        // Create a temporary copy to see that the parameters are valid before
        // even touching *this.
        shared_handle tmp(hndl, std::forward<D>(deleter));

        // Now that we succeeded, we can alter *this.
        if (kInvalidHandle != m_handle) decrement();
        std::swap(tmp.m_handle,        m_handle);
        std::swap(tmp.m_control_block, m_control_block);
    }

    /**
     * @brief Exchange ownership of two shared_handle instances.
     *
     * @param[in,out] o
     *     The shared_handle instance that will swap its handle and control
     *     block with this instance.
     */
    void swap(shared_handle& o) noexcept
    {
        if (&o == this) return;

        std::swap(o.m_handle,        m_handle);
        std::swap(o.m_control_block, m_control_block);
    }

    /**
     * @brief Determines if this is the only instance managing the current
     *        handle.
     * @return This method returns @c true if the reference count is 1.  If the
     *         handle being managed is invalid, the return is @c false.
     */
    bool unique() const noexcept
    {
        return ((m_control_block) && (m_control_block->m_usage_count == 1));
    }

    /**
     * @brief The count of shared_handle instances managing this handle.
     * @return A value from [0,SIZE_MAX].  If the handle being managed is
     *         invalid, the method returns 0.
     */
    size_t use_count() const noexcept
    {
        return (!m_control_block) ? 0 : m_control_block->m_usage_count;
    }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Private Utilities
    ////////////////////////////////////////////////////////////////////////////
private:
    /**
     * @brief Decrement the managed handle's reference count.
     *
     * This method may have the side-effect of freeing the resources held by the
     * handle (i.e., closing the handle) and possibly deleting the control block
     * if the handle has been closed.  If the handle is deleted, @c m_handle
     * will be set to @c invalid_handle_value.
     */
    void decrement() noexcept
    {
        ASSERT(m_control_block);

        auto const usage_count =
            m_control_block->m_usage_count.fetch_sub(
                1, std::memory_order_relaxed);
        ASSERT(0 < usage_count);

        if (1 == usage_count)
        {
            m_control_block->delete_handle(m_handle);
            m_handle = kInvalidHandle;

            auto const weak_count =
                m_control_block->m_weak_count.fetch_sub(1);
            ASSERT(0 < weak_count);

            if (1 == weak_count)
            {
                delete m_control_block;
                m_control_block = nullptr;
            }
        }
    }

    /// @brief Increment the managed handle's reference count.
    void increment() noexcept
    {
        ASSERT(m_control_block);

        // Use relaxed memory ordering for incrementing because nothing really
        // destructive happens as a result of incrementing the ref count...
        auto const usage_count = m_control_block->m_usage_count.fetch_add(
            1, std::memory_order_relaxed);

        // We just blew through the max number of shared_handles!
        ASSERT_USING_VAR(usage_count, SIZE_MAX > usage_count);
    }

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief The handle to manage within this class.
    handle_type m_handle;

    /**
     * @brief The shared "control block" for all instances managing the handle.
     *
     * The control block is passed from shared_handle to shared_handle when
     * managing the same @c handle_type instance.  It is called the control
     * block because it contains all the reference counts (and the deleter) to
     * determine when the data may be cleaned up.
     *
     * When @c m_control_block.m_usage_count is 0, the @c m_handle may be
     * closed.  When @c m_control_block.m_weak_count is 0, @c m_control_block
     * itself shall be freed.
     *
     * @note When @c m_handle is @c invalid_handle_value, @c m_control_block is
     *       @c nullptr.
     */
    shared_handle_data *m_control_block;
};

} // namespace psystem

// -----------------------------------------------------------------------------
// Template specializations injected into the std namespace
// -----------------------------------------------------------------------------

namespace std {

/**
 * @brief Delegate @c std::swap calls for shared_handle to the member function.
 *
 * This specialization of @c std::swap allows shared_handle data swaps to occur
 * using either the free function or the member function.
 *
 * @tparam T              The handle type.
 * @tparam kInvalidHandle The value of an "invalid" handle.
 *
 * @param[in,out] o1 The first instance to swap.
 * @param[in,out] o2 The second instance to swap.
 */
template <typename T, T kInvalidHandle>
void
swap(
    psystem::shared_handle<T, kInvalidHandle>& o1,
    psystem::shared_handle<T, kInvalidHandle>& o2)
    noexcept
{
    o1.swap(o2);
}

/**
 * @brief Forward the @c hash functor along to the underlying handle type for
 *        psystem::shared_handle.
 *
 * This is primarily useful for containers of handles.
 *
 * @tparam T              The handle type.
 * @tparam kInvalidHandle The value of an "invalid" handle.
 */
template <typename T, T kInvalidHandle>
struct hash< psystem::shared_handle<T, kInvalidHandle> >
{
    /// @brief Convenient name for shared_handle.
    using shared_handle_type = psystem::shared_handle<T, kInvalidHandle>;

    /// @brief Invoke the hash functor.
    /// @param[in] value The handle to obtain a hash for.
    /// @return The hashed value of the handle.
    size_t operator()(shared_handle_type const& value) const
    {
        return std::hash<typename shared_handle_type::handle_type>()(
            value.get());
    }
};

} // namespace std

// -----------------------------------------------------------------------------
// Free functions related to shared_handle
// -----------------------------------------------------------------------------

/**
 * @brief Test for equality of two handles.
 *
 * This ignores any deleter instance in the shared_handle.
 *
 * @tparam T              The handle type.
 * @tparam kInvalidHandle The value of an "invalid" handle.
 *
 * @param[in] h1 The first shared_handle to compare.
 * @param[in] h2 The second shared_handle to compare.
 *
 * @return The value of the comparison.
 */
template <typename T, T kInvalidHandle>
inline bool operator==(
    psystem::shared_handle<T, kInvalidHandle> const& h1,
    psystem::shared_handle<T, kInvalidHandle> const& h2) noexcept
{
    return (h1.get() == h2.get());
}

/**
 * @brief Test for inequality of two handles.
 *
 * This ignores any deleter instance in the shared_handle.
 *
 * Inequality is defined in terms of operator=().
 *
 * @tparam T              The handle type.
 * @tparam kInvalidHandle The value of an "invalid" handle.
 *
 * @param[in] h1 The first shared_handle to compare.
 * @param[in] h2 The second shared_handle to compare.
 *
 * @return The value of the comparison.
 */
template <typename T, T kInvalidHandle>
inline bool operator!=(
    psystem::shared_handle<T, kInvalidHandle> const& h1,
    psystem::shared_handle<T, kInvalidHandle> const& h2) noexcept
{
    return !(h1 == h2);
}

/**
 * @brief Test if the first handle is less than the second.
 *
 * This ignores any deleter instance in the shared_handle.
 *
 * This comparison is defined in terms of @c std::less.
 *
 * @tparam T              The handle type.
 * @tparam kInvalidHandle The value of an "invalid" handle.
 *
 * @param[in] h1 The first shared_handle to compare.
 * @param[in] h2 The second shared_handle to compare.
 *
 * @return The value of the comparison.
 */
template <typename T, T kInvalidHandle>
inline bool operator<(
    psystem::shared_handle<T, kInvalidHandle> const& h1,
    psystem::shared_handle<T, kInvalidHandle> const& h2) noexcept
{
    return std::less<
        typename psystem::shared_handle<T, kInvalidHandle>::handle_type>(
            h1.get(), h2.get());
}

/**
 * @brief Test if the first handle is less than or equal to the second.
 *
 * This ignores any deleter instance in the shared_handle.
 *
 * This comparison is defined in terms of operator<()
 *
 * @tparam T              The handle type.
 * @tparam kInvalidHandle The value of an "invalid" handle.
 *
 * @param[in] h1 The first shared_handle to compare.
 * @param[in] h2 The second shared_handle to compare.
 *
 * @return The value of the comparison.
 */
template <typename T, T kInvalidHandle>
inline bool operator<=(
    psystem::shared_handle<T, kInvalidHandle> const& h1,
    psystem::shared_handle<T, kInvalidHandle> const& h2) noexcept
{
    return !(h2 < h1);
}

/**
 * @brief Test if the first handle is greater than the second.
 *
 * This ignores any deleter instance in the shared_handle.
 *
 * This comparison is defined in terms of operator<()
 *
 * @tparam T              The handle type.
 * @tparam kInvalidHandle The value of an "invalid" handle.
 *
 * @param[in] h1 The first shared_handle to compare.
 * @param[in] h2 The second shared_handle to compare.
 *
 * @return The value of the comparison.
 */
template <typename T, T kInvalidHandle>
inline bool operator>(
    psystem::shared_handle<T, kInvalidHandle> const& h1,
    psystem::shared_handle<T, kInvalidHandle> const& h2) noexcept
{
    return (h2 < h1);
}

/**
 * @brief Test if the first handle is greater than or equal to the second.
 *
 * This ignores any deleter instance in the shared_handle.
 *
 * This comparison is defined in terms of operator<()
 *
 * @tparam T              The handle type.
 * @tparam kInvalidHandle The value of an "invalid" handle.
 *
 * @param[in] h1 The first shared_handle to compare.
 * @param[in] h2 The second shared_handle to compare.
 *
 * @return The value of the comparison.
 */
template <typename T, T kInvalidHandle>
inline bool operator>=(
    psystem::shared_handle<T, kInvalidHandle> const& h1,
    psystem::shared_handle<T, kInvalidHandle> const& h2) noexcept
{
    return !(h1 < h2);
}

#endif // PSYSTEM_FRAMEWORK_SHARED_HANDLE_HPP
