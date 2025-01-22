/* Copyright (c) 2015, 2024, Oracle and/or its affiliates.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.

   This program is designed to work with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL hereby grant you an additional
   permission to link the program and your derivative works with the
   separately licensed software that they have either included with
   the program or referenced in the documentation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#include <ios>
#include <sstream>

#include "plugin/group_replication/include/member_version.h"

Member_version::Member_version(unsigned int version) {
  this->version = version & 0x00FFFFFF;
}

uint32 Member_version::get_version() const { return this->version; }

const std::string Member_version::get_version_string() const {
  std::stringstream member_version;
  member_version << std::hex << get_major_version() << "."
                 << get_minor_version() << "." << get_patch_version();
  return member_version.str();
}

uint32 Member_version::get_major_version() const { return this->version >> 16; }

uint32 Member_version::get_minor_version() const {
  return (this->version >> 8) & 0xff;
}

uint32 Member_version::get_patch_version() const {
  return this->version & 0xff;
}

bool Member_version::operator==(const Member_version &other) const {
  return get_version() == other.get_version();
}

bool Member_version::operator!=(const Member_version &other) const {
  return get_version() != other.get_version();
}

bool Member_version::operator<(const Member_version &other) const {
  if (*this == other) return false;

  if (get_major_version() < other.get_major_version()) {
    return true;
  } else if (get_major_version() > other.get_major_version()) {
    return false;
  } else  // major version are the same
  {
    if (get_minor_version() < other.get_minor_version()) {
      return true;
    } else if (get_minor_version() > other.get_minor_version()) {
      return false;
    } else  // minor version are the same
    {
      if (get_patch_version() < other.get_patch_version()) {
        return true;
      }
    }
  }

  return false;
}

bool Member_version::operator>(const Member_version &other) const {
  if (*this == other) return false;

  return !(*this < other);
}

bool Member_version::operator>=(const Member_version &other) const {
  return (*this == other || *this > other);
}

bool Member_version::operator<=(const Member_version &other) const {
  return (*this == other || *this < other);
}

/*
 * For any version return the equivalent version that can be compared against for doing primary GR elections.
 *
 * - original behaviour was to only allow the lowest version to be elegible to be primary.
 * - new behaviour:
 *   - treat 8.0.35+ versions to be equivalent
 *   - treat 8.4.* versions to be equivalent
 *   - treat 9.X.* versions to be equivalent
 *   - when different major/minor versions are involved always choose the lowest equivalent version.
 */
int32 gr_primary_min_equivalent_version(const Member_version &version) {
  if (version.get_major_version() == 8) {
    if (version.get_minor_version() == 0) {
      if (version.get_patch_version() < 35) {
        return version.get_version(); // return the original version
      }
      return ((version.get_version() & 0x00FFFF00) | 35); // return 8.0.35
    }
    if (version.get_minor_version() == 4) {
      return (version.get_version() & 0x00FFFF00); // return 8.4.0
    }
    if (version.get_major_version() == 9) {
      return (version.get_version() & 0x00FFFF00); // return 9.X.0
    }
  }
  return (version.get_version()); // return the original version
}
