// ===-- src/psystem/image_symbol_iterator.cpp ------------ -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Defines functionality for iterating across debuggable symbols.
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

#include "psystem/image_symbol_iterator.hpp"

namespace psystem {

image_symbol_iterator::image_symbol_iterator(address_t const in_ptr) noexcept
    : m_current_symbol(reinterpret_cast<pointer>(in_ptr))
{
}

image_symbol_iterator::image_symbol_iterator(
    image_symbol_iterator const& o) noexcept
    : m_current_symbol(o.m_current_symbol)
{
}

image_symbol_iterator::~image_symbol_iterator() noexcept
{
}

image_symbol_iterator&
image_symbol_iterator::operator=(image_symbol_iterator const& o) noexcept
{
    m_current_symbol = o.m_current_symbol;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////
// Basic Iterator Interface
////////////////////////////////////////////////////////////////////////////////

bool
image_symbol_iterator::operator==(
    image_symbol_iterator const& other) const noexcept
{
    // For larger comparisons, this short-circuit is more interesting.  Here,
    // the member comparison is trivial, and if this == &other the member data
    // will already be ready for comparison (i.e., we don't have to worry about
    // going to RAM for other.m_current_symbol because it will be in the
    // d-cache).
//  if (this == &other) return true;

    return other.m_current_symbol == m_current_symbol;
}

bool
image_symbol_iterator::operator!=(
    image_symbol_iterator const& other) const noexcept
{
    // Define inequality in terms of equality...
    return !(*this == other);
}

bool
image_symbol_iterator::operator<(image_symbol_iterator const& o) const noexcept
{
    // Define all other comparisons in terms of less-than...
    return (m_current_symbol < o.m_current_symbol);
}

bool
image_symbol_iterator::operator<=(image_symbol_iterator const& o) const noexcept
{
    return !(o < *this);
}

bool
image_symbol_iterator::operator>(image_symbol_iterator const& o) const noexcept
{
    return (o < *this);
}

bool
image_symbol_iterator::operator>=(image_symbol_iterator const& o) const noexcept
{
    return !(*this < o);
}

image_symbol_iterator::reference
image_symbol_iterator::operator*() const noexcept
{
    return *m_current_symbol;
}

image_symbol_iterator::pointer
image_symbol_iterator::operator->() const noexcept
{
    return m_current_symbol;
}

void
image_symbol_iterator::swap(image_symbol_iterator& o) noexcept
{
    std::swap(m_current_symbol, o.m_current_symbol);
}

////////////////////////////////////////////////////////////////////////////////
// Random Access Iterator Interface
////////////////////////////////////////////////////////////////////////////////

image_symbol_iterator::reference
image_symbol_iterator::operator[](size_t const count) const noexcept
{
    return m_current_symbol[count];
}

image_symbol_iterator
image_symbol_iterator::operator++(int) noexcept
{
    image_symbol_iterator ret = *this;
    this->operator++();
    return ret;
}

image_symbol_iterator&
image_symbol_iterator::operator++() noexcept
{
    ++m_current_symbol;
    return *this;
}

image_symbol_iterator&
image_symbol_iterator::operator+=(size_t const count) noexcept
{
    m_current_symbol += count;
    return *this;
}

image_symbol_iterator
image_symbol_iterator::operator--(int) noexcept
{
    image_symbol_iterator ret = *this;
    this->operator--();
    return ret;
}

image_symbol_iterator&
image_symbol_iterator::operator--() noexcept
{
    --m_current_symbol;
    return *this;
}

image_symbol_iterator&
image_symbol_iterator::operator-=(size_t const count) noexcept
{
    m_current_symbol -= count;
    return *this;
}

image_symbol_iterator
image_symbol_iterator::operator-(size_t const count) const noexcept
{
    image_symbol_iterator ret = *this;
    ret -= count;
    return ret;
}

image_symbol_iterator::difference_type
image_symbol_iterator::operator-(image_symbol_iterator const& it) const noexcept
{
    return this->m_current_symbol - it.m_current_symbol;
}

////////////////////////////////////////////////////////////////////////////////
// Associated Free-functions
////////////////////////////////////////////////////////////////////////////////

image_symbol_iterator
operator+(image_symbol_iterator const& it, size_t const count) noexcept
{
    image_symbol_iterator ret = it;
    ret += count;
    return ret;
}

image_symbol_iterator
operator+(size_t const count, image_symbol_iterator const& it) noexcept
{
    image_symbol_iterator ret = it;
    ret += count;
    return ret;
}

} // namespace psystem
