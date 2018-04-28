// ===-- include/psystem/framework/iomanip.hpp ------------ -*- C++ -*- --=== //
// Copyright (c) 2014 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Augment the C++ standard header file, @c iomanip.
 *
 * @author  Matt Bisson
 * @date    18 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#pragma once
#ifndef PSYSTEM_FRAMEWORK_IOMANIP_HPP
#define PSYSTEM_FRAMEWORK_IOMANIP_HPP

#include "platform.hpp"
#include <iostream>
#include <iomanip>

#include "stack_allocated.hpp"

namespace psystem {

/**
 * @brief Formatting facilities for @c std::ostream output.
 *
 * These are tags that have little or no run-time cost, and will format data as
 * it sends it directly to the output stream.  An example program:
 *
@code{.cpp}
#include <iostream>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <psystem/framework/iomanip.hpp>
using psystem::framework::mask;

int main()
{
    stat fl_info = { 0 };
    if (0 == stat("/", &fl_info))
    {
        std::cout << "File permissions are: " << mask(fl_info.st_mode) << '\n';
    }

    return 0;
}
@endcode
 *
 * ... which outputs:
@verbatim
File permissions are: (0x000001ED)
@endverbatim
 *
 * @author  Matt Bisson
 * @date    18 August, 2014
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
namespace format {

/// @brief Internal structures used for the psystem::format namespace
namespace internal {

} // namespace internal
} // namespace format
} // namespace psystem

namespace psystem {
namespace format {

////////////////////////////////////////////////////////////////////////////////
/// @name Bitmask Pretty-print
////////////////////////////////////////////////////////////////////////////////

namespace internal {

/**
 * @brief Internal, intermediate result of the mask() function.
 * @tparam T The template parameter must be an integral type.
 *
 * @internal @note
 *     This class is only available on the stack.  It cannot use
 *     psystem::stack_allocated because you cannot have an initializer-list
 *     constructed struct with a base class that is not also initializer-list
 *     constructed.
 */
template <typename T>
struct mask_ostream_output_tag
{
    static_assert(
        std::is_integral<T>::value,
        "Mask ostream output must be an integral type.");

    T value; ///< @brief The value to output

    /// @cond
    static void  operator delete  (void*)  = delete;
    static void  operator delete[](void*)  = delete;
    static void *operator new     (size_t) = delete;
    static void *operator new[]   (size_t) = delete;
    /// @endcond
};

} // namespace internal

/**
 * @brief Mark output stream data to be formatted as a bit-mask.
 *
 * Depending on the size of the input data, this will be sent to an
 * @c std::ostream instance using @c operator<< as a zero-padded hexidecimal
 * number.  It will be surrounded by parentheses.  For example:
 *
@code{.cpp}
    short int const flags = 0x13;
    std::cout << "The flags are: " << mask(flags) << '\n';
@endcode
 *
 * ...produces the output:
 *
@verbatim
The flags are: (0x0013)
@endverbatim
 *
 * @tparam T The parameter type must be an intergral type.
 * @param[in] m The bit-mask to format.
 * @return Internal implementation may vary.
 */
template <typename T>
inline internal::mask_ostream_output_tag<T>
mask(T const m) noexcept
{
    return internal::mask_ostream_output_tag<T>{m};
}

////////////////////////////////////////////////////////////////////////////////
/// @name Hexadecimal Pretty-print
////////////////////////////////////////////////////////////////////////////////

/// @brief Internal implementation details for pretty-printing
namespace internal {

/**
 * @brief Internal, intermediate result of the psystem::format::hex() function.
 * @tparam T The template parameter must be an integral type.
 *
 * @internal @note
 *     This class is only available on the stack.  It cannot use
 *     psystem::stack_allocated because you cannot have an initializer-list
 *     constructed struct with a base class that is not also initializer-list
 *     constructed.
 */
template <typename T>
struct hex_ostream_output_tag
{
    static_assert(
        std::is_integral<T>::value,
        "Hex ostream output must be an integral type.");

    T value; ///< @brief The value to output

    /// @cond
    static void  operator delete  (void*)  = delete;
    static void  operator delete[](void*)  = delete;
    static void *operator new     (size_t) = delete;
    static void *operator new[]   (size_t) = delete;
    /// @endcond
};

} // internal

/**
 * @brief Mark output stream data to be formatted as a hexadecimal number.
 *
 * This will be sent to an @c std::ostream instance using @c
 * operator<< as an hexidecimal number.  For example:
 *
@code{.cpp}
    int32_t const number = 32780;
    std::cout << "The number is: " << hex(number) << '\n';
@endcode
 *
 * ...produces the output:
 *
@verbatim
The number is: 0x800C
@endverbatim
 *
 * @tparam T The parameter type must be an intergral type.
 * @param[in] v The numerical data to format.
 * @return Internal implementation may vary.
 */
template <typename T>
inline internal::hex_ostream_output_tag<T>
hex(T const v) noexcept
{
    return internal::hex_ostream_output_tag<T>{v};
}

////////////////////////////////////////////////////////////////////////////////
/// @name Pointer Pretty-print
////////////////////////////////////////////////////////////////////////////////

namespace internal {

/**
 * @brief Internal, intermediate result of the ptr() function.
 *
 * @internal @note
 *     This class is only available on the stack.  It cannot use
 *     psystem::stack_allocated because you cannot have an initializer-list
 *     constructed struct with a base class that is not also initializer-list
 *     constructed.
 */
struct ptr_ostream_output_tag
{
    void const *value; ///< @brief The value to output

    /// @cond
    static void  operator delete  (void*)  = delete;
    static void  operator delete[](void*)  = delete;
    static void *operator new     (size_t) = delete;
    static void *operator new[]   (size_t) = delete;
    /// @endcond
};

}

/**
 * @brief Mark output stream data to be formatted as a memory address.
 *
 * This function only applies to data that can be converted implicitly to a
 * @c void* data-type.  The output will be zero-padded to the length of
 * addresses on the target architecture.  For example:
 *
@code{.cpp}
    std::string const test = "test!";
    std::cout << "The address is: " << ptr(test.c_str()) << '\n';
@endcode
 *
 * ...produces output like:
 *
@verbatim
The address is: 0x0000007FF018E924
@endverbatim
 *
 * @param[in] p The pointer / address to format.
 * @return Internal implementation may vary.
 */
inline internal::ptr_ostream_output_tag
ptr(void const *p) noexcept
{
    return internal::ptr_ostream_output_tag{p};
}

/**
 * @brief Mark output stream data to be formatted as a memory address.
 *
 * This function only applies to input that can be converted to the
 * psystem::address_t type. The output will be zero-padded to the length of
 * addresses on the target architecture.  For example:
 *
@code{.cpp}
    psystem::address_t const addr = get_some_address();
    std::cout << "The address is: " << ptr(addr) << '\n';
@endcode
 *
 * ...produces output like:
 *
@verbatim
The address is: 0x0000001FE0384BB4
@endverbatim
 *
 * @param[in] p The address to format.
 * @return Internal implementation may vary.
 */
inline internal::ptr_ostream_output_tag
ptr(psystem::address_t const p) noexcept
{
    return internal::ptr_ostream_output_tag{reinterpret_cast<void*>(p)};
}

/**
 * @brief Mark a function pointer to be formatted to an output stream as a
 *        memory address.
 *
 * This is a specialization to implicitly convert function pointer types to
 * printable memory addresses. The output will be zero-padded to the length of
 * addresses on the target architecture.  For example:
 *
@code{.cpp}
    std::cout << "The address is: " << ptr(&printf) << '\n';
@endcode
 *
 * ...produces output like:
 *
@verbatim
The address is: 0x00000080DD198C02
@endverbatim
 *
 * @note
 *     It is generally not a good idea to cast function pointers to @c void* and
 *     back because one doesn't know where the memory being pointed to actually
 *     physically resides.  Some hardware may have separate locations for code
 *     segments and data segments.  Since we're not actually accessing the
 *     memory, but are more interested in the pointer value itself, it's OK to
 *     reinterpret the bytes as a @c void*.
 *
 * @tparam R       The function's return type.
 * @tparam Args... The full argument list for the function.
 *
 * @param[in] p The function pointer format.
 * @return Internal implementation may vary.
 */
template <typename R, typename... Args>
inline internal::ptr_ostream_output_tag
ptr(R(*p)(Args...)) noexcept
{
    return internal::ptr_ostream_output_tag{reinterpret_cast<void*>(p)};
}

////////////////////////////////////////////////////////////////////////////////
/// @name Boolean Value Pretty-print
////////////////////////////////////////////////////////////////////////////////

namespace internal {

/**
 * @brief Internal, intermediate result of the mask() function.
 *
 * This class could be expanded to accept any data-type that implicitly converts
 * to a boolean value, but it's not required at the moment, so we will leave it
 * as a non-templated class.
 *
 * @internal @note
 *     This class is only available on the stack.  It cannot use
 *     psystem::stack_allocated because you cannot have an initializer-list
 *     constructed struct with a base class that is not also initializer-list
 *     constructed.
 */
struct swtch_ostream_output_tag
{
    bool value; ///< @brief The value to output

    /// @cond
    static void  operator delete  (void*)  = delete;
    static void  operator delete[](void*)  = delete;
    static void *operator new     (size_t) = delete;
    static void *operator new[]   (size_t) = delete;
    /// @endcond
};

} // namespace internal

/**
 * @brief Mark output stream data to be formatted as an on/off switch.
 *
 * Output simply translates a boolean value to a more human-friendly format.
 * For example:
 *
@code{.cpp}
    bool const is_true = true;
    std::cout << "The is_true switch is set to: " << swtch(is_true) << '\n';
@endcode
 *
 * ...produces the output:
 *
@verbatim
The is_true switch is set to: on
@endverbatim
 *
 * @param[in] s The boolean value to format.
 * @return Internal implementation may vary.
 */
inline internal::swtch_ostream_output_tag
swtch(bool const s) noexcept
{
    return internal::swtch_ostream_output_tag{s};
}

/// @}

} // namespace format
} // namespace psystem

/**
 * @brief Free function to format boolean values for @c std::ostream.
 *
 * @param[in,out] out The output stream.
 * @param[in]     v   The data to format.
 * @return The output stream after output.
 */
inline std::ostream&
operator<<(
    std::ostream& out,
    psystem::format::internal::swtch_ostream_output_tag&& v)
{
    return out << ((v.value) ? "on" : "off");
}

/**
 * @brief Free function to format hexadecimal numbers for @c std::ostream.
 *
 * @tparam T The data-typed passed to psystem::format::hex()
 * @param[in,out] out The output stream.
 * @param[in]     v   The data to format.
 * @return The output stream after output.
 */
template <typename T>
inline std::ostream&
operator<<(
    std::ostream& out,
    psystem::format::internal::hex_ostream_output_tag<T>&& v)
{
    return out << "0x" << std::hex << v.value << std::dec;
}

/**
 * @brief Free function to format bit-masks for @c std::ostream.
 *
 * @tparam T The data-typed passed to psystem::format::mask()
 * @param[in,out] out The output stream.
 * @param[in]     m   The data to format.
 * @return The output stream after output.
 */
template <typename T>
inline std::ostream&
operator<<(
    std::ostream& out,
    psystem::format::internal::mask_ostream_output_tag<T>&& m)
{
    return out
        << "(0x"
        << std::setfill('0') << std::setw(sizeof(T)*2) << std::hex << m.value
        << std::setfill(' ') << std::dec
        << ')';
}

/**
 * @brief Free function to format pointers for @c std::ostream.
 *
 * @param[in,out] out The output stream.
 * @param[in]     p   The data to format.
 * @return The output stream after output.
 */
inline std::ostream&
operator<<(
    std::ostream& out,
    psystem::format::internal::ptr_ostream_output_tag&& p)
{
    return out
        << "0x"
        << std::setfill('0') << std::setw(sizeof(void*)*2) << std::hex
        << reinterpret_cast<psystem::integral_address_t>(p.value)
        << std::setfill(' ') << std::dec;
}

#endif // PSYSTEM_FRAMEWORK_IOMANIP_HPP
