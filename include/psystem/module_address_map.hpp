// ===-- include/psystem/module_address_map.hpp ----------- -*- C++ -*- --=== //
// Copyright (c) 2015 Matt Bisson.  All rights reserved.

/**
 * @file
 * @brief Declare a type to map addresses to modules in a useful sort-order.
 *
 * @author  Matt Bisson
 * @date    17 November, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 *
 * @copyright
 *     This file is distributed under the University of Illinois Open Source
 *     License.  See LICENSE.TXT for details.
 */

#ifndef PSYSTEM_MODULE_ADDRESS_MAP_HPP
#define PSYSTEM_MODULE_ADDRESS_MAP_HPP

#include <map>
#include "module_info.hpp"

namespace psystem {

/**
 * @brief A mapping of module base-addresses to the psystem::module_info.
 *
 * This type maps key representing a shared library or executable image to the
 * psystem object that encapsulates its information.  Keys are ordered from
 * largest to smallest so that @c std::lower_bound can easily determine where a
 * symbol's address exist in terms of the loaded modules.
 *
 * @author  Matt Bisson
 * @date    17 November, 2015
 * @since   PSystem 2.0
 * @version PSystem 2.0
 */
using module_address_map = std::map<
    psystem::address_t,
    psystem::module_info,
    std::greater<psystem::address_t> >;

} // namespace psystem

#endif // PSYSTEM_MODULE_ADDRESS_MAP_HPP
