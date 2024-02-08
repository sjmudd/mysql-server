/*
 * Copyright (c) 2015, 2023, Oracle and/or its affiliates.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0,
 * as published by the Free Software Foundation.
 *
 * This program is designed to work with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have either included with
 * the program or referenced in the documentation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#ifndef XPL_MYSQL_FUNCTION_NAMES_H_
#define XPL_MYSQL_FUNCTION_NAMES_H_

#include <cstring>
#include <string>

namespace xpl {

struct Is_less {
  bool operator()(const char *const pattern, const char *const source) const {
    return std::strcmp(pattern, source) < 0;
  }
};

template <typename T, unsigned size>
inline const T *get_array_end(T (&array)[size]) {
  return array + size;
}

bool is_native_mysql_function(const std::string &name);

}  // namespace xpl

#endif  // XPL_MYSQL_FUNCTION_NAMES_H_
