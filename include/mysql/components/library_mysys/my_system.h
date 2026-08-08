/* Copyright (c) 2025, 2026, Oracle and/or its affiliates.

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

#pragma once

#include <cstdint>
#include <string>

using ulonglong = unsigned long long;

/**
  Determine the total number of logical CPUs available to be used by the mysql
  server process.

  This API uses the process affinity to calculate the number of logical CPUs. If
  this method fails, then the API calls the corresponding system API to retrieve
  the number of logical CPUs. If this method fails too, then the API calls the
  C++ standard API hardware_concurrency.

  If the API is unable to determine the number of logical CPUs, then it returns
  0.

  @return number of logical CPUs or 0
*/
uint32_t my_num_vcpus() noexcept;

/**
  Determine if resource limits set by container must be respected and checks the
  correctness of the configurations. Initializes internal state with input

  @param[in]  is_container_aware  true if container config must be respected
  @return true if container configurations are correct, false otherwise
*/
bool init_container_aware(const bool is_container_aware) noexcept;

/**
  Overloaded function for easy use.

  @param[in]  is_container_aware  string considered to be true if "ON", false
  otherwise
  @return true if container configurations are correct, false otherwise
*/
bool init_container_aware(const std::string &is_container_aware) noexcept;

/**
  Release internal state and deinitialize container awareness
*/
void deinit_container_aware() noexcept;

/**
  Determines if container configurations has set resource limits
  @return true if container configuration resource limits
*/
bool has_container_resource_limits();
