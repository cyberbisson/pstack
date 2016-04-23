// ===-- include/psystem/image_symbol_iterator.hpp -------- -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare an iterator for examining debuggable symbols.
 *
 * @author  Matt Bisson
 * @date    8 February, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#pragma once
#ifndef PSYSTEM_IMAGE_SYMBOL_ITERATOR_HPP
#define PSYSTEM_IMAGE_SYMBOL_ITERATOR_HPP

#include "psystem_ns.hpp"

#include <iterator>

namespace psystem {

/**
 * @brief Iterator type for COFF-format debugging symbols.
 *
 * COFF images often maintain a section of the file dedicated to debugging
 * symbols.  The Win32 type, @c IMAGE_SYMBOL exposes this information, and that
 * is the type that this iterator examines.
 *
 * @note While this type defines @c operator- as a member function, @c operator+
 * is a free-function to allow "4 + it" and "it + 4".  While "it - 4" makes
 * sense, "4 - it" is meaningless.  Also, while "it1 - it2" is a reasonable
 * offset computation, "it1 + it2" is meaningless.
 *
 * @author  Matt Bisson
 * @date    8 February, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @todo Some debug runtime bounds checking would be nice.
 */
class image_symbol_iterator
    : public std::iterator<std::random_access_iterator_tag, IMAGE_SYMBOL const>
{
    ////////////////////////////////////////////////////////////////////////////
    /// @name Construction / Destruction
    ////////////////////////////////////////////////////////////////////////////
public:
    /**
     * @brief Construct an iterator based on a backing store.
     *
     * This constructor copies only one pointer, but the data backing this
     * pointer must outlive the iteration.
     *
     * @param[in] in_ptr
     *    I intended the iterator to be backed by a memory-mapped COFF image, so
     *    the input is a binary blob that we know points to the debugging
     *    symbols.  This should be a contiguous array of @c IMAGE_SYMBOL
     *    structures.  The constructor has no way to validate the real type of
     *    the data passed to it, so take care.
     */
    explicit image_symbol_iterator(psystem::address_t const in_ptr) noexcept;

    /**
     * @brief Copy another iterator's state into this one.
     *
     * @param[in] o The other iterator instance from which to transfer data.
     */
    image_symbol_iterator(image_symbol_iterator const& o) noexcept;

    /**
     * @brief Destroy this iterator instance.
     * @todo Should "=default" when MSVC sorts out "noexcept".
     */
    ~image_symbol_iterator() noexcept;

    /**
     * @brief Assign a new state to this instance.
     *
     * @param[in] o The other iterator instance from which to transfer state.
     * @return The current instance.
     */
    image_symbol_iterator& operator=(image_symbol_iterator const& o) noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Basic Iterator Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    ////////////////////////////////////////
    // Basic equality:
    ////////////////////////////////////////

    /**
     * @brief Determine the equality of two instances.
     *
     * @param[in] other The instance to compare against @c this for equivalence.
     * @return
     *     @c true if both @c this and @p other examine the same location in the
     *     same container; @c false in all other cases.
     */
    bool operator==(image_symbol_iterator const& other) const noexcept;

    /**
     * @brief Determine the inequality of two instances.
     *
     * @param[in] other The instance to compare against @c this for
     *                  non-equivalence.
     * @return
     *     @c false if both @c this and @p other examine the same location in
     *     the same container; @c true in all other cases.
     */
    bool operator!=(image_symbol_iterator const& other) const noexcept;

    ////////////////////////////////////////
    // "Magnitude" Comparison:
    ////////////////////////////////////////

    /**
     * @brief Determines if @c this is less-than another instance.
     *
     * @param[in] o The other iterator instance to compare against @c this.
     * @return @c true if @c this is closer to the beginning of the container
     *         than @p o.
     */
    bool operator<(image_symbol_iterator const& o) const noexcept;

    /**
     * @brief Determines if @c this is less-than or equal-to another instance.
     *
     * @param[in] o The other iterator instance to compare against @c this.
     * @return @c true if @c this is closer to the beginning of the container
     *         than @p o, or if they are equal.
     */
    bool operator<=(image_symbol_iterator const& o) const noexcept;

    /**
     * @brief Determines if @c this is greater-than another instance.
     *
     * @param[in] o The other iterator instance to compare against @c this.
     * @return @c true if @c this is farther from the beginning of the container
     *         than @p o.
     */
    bool operator>(image_symbol_iterator const& o) const noexcept;

    /**
     * @brief Determines if @c this is greater-than another or equal-to
     *        instance.
     *
     * @param[in] o The other iterator instance to compare against @c this.
     * @return @c true if @c this is farther from the beginning of the container
     *         than @p o, or if they are equal.
     */
    bool operator>=(image_symbol_iterator const& o) const noexcept;

    ////////////////////////////////////////
    // Data Access
    ////////////////////////////////////////

    /**
     * @brief Access the underlying container data as a reference type.
     *
     * @return
     *     A const-reference to @c IMAGE_SYMBOL in the container across which
     *     this instance iterates.
     */
    reference operator*() const noexcept;

    /**
     * @brief Access to member data in the underlying container data.
     * @return Access to the @c IMAGE_SYMBOL through a const pointer.
     */
    pointer operator->() const noexcept;

    /**
     * @brief Swap the contents of another iterator with this instance.
     *
     * This swaps the contents of the iterators (not the contents of the data
     * being iterated across).
     *
     * @param[in] o The other iterator instance that will swap with @c this.
     */
    void swap(image_symbol_iterator& o) noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Random Access Iterator Interface
    ////////////////////////////////////////////////////////////////////////////
public:
    ////////////////////////////////////////
    // Data Access
    ////////////////////////////////////////

    /**
     * @brief Access underlying container data by index.
     *
     * This is an "offset" dereference operator.  This means that @c this[0] is
     * equivalent to @c *this, @c this[3] is equivalent to @c *(this+3), and so
     * on.
     *
     * @param[in] count The offset from this current iterator's position.
     * @return A const reference to the desired @c IMAGE_SYMBOL.
     */
    reference operator[](size_t const count) const noexcept;

    ////////////////////////////////////////
    // Increment
    ////////////////////////////////////////

    /**
     * @brief Postfix increment.
     *
     * Make a copy of the current iterator, increment @c this, then return the
     * copy.
     *
     * @return A copy of the iterator before incrementing it.
     */
    image_symbol_iterator operator++(int) noexcept;

    /**
     * @brief Prefix increment.
     *
     * Simply advance the iterator to the next position.
     *
     * @return A reference to the current iterator.
     */
    image_symbol_iterator& operator++() noexcept;

    /**
     * @brief Compound addition.
     *
     * @param[in] count The number of positions to advance the iterator.
     * @return A reference to the current iterator.
     */
    image_symbol_iterator& operator+=(size_t const count) noexcept;

    ////////////////////////////////////////
    // Decrement
    ////////////////////////////////////////

    /**
     * @brief Postfix decrement.
     *
     * Make a copy of the current iterator, decrement @c this, then return the
     * copy.
     *
     * @return A copy of the iterator before decrementing it.
     */
    image_symbol_iterator operator--(int) noexcept;

    /**
     * @brief Prefix decrement.
     *
     * Simply move the iterator to the prior position.
     *
     * @return A reference to the current iterator.
     */
    image_symbol_iterator& operator--() noexcept;

    /**
     * @brief Compound subtraction.
     *
     * @param[in] count The number of positions to decrease the iterator.
     * @return A reference to the current iterator.
     */
    image_symbol_iterator& operator-=(size_t const count) noexcept;

    /**
     * @brief Create a new iterator placed prior to this instance in the
     *        container.
     *
     * @param[in] count The number of positions to decrease the resulting
     *                  iterator relative to this one.
     * @return A new iterator at the desired position.
     */
    image_symbol_iterator operator-(size_t const count) const noexcept;

    /**
     * @brief Determine the distance between two iterators.
     *
     * @param[in] it The iterator against which this will be compared.
     * @return
     *     The number of elements that @p it is behind @c this.  This value is
     *     negative if @c this is closer to the beginning of the container than
     *     @p it.
     */
    difference_type operator-(image_symbol_iterator const& it) const noexcept;

    ////////////////////////////////////////////////////////////////////////////
    /// @name Member Data
    ////////////////////////////////////////////////////////////////////////////
private:
    /// @brief The COFF debugging symbol currently examined by this instance.
    IMAGE_SYMBOL const *m_current_symbol;
};

////////////////////////////////////////////////////////////////////////////////
/// @name Associated Free-functions
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Create a new iterator placed after @p it in the container.
 *
 * @param[in] it    The iterator instance from which to offset.
 * @param[in] count The number of positions to advance the resulting iterator
 *                  relative to this one.
 * @return A new iterator at the desired position.
 */
image_symbol_iterator operator+(
    image_symbol_iterator const& it, size_t const count) noexcept;

/**
 * @brief Create a new iterator placed after @p it in the container (using "n +
 *        this" syntax).
 *
 * @param[in] it    The iterator instance from which to offset.
 * @param[in] count The number of positions to advance the resulting iterator
 *                  relative to this one.
 * @return A new iterator at the desired position.
 */
image_symbol_iterator operator+(
    size_t const count, image_symbol_iterator const& it) noexcept;

} // namespace psystem

namespace std {

/**
 * @brief Define the STL @c swap function for the psystem::image_symbol_iterator
 *        type.
 *
 * This swaps the contents of the iterators (not the contents of the data being
 * iterated across).
 *
 * @param[in,out] it1 The type to receive @p it2's data.
 * @param[in,out] it2 The type to receive @p it1's data.
 */
inline void
swap(
    psystem::image_symbol_iterator& it1,
    psystem::image_symbol_iterator& it2) noexcept
{
    it1.swap(it2);
}

}

#endif // PSYSTEM_IMAGE_SYMBOL_ITERATOR_HPP
